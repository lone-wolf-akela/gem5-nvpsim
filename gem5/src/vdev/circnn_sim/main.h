#include "simulator.h"

class CirCNN
{
public:
	std::array<std::array<double, 64>, 8> Result;
	bool finished = false;
	enum {HighEnergy, LowEnergy} state;

	std::pair<double, double> Run();
	void Rewind();
private:
	int counter = 0;
	Simulator sim;
	
	double cycleLen = 5000; //200MHz.
	double energy_FFT = 394.72;	//pJ
	double energy_PE = 568.85;
	double energy_BiasReLU = 1.485;
	double energy_other = 166.085;
};
