#include "simulator.h"

Simulator::Simulator()
{
	fft128.shift_mode = 0;

	for (int k = 0; k < FFT_W_LEN - 1; k++)
	{
		fft128.w_re_sign[k] = wreal_sign(k);
		fft128.w_im_sign[k] = wimag_sign(k);
	}
}

int Simulator::FFT(
	std::string logfile, 
	bool FFTx_choose,
	bool tf_en, 
	int x_addr, 
	int fft_level, 
	int FFTx_addr
)
{
	int cycles = 0;
	
	mem_hub.inst_sram.emplace_back();
	mem_hub.inst_sram.back().setBit(31, 28, 1); //FFT
	mem_hub.inst_sram.back().setBit(27, FFTx_choose); //FFTx_choose?
	mem_hub.inst_sram.back().setBit(26, tf_en); //tf_en?

	mem_hub.inst_sram.back().setBit(25, 16, x_addr); //x_addr
	mem_hub.inst_sram.back().setBit(15, 13, fft_level); //fft_level
	mem_hub.inst_sram.back().setBit(9, 0, FFTx_addr); //FFTx_addr

	if(tf_en)
	{
		assert(logfile == "" && "tf_en instruction cannot be used with log file" );
		NoOP();
		NoOP();
		return 1;
	}

	for (size_t i = 0; i < fft_index.size(); i++)
	{
		fft_index[i] = i;
	}
	if (fft_level <= FFT_LEVEL)
	{
		const int block = 1 << fft_level;
		for (int i = 0; i < FFT_LEN; i += block)
		{
			fft_index = fft_rearrange(i, i + block,fft_index);
		}
	}
	else
	{
		throw NotImplemented();
	}


	//write input number to file 
	/*std::ofstream ofile(logfile);
	if(!ofile)
	{
		throw std::runtime_error("Openning Log File Failed");
	}
	for (size_t i = 0; i <= fft_controller.batch_num.getRaw(); i++)
	{
		ofile << ">> Batch " << i << " Input Data<<\n";
		ofile << "\t>> Real Part <<\n";
		for (int j = 0; j < FFT_LEN; j++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k < NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.x_sram
					[x_addr + i * NUM_LEN + k][j];
			}
			ofile << Num<NUM_LEN>(number,true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << "\t>> Imag Part <<\n";
		for (int j = 0; j < FFT_LEN; j++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k < NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.x_sram
					[x_addr + i * NUM_LEN + k][j + 64];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}
		ofile << std::endl;
	}*/

	fft_controller.inst_in = mem_hub.inst_sram.back();
	fft_controller.fft_level = fft_level;
	fft128.fft_level = fft_level;
	
	bool first = true;

	while (!fft_controller.FFT_ctrl_done || first)
	{
		cycles++;
		
		first = false;

		//run fft	
		fft128.run();

		//get output	
		//需要24周期
		//注意这个输出必须在fft run之后、control run之前
		if (fft_controller.FFTx_sram_en)
		{
			if (mem_hub.FFTx_sram.size() <= fft_controller.FFTx_addr.getRaw())
			{
				mem_hub.FFTx_sram.resize(fft_controller.FFTx_addr.getRaw() + 1);
			}
			for (size_t i = 0; i < 128; i++)
			{
				mem_hub.FFTx_sram[fft_controller.FFTx_addr.getRaw()][i] = 
					fft128.fft_out_bit[i];
			}
		}

		//run controller
		fft_controller.run();

		//set fft128
		if (fft_controller.x_sram_en)
		{
			for (int i = 0; i < FFT_LEN; i++)
			{
				//这里注意是大端序
				fft128.in_re[i].setBit(
					NUM_LEN - 1 - fft_controller.cycle_current,
					mem_hub.x_sram[fft_controller.x_addr.getRaw()][fft_index[i]]
				);

				fft128.in_im[i].setBit(
					NUM_LEN - 1 - fft_controller.cycle_current,
					mem_hub.x_sram[fft_controller.x_addr.getRaw()][fft_index[i] + 64]
				);
			}
		}

		fft128.out_switch = fft_controller.out_switch;
		fft128.bfu_en = fft_controller.bfu_en;
		fft128.mac_en = fft_controller.mac_en;
		fft128.fft_clear = fft_controller.fft_clear;

		for (int k = 0; k < FFT_W_LEN - 1; k++)
		{
			fft128.w_re[k] = wreal(k).get(fft_controller.twiddle_bitchoose.getRaw());
			fft128.w_im[k] = wimag(k).get(fft_controller.twiddle_bitchoose.getRaw());
		}
	}

	//write output number to file
	/*ofile << std::endl;
	for (size_t i = 0; i <= fft_controller.batch_num.getRaw(); i++)
	{
		ofile << ">> Batch " << i << " Output Data<<\n";
		ofile << "\t>> Real Part <<\n";
		for (int j = 0; j < FFT_LEN; j++)
		{
			const int index = j / 16 * 32 + j % 16;

			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k < NUM_LEN; k++)
			{
				const size_t addr = FFTx_addr + i * NUM_LEN + k;
				number = (number << 1) + mem_hub.FFTx_sram[addr][index];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << "\t>> Imag Part <<\n";
		for (int j = 0; j < FFT_LEN; j++)
		{
			const int index = j / 16 * 32 + j % 16 + 16;

			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k < NUM_LEN; k++)
			{
				const size_t addr = FFTx_addr + i * NUM_LEN + k;
				number = (number << 1) + mem_hub.FFTx_sram[addr][index];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}
		ofile << std::endl;
	}
	ofile.close();*/


	NoOP();
	NoOP();
	
	return cycles;
}

