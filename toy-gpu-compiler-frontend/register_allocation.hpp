#ifndef REGISTER_ALLOC_HPP
#define REGISTER_ALLOC_HPP

#include "tac_nodes.hpp"
#include "liveness_analysis.hpp"

#include <vector>

class RegisterAllocation {
    int _k = 2; // this is the # of physical registers available, but also the size of the sliding window in the linear scan algorithm

    std::vector<int> _active_temps_in_window; 
};

#endif