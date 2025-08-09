#pragma once
#include <cstddef>

struct Synapse {
    size_t from;      // index of source neuron
    size_t to;        // index of target neuron
    double weight;    // synaptic strength (+ for excitatory, - for inhibitory)
    int delaySteps;   // delay in timesteps before spike arrives
};