int Simulator::PE(
	std::string logfile,
	int FFTx_addr_inst,					//inst结尾的参数是写进data_rom里的
	int FFTw_addr,
	int FFTx_addr_sim,					//sim结尾的参数是模拟器用的
	int FFTx_part,						//这个参数反应进行运算的数是128位中的哪个part(0~3)
	int FFTx_addr_numberStep			//这个参数反应实际硬件上的FFTx中会被同时取出的四组数
										//在模拟器的FFTx地址空间中的距离
)
{
	int cycles = 0;
	
	assert(FFTx_part >= 0);
	assert(FFTx_part < 4);

	//硬件上是每个指令会算四组数各自的1/4part，要四次指令才能完整的进行四组数的运算
	//指令生成
	mem_hub.inst_sram.emplace_back();
	mem_hub.inst_sram.back().setBit(31, 28, 2);
	//FFTx_addr_next = inst_in.get(25, 16);
	mem_hub.inst_sram.back().setBit(25, 16, FFTx_addr_inst);
	//FFTw_addr_next = inst_in.get(13, 0);
	mem_hub.inst_sram.back().setBit(13, 0, FFTw_addr);


	//设定controller
	pe_controller.inst_in = mem_hub.inst_sram.back();
	pe_controller.FFTw_addr_init = FFTw_addr;
	pe_controller.FFTx_addr_init = FFTx_addr_sim;


	bool first = true;
	while (!pe_controller.PE_ctrl_done || first)
	{
		cycles++;
		
		first = false;
		pe_controller.run();

		pe_array.mac_en = pe_controller.mac_en;
		pe_array.accu_en = pe_controller.accu_en;
		pe_array.mac_clear = pe_controller.mac_clear;
		pe_array.accu_clear = pe_controller.accu_clear;
		pe_array.data_in = pe_controller.PE_data_in;

		if (pe_controller.FFTx_sram_en)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					//符号位
					if (pe_controller.cycle_current == 0)
					{
						pe_array.activ_re[i][j] = 0;
						pe_array.activ_im[i][j] = 0;
						for(int bit = pe_controller.bit_num.getRaw()+1;
							bit<NUM_LEN;
							bit++)
						{
							
							pe_array.activ_re[i][j].setBit(
								bit,
								mem_hub.FFTx_sram[
									pe_controller.FFTx_addr.getRaw() + FFTx_addr_numberStep * i]
								[FFTx_part * 32 + j]
									);						
							pe_array.activ_im[i][j].setBit(
								bit,
								mem_hub.FFTx_sram[
									pe_controller.FFTx_addr.getRaw() + FFTx_addr_numberStep * i]
								[FFTx_part * 32 + j + 16]
									);
						}
					}
					pe_array.activ_re[i][j].setBit(
						pe_controller.bit_num.getRaw() - pe_controller.cycle_current,
						mem_hub.FFTx_sram[
							pe_controller.FFTx_addr.getRaw() + FFTx_addr_numberStep * i]
						[FFTx_part * 32 + j]
							);

					pe_array.activ_im[i][j].setBit(
						pe_controller.bit_num.getRaw() - pe_controller.cycle_current,
						mem_hub.FFTx_sram[
							pe_controller.FFTx_addr.getRaw() + FFTx_addr_numberStep * i]
						[FFTx_part * 32 + j + 16]
							);

					if(false)
					{
						const int dropbits[] = { 8,10,11,13,14,15,16,17,18,
						20,21,22,23,24,25,26,27,28,30,31,32}; // 0 to 32
						for (const auto dropbit : dropbits)
						{
							if (FFTx_part * 16 + j == dropbit || FFTx_part * 16 + j == 64 - dropbit)
							{
								pe_array.activ_re[i][j] = 0;
								pe_array.activ_im[i][j] = 0;
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				pe_array.weightbit_re[i][j] = mem_hub.FFTw_sram
					[pe_controller.FFTw_addr.getRaw()][i * 32 + j];

				pe_array.weightbit_im[i][j] = mem_hub.FFTw_sram
					[pe_controller.FFTw_addr.getRaw()][i * 32 + j + 16];
			}
		}

		pe_array.run();
	}


	/*std::ofstream ofile(logfile);
	if (!ofile)
	{
		throw std::runtime_error("Openning Log File Failed");
	}
	for (size_t i = 0; i <= pe_controller.mac_num.getRaw(); i++)
	{
		ofile << ">> Mac " << i << " Input X <<\n";
		ofile << "\t>> Real Part\n";
		for (int j = 0; j < 64; j++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k<NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.FFTx_sram
					[FFTx_addr_sim + pe_controller.FFTx_addr_step * i +
					FFTx_addr_numberStep * (j / 16) + k][FFTx_part * 32 + j % 16];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << "\t>> Imag Part\n";
		for (int j = 0; j < 64; j++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k < NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.FFTx_sram
					[FFTx_addr_sim + pe_controller.FFTx_addr_step * i +
					FFTx_addr_numberStep * (j / 16) + k][FFTx_part * 32 + j % 16 + 16];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (j % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << std::endl;
	}



	for (size_t i = 0; i <= pe_controller.mac_num.getRaw(); i++)
	{
		ofile << ">> Mac " << i << " Input W <<\n";
		ofile << "\t>> Real Part\n";
		for (int w = 0; w < 64; w++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k<NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.FFTw_sram
					[FFTw_addr + i*NUM_LEN + k][(w / 16) * 32 + w % 16];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (w % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << "\t>> Imag Part\n";
		for (int w = 0; w < 64; w++)
		{
			Num<NUM_LEN>::ValueType number = 0;
			for (int k = 0; k<NUM_LEN; k++)
			{
				number = (number << 1) + mem_hub.FFTw_sram
					[FFTw_addr + i * NUM_LEN + k][(w / 16) * 32 + w % 16 + 16];
			}
			ofile << Num<NUM_LEN>(number, true).getRawValue() << '\t';
			if (w % 16 == 15)
			{
				ofile << std::endl;
			}
		}

		ofile << std::endl;
	}

	for(int x=0;x<4;x++)
	{
		for(int w=0;w<4;w++)
		{
			ofile << ">>Output: X=" << x << " W=" << w << " <<\n";
			ofile << "\tReal Part:\n";
			for (int i = 0; i < 16; i++)
			{
				ofile << pe_array.accu_result_out_re_full[x][w][i].getRawValue() << '\t';
			}
			ofile << std::endl;
			ofile << "\tImag Part:\n";
			for (int i = 0; i < 16; i++)
			{
				ofile << pe_array.accu_result_out_im_full[x][w][i].getRawValue() << '\t';
			}
			ofile << std::endl;
			ofile << std::endl;
		}
	}
	

	ofile.close();*/

	NoOP();
	NoOP();
	
	return cycles;
}

int Simulator::Parameter(
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
) 
{
	Inst<32> inst = 0;
	if(fft) 
	{
		assert(batch_num > 0);
		assert(bit_num > 0);
		assert(shift_mode >= 0);
		assert(mac_shift10 >= 0);
		assert(mac_shift42 >= 0);
		assert(out_switch >= 0);

		//fft parameters
		inst.setBit(31, 27, 0b01011);
		
		fft_controller.batch_num = batch_num - 1;
		inst.setBit(25, 22, batch_num -1);

		//FFT_bit_num_temp <= inst_in[21:18];
		fft_controller.bit_num = bit_num - 1;
		inst.setBit(21, 18, bit_num - 1);

		fft128.shift_mode = shift_mode; //don't know what this does
		inst.setBit(17, 11, shift_mode);

		//right shift 5 bit since w is 6 bit
		fft128.mac_shift.setBit(1, 0, mac_shift10);//shift right 1		
		fft128.mac_shift.setBit(4, 2, mac_shift42); //shift right 4
		inst.setBit(5, 4, mac_shift10);
		inst.setBit(8, 6, mac_shift42);

		//out_switch_temp <= inst_in[3:0];
		if (out_switch != 12)
		{
			throw NotImplemented();
		}
		inst.setBit(3, 0, out_switch - 1);
	}
	else
	{
		assert(bit_num >= 0);
		assert(mac_num >= 0);
		assert(FFTx_addr_step_inst >= 0);
		assert(FFTx_addr_step_sim >= 0);

		//PE parameters
		inst.setBit(31, 27, 0b01010);

		//data_pool_step_temp <= inst_in[19:13];
		inst.setBit(19, 13, 0); //pool相关，暂时置零

		//PE_sram_choose_temp <= inst_in[12];
		inst.setBit(12, false);

		//PE_mac_num_temp <= inst_in[11:8];
		inst.setBit(11, 8, mac_num - 1);
		pe_controller.mac_num = mac_num - 1;

		//PE_bit_num_temp <= inst_in[7:4];
		inst.setBit(7, 4, bit_num - 1);
		pe_controller.bit_num = bit_num - 1;

		//PE_FFTx_addr_step_temp <= inst_in[3:0];
		inst.setBit(3, 0, FFTx_addr_step_inst);
		pe_controller.FFTx_addr_step = FFTx_addr_step_sim;
	}
	mem_hub.inst_sram.push_back(inst);
	
	return 0;
}

int Simulator::Output_PE(int x_addr, int part, int rightshift)
{
	int cycles = 16 * NUM_LEN;
	
	Inst<32> inst = 0;
	//Output_ctrl  0011
	inst.setBit(31, 27, 0b00110);
	//{FFTx_addr_h_0, FFTx_addr_l_0} <= inst_in[25:16];// next addr
	//{FFTx_addr_h,FFTx_addr_l}<=inst_in[25:16];
	inst.setBit(25, 16, 0);
	//FFTx_addr_step <= inst_in[15:11];
	inst.setBit(15, 11, 0);
	//output_sram_choose<=inst_in[10];
	inst.setBit(10, false);
	//bit_choose_0<=inst_in[8:4];
	inst.setBit(8, 4, 0);
	//bit_num<=inst_in[3:0];
	inst.setBit(3, 0, NUM_LEN - 1);

	mem_hub.inst_sram.push_back(inst);

	for (size_t w = 0; w < 4; w++)
	{
		for (size_t input = 0; input < 4; input++)
		{
			const size_t x_addr_current = x_addr + (w * 4 + input)*NUM_LEN;
			if (mem_hub.x_sram.size() < x_addr_current + NUM_LEN)
			{
				mem_hub.x_sram.resize(x_addr_current + NUM_LEN);
			}

			for (int index = 0; index < 16; index++)
			{
				//到底右移多少位？
				const Num<NUM_LEN> re(
					pe_array.accu_result_out_re_full[input][w][index].getRawValue()
					>> (WNUM_FRAC_BITS + rightshift),
					true);

				//注意取共轭复数
				const Num<NUM_LEN> im(
					-(pe_array.accu_result_out_im_full[input][w][index].getRawValue()
						>> (WNUM_FRAC_BITS + rightshift)),
					true);

				for (int bit = 0; bit < NUM_LEN; bit++)
				{
					mem_hub.x_sram[x_addr_current + bit][index + part * 16] =
						re.getBit(NUM_LEN - 1 - bit);

					mem_hub.x_sram[x_addr_current + bit][index + part * 16 + 64] =
						im.getBit(NUM_LEN - 1 - bit);
				}
			}
		}
	}

	NoOP();
	NoOP();
	
	return cycles;
}

int Simulator::Output_IFFT(int x_addr, int FFTx_addr_sim, int batch_num, int rightshift)
{
	int cycles = 0;
	
	Inst<32> inst = 0;
	//TODO:set inst
	mem_hub.inst_sram.push_back(inst);

	for (int i = 0; i < batch_num; i++)
	{
		if(mem_hub.x_sram.size() <= x_addr + i * NUM_LEN)
		{
			mem_hub.x_sram.resize(x_addr + i + 1);
		}
		std::array<Num<NUM_LEN>, 128> temp;

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 16; k++)
			{
				for (int bit = 0; bit < NUM_LEN; bit++)
				{
					//real
					temp[j * 16 + k].setBit(NUM_LEN - 1 - bit,
						mem_hub.FFTx_sram[FFTx_addr_sim + i * NUM_LEN + bit]
						[j * 32 + k]
					);
					//imag
					temp[j * 16 + k + 64].setBit(NUM_LEN - 1 - bit,
						mem_hub.FFTx_sram[FFTx_addr_sim + i * NUM_LEN + bit]
						[j * 32 + k + 16]
					);
				}
				temp[j * 16 + k] = temp[j * 16 + k] >> rightshift;
				//共轭！
				temp[j * 16 + k + 64] = (-temp[j * 16 + k + 64]) >> rightshift;
			}
		}

		for(int bit=0;bit<NUM_LEN;bit++)
		{
			cycles++;
			
			for(int j=0;j<128;j++)
			{
				mem_hub.x_sram[x_addr + i * NUM_LEN + bit][j] =
					temp[j].getBit(NUM_LEN - 1 - bit);
			}
		}
	}



	NoOP();
	NoOP();
	
	return cycles;
}

