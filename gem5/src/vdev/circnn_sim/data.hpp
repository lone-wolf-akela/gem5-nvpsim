#pragma once
#include "data.h"

inline float getW(size_t index, bool real)
{
	static std::array<float, FFT_W_LEN> wreal, wimag;
	static bool initialized = false;
	if (!initialized)
	{
		constexpr float PI = 3.14159265358979f;
		constexpr float arg = -2 * PI / (FFT_W_LEN * 2);
		const float treal = cos(arg);
		const float timag = sin(arg);
		wreal[0] = 1.0;
		wimag[0] = 0.0;
		for (int j = 1; j < FFT_W_LEN; j++)
		{
			wreal[j] = wreal[j - 1] * treal - wimag[j - 1] * timag;
			wimag[j] = wreal[j - 1] * timag + wimag[j - 1] * treal;
		}

		initialized = true;
	}
	return real ? wreal[index] : wimag[index];
}

inline Inst<W_BITNUM> wreal(int index)
{
	return Inst<W_BITNUM>(
		uint8_t(std::lround(std::abs(getW(index + 1, true)) * (1 << (W_BITNUM - 1))))
	);
}

inline Inst<W_BITNUM> wimag(int index)
{
	return Inst<W_BITNUM>(
		uint8_t(std::lround(std::abs(getW(index + 1, false)) * (1 << (W_BITNUM - 1))))
	);
}

inline bool wreal_sign(int index)
{
	return getW(index + 1, true) >= 0;
}

inline bool wimag_sign(int index)
{
	return getW(index + 1, false) >= 0;
}
