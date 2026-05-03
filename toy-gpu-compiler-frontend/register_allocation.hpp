#ifndef REGISTER_ALLOC_HPP
#define REGISTER_ALLOC_HPP

#include "tac_nodes.hpp"
#include "liveness_analysis.hpp"

#include <vector>
#include <unordered_map>
#include <algorithm>

struct AllocationTable {
    std::vector<int> in_register;
    std::vector<int> in_memory; 
};

// potentially add max memory size
class RegisterAllocation {
    int _avail_pregisters; // this is the # of physical registers available, but also the size of the sliding window in the linear scan algorithm
    int _memory_offset = 0; // will be used in instruction selection
    std::set<int> _active_temps_in_window; // if size of this > _avail_pregisters then spill
    // std::vector<int> _allocation_table{-1,-1}; 
    std::vector<AllocationTable> _allocations; // only DS that has to be modified/ added to
    std::vector<std::unique_ptr<TACNode>> _instructions; 
    std::unordered_map<int, LiveRange> _live_ranges; 
    std::vector<LivenessInfo> _live_info_at_instruction;

    enum class TempState {
        NEW,
        IN_REGISTER,
        IN_MEMORY,
        DEAD
    };

    std::unordered_map<int, TempState> _temp_state; 
    
    RegisterAllocation(int n, const std::vector<std::unique_ptr<TACNode>>& tac_nodes, const std::unordered_map<int, LiveRange> ranges, std::vector<LivenessInfo> live_info_at_instruction) : _avail_pregisters{n}, _instructions{tac_nodes}, _live_ranges{ranges}, _live_info_at_instruction{live_info_at_instruction} {};

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
    }

    void allocate() {
        // _active_temps_in_window is empty
        // _avail_pregisters = 2, also n
        // _memory_offset = 0; 

        // push_back to _active_temps_in_window
        size_t i = 0; 

        while (i < _instructions.size()) {
            // remove all un-live temps from the active-temps set
            for (auto& active_temp : _active_temps_in_window) {
                LiveRange cur_temp_range = _live_ranges[active_temp]; 
                if (cur_temp_range.end <= i) _active_temps_in_window.erase(active_temp); 
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
                    auto it = _active_temps_in_window.insert(after);
                    if (it.second) inserted_it = it.first;
                }
            }

            std::vector<int> i_instruction_registers;
            std::vector<int> i_instruction_memory;

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
                AllocationTable prev_alloca = _allocations[i-1]; 
                int longer_living_temp; 

                // TODO: should be a for loop to check which temp lives longer
                // check if r1 or r2 temp lives longer
                if (_live_ranges[prev_alloca.in_register[0]].end > _live_ranges[prev_alloca.in_register[1]].end) {
                    longer_living_temp = prev_alloca.in_register[0];
                } else {
                    longer_living_temp = prev_alloca.in_register[1];
                }
                // TODO: store memory offset occured at instruction i; 
                // save to memory
                i_instruction_memory.push_back(longer_living_temp);
                i_instruction_registers = prev_alloca.in_register;
                // delete from the registers 
                auto it = find(i_instruction_registers.begin(), i_instruction_registers.end(), longer_living_temp);
                i_instruction_registers.erase(it); 

                // figure out which register is the temp to insert into i_instruction_registers
                i_instruction_registers.push_back(*inserted_it);
                
                // create the allocation table for this instruction i
                _allocations.push_back(AllocationTable{.in_register = i_instruction_registers, .in_memory = i_instruction_memory});
            } else {
                // simply add to instruction registers and memory
                if (i == 0) {
                    // make a vector from a set
                    i_instruction_registers.assign(_active_temps_in_window.begin(), _active_temps_in_window.end()); 
                    _allocations.push_back(AllocationTable{.in_register = i_instruction_registers, .in_memory = {}}); 
                } else {
                    AllocationTable prev_alloca = _allocations[i-1]; 

                }
            }
            // somewhere erase the temp we put in memory from the active_temps_in_registers
            ++i;
        }
    }
    // use -1 to indicate a register is empty

};

#endif