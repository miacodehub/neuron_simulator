#include "network_sim.hpp"

void runNetworkStep(
    std::vector<Neuron>& neurons,
    const std::vector<Synapse>& synapses,
    std::deque<SpikeEvent>& spikeQueue,
    int currentTime,
    double dt
) {
    while (!spikeQueue.empty() && spikeQueue.front().deliveryTime <= currentTime) {
        auto ev = spikeQueue.front();
        spikeQueue.pop_front();
        neurons[ev.target].applyInput(ev.weight);
    }

    for (size_t i = 0; i < neurons.size(); ++i) {
        bool fired = neurons[i].update(dt);

        if (fired) {
            for (const auto& s : synapses) {
                if (s.from == i) {
                    spikeQueue.push_back({
                        currentTime + s.delaySteps,
                        s.to,
                        s.weight
                        });
                }
            }
        }
    }
}
