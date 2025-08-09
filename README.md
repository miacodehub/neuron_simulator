# neuron_simulator
An intercative C++ based neuron simulator with SFML GUI to showcase neuron activity in the Leaky Integrate and Fire model with real-time visualization of spiking neuron behavior.
This project is a real-time neuron simulation built using C++ and SFML.
The first iteration focuses on modeling simple leaky integrate-and-fire neurons with interactive controls to adjust network size and apply stimuli while observing voltage changes over time.

The GUI features:
Voltage-over-time graph with axis calibration
Dynamic neuron count control (+/- buttons)
Stimulus control panel (enabled when neurons exist)
Play/Pause simulation control
Real-time spike propagation through connected neurons
Oscilloscope-style visualization with labeled axes and grid lines

Tech Stack:
C++17
SFML 2.6+ for graphics/UI
Standard Template Library (STL) for data structures

How It Works:
Each neuron is modeled as a leaky integrate-and-fire unit.
Synapses connect neurons, transmitting spikes after a configurable delay.
A spike queue ensures accurate timing of spike delivery.
The GUI updates every frame to show neuron voltages and spike events.

Building & Running:
Command line:
git clone https://github.com/yourusername/neuron-simulator.git
cd neuron-simulator
g++ gui_main.cpp neuron.cpp synapse.cpp spike_event.cpp -o neuron_sim -lsfml-graphics -lsfml-window -lsfml-system ./neuron_sim

Planned improvements:
Multiple neuron types (excitatory, inhibitory, adaptive)
Adjustable synaptic weights and delays
More detailed membrane potential modeling (Hodgkin–Huxley, Izhikevich)
