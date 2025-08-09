#pragma once
#include <cstddef>

struct SpikeEvent {
    int deliveryTime; // simulation timestep when event should apply
    size_t target;    // neuron to receive the input
    double weight;    // voltage increment
};
