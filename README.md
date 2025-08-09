Leaky Integrate and Fire Neuron Simulator 
An interactive C++ based neuron simulator with SFML GUI to showcase neuron activity in the Leaky Integrate and Fire model with real-time visualization of spiking neuron behavior.
This project is a real-time neuron simulation built using C++ and SFML.
The first iteration focuses on modeling simple leaky integrate-and-fire neurons with interactive controls to adjust network size and apply stimuli while observing voltage changes over time.

Leaky Integrate and Fire definition:

It’s a simple model of how neurons work. The neuron’s voltage slowly increases when it receives input, but also “leaks” away over time. When the voltage reaches a threshold, the neuron “fires” a spike, then resets.

Stimuli function:

Stimuli are short bursts of input sent to neurons. They raise the neuron’s voltage, making it more likely to fire a spike. In the simulation, you can apply different stimuli to see how the network responds in real time.

Applications of the simulator:

* Learning and visualizing basic neuroscience concepts
* Experimenting with simple brain network models
* Demonstrating spike timing and propagation
* Building a foundation for more advanced simulations used in AI, robotics, or medical research

GUI with no neurons:
<img width="990" height="682" alt="Screenshot 2025-08-09 181834" src="https://github.com/user-attachments/assets/83aefe5c-c229-49c2-bc1c-4301b6a83595" />

Single Neuron:
<img width="1007" height="692" alt="Screenshot 2025-08-09 181955" src="https://github.com/user-attachments/assets/4cb5c52f-67d1-4245-8ac4-87911e4a8400" />

Multiple Neurons:
<img width="1003" height="683" alt="Screenshot 2025-08-09 182034" src="https://github.com/user-attachments/assets/072bec0f-dc26-46f0-ab56-08a8e61b1eb4" />

Multiple neurons with stimuli given:
<img width="998" height="677" alt="Screenshot 2025-08-09 182127" src="https://github.com/user-attachments/assets/558dff74-5408-49b2-a0bf-91ab67359a5b" />





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
