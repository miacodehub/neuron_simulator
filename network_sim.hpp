#pragma once
#include "neuron.hpp"
#include "synapse.hpp"
#include "spike_event.hpp"
#include <vector>
#include <deque>

void runNetworkStep(
    std::vector<Neuron>& neurons,
    const std::vector<Synapse>& synapses,
    std::deque<SpikeEvent>& spikeQueue,
    int currentTime,
    double dt
);
