class Neuron {
public:
    Neuron(double threshold = -50.0, double resting = -70.0)
        : threshold(threshold), voltage(resting), resting(resting) {}

    bool update(double dt) {
        // Simple leaky integrate-and-fire update
        voltage += (resting - voltage) * 0.1 * dt; // leak
        if (voltage >= threshold) {
            voltage = resting; // reset
            return true; // fired
        }
        return false;
    }

    void applyInput(double amount) {
        voltage += amount;
    }

    double getVoltage() const { return voltage; }

private:
    double threshold;
    double voltage;
    double resting;
};
