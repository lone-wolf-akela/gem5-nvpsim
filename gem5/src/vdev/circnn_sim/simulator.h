#pragma once

#include "define.h"
#include "FFT_controller.h"
#include "FFT128.h"
#include "PE_controller.h"
#include "PE_array.h"
#include "mem_hub.h"
#include "helper.h"

class Simulator
{
public:
	Simulator();

	void FFT(
		std::string logfile,
		bool FFTx_choose, 
		bool tf_en, 
		int x_addr,
		int fft_level, 
		int FFTx_addr
	);

	//每个指令会算四组数各自的1/4part，要四次指令才能完整的进行四组数的运算
	void PE(
		std::string logfile,
		int FFTx_addr_inst,					//inst结尾的参数是写进data_rom里的
		int FFTw_addr,
		int FFTx_addr_sim,					//sim结尾的参数是模拟器用的
		int FFTx_part,						//这个参数反应进行运算的数是128位中的哪个part(0~3)
		int FFTx_addr_numberStep = NUM_LEN	//这个参数反应实际硬件上的FFTx中会被同时取出的四组数
											//在模拟器的FFTx地址空间中的距离
	);
	void Parameter(
		bool fft,

		int batch_num,
		int bit_num,
		int shift_mode,
		int mac_shift42,
		int mac_shift10,
		int out_switch,

		int mac_num,
		int FFTx_addr_step_inst,
		int FFTx_addr_step_sim
	);
	void Output_PE(int x_addr, int part, int rightshift);
	void Output_IFFT(int x_addr, int FFTx_addr, int batch_num, int rightshift);
	void Bias_Relu(
		int x_addr, 
		int FFTw_addr_sim, 
		int bias_size, 
		int batch_num
	);
	void NoOP();
	void Mem_in(int batchBegin, int batchNum, int layer);
	void GenerateRom(const std::string& file);
	void Debug_WriteLayer(const std::string& file);
	std::array<std::array<double, 64>, 8> GetLayer3();
private:
	PE_controller pe_controller;
	PE_array pe_array;
	FFT128 fft128;
	FFT_controller fft_controller;
	Mem_hub mem_hub;

	std::array<size_t, FFT_LEN> fft_index;

	//decltype(mem_hub.inst_sram)::value_type::ValueType x_addr = 12;
	//decltype(mem_hub.inst_sram)::value_type::ValueType FFTx_addr = 0;
};

