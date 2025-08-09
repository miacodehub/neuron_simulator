#ifndef NEURON_HPP
#define NEURON_HPP
#endif


#include<vector>

class Neuron {
public:
	//properties of neuron
	double V; // current voltage
	double V_rest; // voltage at rest
	double V_thresh; // voltage threshold of neuron post which "spike" occurs
	double V_reset;// 
	double tau_m;
	double R;
	std::vector<double> spike_times;

	Neuron();
	void update(double I, double dt, double time_now);
};