int Simulator::Bias_Relu(int x_addr, int FFTw_addr, int bias_size, int batch_num)
{
	int cycles = bias_size/64 * batch_num * NUM_LEN;
	
	Inst<32> inst = 0;
	//TODO:set inst
	mem_hub.inst_sram.push_back(inst);

	for(int i=0;i<bias_size/64;i++)
	{
		for(int j=0;j<64;j++)
		{
			Num<NUM_LEN> bias;
			for(int bit=0;bit<NUM_LEN;bit++)
			{
				bias.setBit(NUM_LEN - 1 - bit,
					mem_hub.FFTw_sram[FFTw_addr + i / 2 * NUM_LEN + bit][j + i % 2 * 64]);
			}
			for(int batch=0;batch<batch_num;batch++)
			{
				const int x_addr_current = x_addr + (i*batch_num + batch)*NUM_LEN;
				Num<NUM_LEN> tempR,tempI;

				for (int bit = 0; bit<NUM_LEN; bit++)
				{
					tempR.setBit(NUM_LEN - 1 - bit,
						mem_hub.x_sram[x_addr_current + bit][j]);
					tempI.setBit(NUM_LEN - 1 - bit,
						mem_hub.x_sram[x_addr_current + bit][j+64]);
				}
				tempR = tempR + bias;
				tempI = tempI + bias;
				if (tempR.getRawValue() < 0) tempR = 0;
				if (tempI.getRawValue() < 0) tempI = 0;

				for (int bit = 0; bit<NUM_LEN; bit++)
				{
					mem_hub.x_sram[x_addr_current + bit][j] =
						tempR.getBit(NUM_LEN - 1 - bit);
					mem_hub.x_sram[x_addr_current + bit][j + 64] =
						tempI.getBit(NUM_LEN - 1 - bit);
				}
			}
		}
	}

	NoOP();
	NoOP();
	
	return cycles;
}

