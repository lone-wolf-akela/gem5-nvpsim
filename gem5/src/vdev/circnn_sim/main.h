#include "simulator.h"

class CirCNN
{
public:
	std::array<std::array<double, 64>, 8> Result;
	bool finished = false;
	enum State {HighEnergy = 1, LowEnergy = 0} state;

	std::pair<double, double> Run();
	void Rewind();
private:
	int counter = 0;
	Simulator sim;
	
	double cycleLen = 5000; //200MHz.
	const double cycleLenHigh = 5000;
	const double cycleLenLow = 1000000; //1MHz
	
	const double energy_FFT = 394.72;	//pJ
	const double energy_PE = 568.85;
	const double energy_BiasReLU = 1.485;
	const double energy_other = 166.085;
};
