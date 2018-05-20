#pragma once
#include "define.h"
#include <bitset>
#include "FFT128.h"

class Mem_hub
{
public:
	void readInput(
		const std::string& path,
		size_t batchBegin,
		size_t batch
	);

	void readWeightInput(
		const std::string& fileReal,
		const std::string& fileImag,
		bool layer3
	);

	void readBiasInput(const std::string& file);

	void generate(const std::string& outFile) const;

	std::vector<std::bitset<128>> x_sram;		//input x sram
	std::vector<std::bitset<128>> FFTx_sram;	//mid-result FFTx sram
	std::vector<std::bitset<128>> FFTw_sram;	//input FFTw sram
	std::vector<Inst<32>> inst_sram;	//inst ram
private:

};


















