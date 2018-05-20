#pragma once
#include "define.h"
#include "data.h"
#include "BFU2.h"
#include "BFUMUL2.h"



class FFT128
{
public:
	IN std::array<Num<NUM_LEN>, FFT_LEN> in_re;
	IN std::array<Num<NUM_LEN>, FFT_LEN> in_im;
	IN Inst<FFT_LEVEL> shift_mode = 0;
	IN Inst<5> mac_shift = 0;
	IN Inst<4> out_switch = 0;
	IN Inst<FFT_LEVEL> bfu_en = 0;
	IN Inst<FFT_LEVEL> mac_en = 0;
	IN Inst<FFT_LEVEL> fft_clear = 0;
	IN std::array<bool, FFT_W_LEN - 1> w_re; //63-bit value!
	IN std::array<bool, FFT_W_LEN - 1> w_im; //63-bit value!
	IN Inst<3> fft_level = 0;

	IN std::array<bool, FFT_W_LEN - 1> w_re_sign; //63-bit value!
	IN std::array<bool, FFT_W_LEN - 1> w_im_sign; //63-bit value!


	OUT std::bitset<FFT_LEN> fft_out_re = 0;
	OUT std::bitset<FFT_LEN> fft_out_im = 0;

	OUT std::bitset<FFT_LEN * 2> fft_out_bit = 0;

	std::array<Num<NUM_LEN>, FFT_LEN> fft_out_re_full;
	std::array<Num<NUM_LEN>, FFT_LEN> fft_out_im_full;

	void run();
private:
	std::array<BFU2, FFT_LEN / 2> bfu2[FFT_LEVEL];
	std::array<BFUMUL2, FFT_LEN / 2> bfumul2[FFT_LEVEL - 1];
	//std::array<BFU2end, FFT_LEN / 2> bfu2end;

	std::array<Num<NUM_LEN>, FFT_LEN> in_re_s[FFT_LEVEL];	//bfumul中间结果
	std::array<Num<NUM_LEN>, FFT_LEN> in_im_s[FFT_LEVEL];	//bfumul中间结果

	std::array<Num<NUM_LEN>, FFT_LEN> out_re_s[FFT_LEVEL];	//bfu中间结果
	std::array<Num<NUM_LEN>, FFT_LEN> out_im_s[FFT_LEVEL];	//bfu中间结果

	std::array<Num<NUM_LEN>, FFT_LEN> in_re_buffer;
	std::array<Num<NUM_LEN>, FFT_LEN> in_im_buffer;

	void level(int lv);
	//void level_last();
};