void Simulator::NoOP()
{
	mem_hub.inst_sram.emplace_back(0);
}

int Simulator::Mem_in(int batchBegin, int batchNum, int layer)
{
	if (batchNum > 0)
	{
		mem_hub.x_sram.clear();
		mem_hub.readInput("data/mnist/input/", batchBegin, batchNum);
	}
	mem_hub.FFTw_sram.clear();
	mem_hub.readWeightInput(
		"data/mnist/64weight/real_w" + std::to_string(layer) + ".txt",
		"data/mnist/64weight/img_w" + std::to_string(layer) + ".txt",
		layer == 2
	);
	mem_hub.readBiasInput("data/mnist/64bias/b" + std::to_string(layer) + ".txt");
		
	return 0;
}

void Simulator::GenerateRom(const std::string& file)
{
	assert(mem_hub.inst_sram.size() <= 1024);
	mem_hub.inst_sram.resize(1024);

	mem_hub.generate(file);
}

void Simulator::Debug_WriteLayer(const std::string& file)
{
	std::ofstream fout(file);
	if (!fout)
	{
		throw std::runtime_error("Error opening file");
	}

	for(int pic=0;pic<8;pic++)
	{
		fout << "\n>> Pic " << pic << " <<\n";
		for(int block = 0;block<8;block++)
		{
			for (int num = 0; num < 64; num++)
			{
				const int x_addr = 12 + (block*4 + pic / 2)* NUM_LEN;
				Num<NUM_LEN> temp;
				for(int bit =0;bit<NUM_LEN;bit++)
				{
					temp.setBit(NUM_LEN - 1 - bit, 
						mem_hub.x_sram[x_addr + bit][num + (pic % 2) * 64]);
				}
				fout << double(temp.getRawValue()) / 64 << '\t';

				if(num%16==15)
				{
					fout << std::endl;
				}
			}
		}
	}

	fout.close();
}

std::array<std::array<double, 64>, 8> Simulator::GetLayer3()
{
	std::array<std::array<double, 64>, 8> result;

	for (int pic = 0; pic < 8; pic++)
	{
		for (int num = 0; num < 64; num++)
		{
			const int x_addr = 12 + pic / 2 * NUM_LEN;
			Num<NUM_LEN> temp;
			for (int bit = 0; bit < NUM_LEN; bit++)
			{
				temp.setBit(NUM_LEN - 1 - bit,
					mem_hub.x_sram[x_addr + bit][num + (pic % 2) * 64]);
			}
			result[pic][num] = double(temp.getRawValue()) / 64;
		}
	}

	return result;
}
