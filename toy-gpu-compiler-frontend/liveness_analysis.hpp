#ifndef LIVENESS_ANALYSIS_HPP
#define LIVENESS_ANALYSIS_HPP

#include "tac_nodes.hpp"

#include <set>
#include <vector>
#include <memory>
#include <set>
#include <cassert>
#include <unordered_map>

struct LivenessInfo {
    std::set<int> live_before; 
    std::set<int> live_after; 
};

struct LiveRange {
    int start; 
    int end; 
};

class LivenessAnalysis {
public: 
    std::unordered_map<int, LiveRange> temp_to_live_range; // use this when register allocating

    std::vector<LivenessInfo> instruction_liveness; //liveness at each instruction i. not specific to temps 

    void analyze(const std::vector<std::unique_ptr<TACNode>>& tac_instructions) {
        size_t n = tac_instructions.size(); 
        instruction_liveness.resize(n); 

        // track last use of temporary for range calculation

        std::set<int> live_set; 

        for (int i = n - 1; i >= 0; --i) {
            instruction_liveness[i].live_after = live_set;

            std::vector<int> defs = get_definition(tac_instructions[i].get());
            std::vector<int> uses = get_uses(tac_instructions[i].get());

            // remove all definitions from the live set at this time, they aren't referenced anywhere above (chain starts at the definition, so the temporary will never show up in a line above)
            for (int def : defs) {
                live_set.erase(def); 
            }

            // insert all uses into the live set, as they are still live. A use in line i means there are potentially more uses in earlier lines, and definitely a def. 
            for (int use : uses) {
                live_set.insert(use);
            }
            instruction_liveness[i].live_before = live_set; 
        }
        // compute ranges of each temporary from the liveness info at each line
        compute_live_ranges(); 
    }

private: 
    void compute_live_ranges() {
        std::unordered_map<int,int> first_use;
        std::unordered_map<int,int> last_use; 
        for (size_t i = 0; i < instruction_liveness.size(); ++i) {
            for (int temp : instruction_liveness[i].live_before) {
                if (first_use.find(temp) == first_use.end()) {
                    first_use[temp] = i; 
                }
                last_use[temp] = i; 
            }
            for (int temp : instruction_liveness[i].live_after) {
                if (first_use.find(temp) == first_use.end()) {
                    first_use[temp] = i; 
                }
                last_use[temp] = i; 
            }
        }
        temp_to_live_range.clear(); 
        for (const auto& [temp, first] : first_use) {
            temp_to_live_range[temp] = {first, last_use[temp]}; 
        }
        return; 
    }

    bool is_temp(const Operand& op) {
        return std::holds_alternative<Temp>(op); 
    }

    std::vector<int> get_definition(TACNode* instr) {
        std::vector<int> defs; 

        if (auto* tac_constant = dynamic_cast<TACConstant*>(instr)) {
            defs.push_back(tac_constant->_dest.identifer); 
        } else if (auto* tac_binary = dynamic_cast<TACBinaryOp*>(instr)) {
            defs.push_back(tac_binary->_dest.identifer); 
        } else if (auto* tac_load = dynamic_cast<TACLoad*>(instr)) {
            defs.push_back(tac_load->_dest_temp.identifer);
        } else if (auto* tac_store = dynamic_cast<TACStore*>(instr)) {}
        return defs;
    }

    std::vector<int> get_uses(TACNode* instr) {        
        std::vector<int> uses;  

        if (auto* tac_binary = dynamic_cast<TACBinaryOp*>(instr)) {
            if (is_temp(tac_binary->_operand1)) uses.push_back(std::get<Temp>(tac_binary->_operand1).identifer); //TODO doesn't fully work I think, need to also factor in TACVariable
            if (is_temp(tac_binary->_operand2)) uses.push_back(std::get<Temp>(tac_binary->_operand2).identifer);
        } else if (auto* tac_store = dynamic_cast<TACStore*>(instr)) {
            if (is_temp(tac_store->_temporary)) uses.push_back(tac_store->_temporary.identifer);
        }
        return uses; 
    } 
};

#endif