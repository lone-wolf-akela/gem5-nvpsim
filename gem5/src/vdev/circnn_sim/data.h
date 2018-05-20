#pragma once
#include "define.h"

constexpr int FFT_LEVEL = 6;
constexpr int FFT_LEN = 1 << FFT_LEVEL;
constexpr int FFT_W_LEN = 128;


inline float getW(size_t index, bool real);

//无符号1.5比特量化w
constexpr int W_BITNUM = 6;

inline Inst<W_BITNUM> wreal(int index);

inline Inst<W_BITNUM> wimag(int index);

inline bool wreal_sign(int index);

inline bool wimag_sign(int index);

#include "data.hpp"