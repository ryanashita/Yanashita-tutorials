#ifndef REGISTER_ALLOC_HPP
#define REGISTER_ALLOC_HPP

#include "tac_nodes.hpp"
#include "liveness_analysis.hpp"

#include <vector>
#include <unordered_map>
#include <algorithm>

struct AllocationTable {
    bool spill_happened;
    std::unordered_map<int,int> in_register; // register#:temp_number
    std::unordered_map<int,int> in_memory; // memory offset:temp_number
};

// potentially add max memory size
class RegisterAllocation {
public: 
    std::vector<AllocationTable> _allocations; // only DS that has to be modified/ added to

    RegisterAllocation(int n, const std::vector<std::unique_ptr<TACNode>>& tac_nodes, const std::unordered_map<int, LiveRange>& ranges, std::vector<LivenessInfo>& live_info_at_instruction) : _avail_pregisters{n}, _instructions{tac_nodes}, _live_ranges{ranges}, _live_info_at_instruction{live_info_at_instruction} {};

    void allocate() {
        std::cout << "debug line 25" << std::endl; 
        // _active_temps_in_window is empty
        // _avail_pregisters = 2, also n
        // _memory_offset = 0; 

        // push_back to _active_temps_in_window
        size_t i = 0; 

        while (i < _instructions.size()) {
            std::cout << "debug line 34: instruction " << i << std::endl; 
            // remove all un-live temps from the active-temps set
            for (auto& active_temp : _active_temps_in_window) {
                LiveRange cur_temp_range = _live_ranges.at(active_temp); 
                if (cur_temp_range.end <= i) _active_temps_in_window.erase(active_temp); 
                _temp_state[active_temp] = TempState::DEAD; 
            }
            // live_after is important here. add all non-duplicate temps to the current active temporaries (active temps need a register)
            // non-duplicate because a duplicate just means that the temp was live before and still remains alive at this instruction
            // only one temp gets added to the active temp window at a time bc only one temp is created in a line
            std::set<int>::iterator inserted_it; 
            for (auto& after : _live_info_at_instruction[i].live_after) {

                bool needs_register = false; 

                switch (_temp_state[after]) {
                    case TempState::IN_REGISTER:
                        needs_register = true; 
                        break;
                    case TempState::IN_MEMORY:
                        // check if this temp 'after' is needed in this instruction    
                        if (is_used_at_instruction(_instructions[i].get(), after)) {
                            needs_register = true; // triggers a load
                            _temp_state[after] = TempState::IN_REGISTER;
                        }
                        break;
                    case TempState::DEAD:
                        break; 
                    default:
                        needs_register = true;
                        _temp_state[after] = TempState::IN_REGISTER;
                        break;
         
                }
                if (needs_register) {
                    // saving the iterator and not just temp 'after' bc have to confirm that 'after' wasn't a duplicate and does indeed need to be placed in a register
                    auto it = _active_temps_in_window.insert(after);
                    if (it.second) inserted_it = it.first;
                }
            }
            std::cout << "debug line 74" << std::endl; 

            std::unordered_map<int,int> i_instruction_registers;
            std::unordered_map<int,int> i_instruction_memory;

            // if the first instruction, don't need to / can't look back at previous allocation
            if (i == 0) {
                std::cout << "debug line 81" << std::endl; 
                int j = 0; 
                for (auto& temp : _active_temps_in_window) {
                    i_instruction_registers[j] = temp; 
                    ++j;
                }
                _allocations.push_back(AllocationTable{.spill_happened = false,.in_register = i_instruction_registers, .in_memory = {}}); 
                ++i;
                continue;
            }
            std::cout << "debug line 91" << std::endl; 
            // the rest of the logic in the function is for instructions i > 0. 

            // only add values from registers if they are still live
            AllocationTable prev_alloca = _allocations[i-1];
            std::cout << "debug line 94" << std::endl;
            for (auto& [key,value] : prev_alloca.in_register) {
                if (_temp_state[value] != TempState::DEAD) {
                     i_instruction_registers[key] = value; 
                }
            }
            std::cout << "debug line 100" << std::endl; 

            // check if more actives than physical register available. if yes, must swap values in registers and spill
            // TODO: find a way to save that a spill occured here
            // example: if there are 3 temps looking for registers and two physical registers available, 1 temp has to be spilled to memory. 
            if (_active_temps_in_window.size() > _avail_pregisters) {
                /* 
                    the logic in this conditional only runs if 
                    there are more temps that need registers than 
                    the # of available physical registers. At
                    three-address code line 0, it is impossible for 
                    this to occur, and this it is ok to say i-1 below. 
                */
                if (prev_alloca.in_register.size() > _avail_pregisters) std::exit; 
                
                // check if r1 or r2 temp lives longer
                int longest_living_temp = -1; 
                int reg_being_modified; 
                for (auto& [key,value] : prev_alloca.in_register) {
                    if (_live_ranges.at(value).end > longest_living_temp) {
                        longest_living_temp = value; 
                        reg_being_modified = key;
                    }
                }

                // save to memory 
                i_instruction_memory[_memory_offset] = longest_living_temp;
                _temp_state[longest_living_temp] = TempState::IN_MEMORY;
                ++_memory_offset;
        
                // change the temp at the register that should be modified, to the inserted
                i_instruction_registers = prev_alloca.in_register;
                i_instruction_registers[reg_being_modified] = *inserted_it;

                // i_instruction_registers.assign(prev_alloca.in_register.begin(), prev_alloca.in_register.end());
                // auto it = find(i_instruction_registers.begin(), i_instruction_registers.end(), longest_living_temp);
                // i_instruction_registers.erase(it); 

                // create the allocation table for this instruction i
                _allocations.push_back(AllocationTable{.spill_happened = true,.in_register = i_instruction_registers, .in_memory = i_instruction_memory});
            } else {
                // simply add to instruction registers and memory
                if (i == 0) {
                    // make a vector from a set
                    int j = 0; 
                    for (auto& temp : _active_temps_in_window) {
                        i_instruction_registers[j] = temp; 
                        ++j;
                    }
                    _allocations.push_back(AllocationTable{.spill_happened = false,.in_register = i_instruction_registers, .in_memory = {}}); 
                } else {
                    AllocationTable prev_alloca = _allocations[i-1]; 

                    for (size_t k = 0; k < _avail_pregisters; ++k) {
                        if (prev_alloca.in_register.find(k) == prev_alloca.in_register.end()) {
                            prev_alloca.in_register[k] = *inserted_it;
                        }
                    }
                    _allocations.push_back(AllocationTable{.spill_happened = false,.in_register = prev_alloca.in_register, .in_memory = {}}); 
                }
            }
            ++i;
        }
    }

private:
    int _avail_pregisters; // this is the # of physical registers available, but also the size of the sliding window in the linear scan algorithm
    int _memory_offset = 0; // will be used in instruction selection
    std::set<int> _active_temps_in_window; // if size of this > _avail_pregisters then spill
    // std::vector<int> _allocation_table{-1,-1}; 
    
    const std::vector<std::unique_ptr<TACNode>>& _instructions; 
    const std::unordered_map<int, LiveRange>& _live_ranges; 
    const std::vector<LivenessInfo>& _live_info_at_instruction;

    enum class TempState {
        IN_REGISTER, // the default
        IN_MEMORY, 
        DEAD
    };

    std::unordered_map<int, TempState> _temp_state; 
    
    bool is_used_at_instruction(TACNode* instr, int temp) {
        if (auto* tac_binary = dynamic_cast<TACBinaryOp*>(instr)) {
            auto check_operand = [temp](const Operand& op) -> bool {
                if (std::holds_alternative<Temp>(op)) {
                    return std::get<Temp>(op).identifer == temp; 
                }
                return false; 
            };
            return check_operand(tac_binary->_operand1) || check_operand(tac_binary->_operand2); 
        } 

        if (auto* tac_store = dynamic_cast<TACStore*>(instr)) {
            return tac_store->_temporary.identifer == temp; 
        }
        
        return false; 
    }
};

#endif