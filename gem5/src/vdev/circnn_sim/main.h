#include "simulator.h"


class CirCNN
{
public:
	std::array<std::array<double, 64>, 8> Result;
	int batch = 0;
	int ins = 0;
	int layer = 0;
	bool finished = false;
	Simulator sim;
	std::pair<double,double> Run();
};