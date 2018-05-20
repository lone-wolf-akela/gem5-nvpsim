#pragma once

inline size_t reverseBit(size_t num, size_t len)
{
	size_t output = 0;
	for (size_t i = 0; i<len; i++)
	{
		output <<= 1;
		output |= num & 1;
		num >>= 1;		
	}
	return output;
}

constexpr size_t log2(int N)
{
	return (N > 1) ? 1 + log2(N >> 1) : 0;
}

template<typename T>
T fft_rearrange(int Begin, int End, const T &array)
{
	assert(End > Begin);
	assert(Begin >= 0);

	const int N = End - Begin;
	T temp = array;	
	
	const size_t len = log2(N);
	for (int i = Begin; i < End; i++)
	{
		temp[i] = array[Begin + reverseBit((i - Begin), len)];
	}
	return temp;
}

