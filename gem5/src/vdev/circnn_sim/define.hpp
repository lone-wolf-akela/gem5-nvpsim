#pragma once
#include "define.h"

template <int BitNum>
Num<BitNum>::Num(const ValueType& value)
{
	set(value);
}

template <int BitNum>
Num<BitNum>::Num(const ValueType& value, bool)
{
	setRegulation(value);
}

template <int BitNum>
void Num<BitNum>::set(const ValueType& value)
{	
	if (value >= 0)
	{
		assert(value < (1 << (BitNum - 1)));
	}
	else
	{
		assert(-value <= (1 << (BitNum - 1)));
	}	
	_value = value;
}

template <int BitNum>
void Num<BitNum>::setRegulation(const ValueType& value)
{
	_value = value;
	_regulation();
}

template <int BitNum>
void Num<BitNum>::setBit(int bit, const bool& v)
{
	assert(bit >= 0);
	assert(bit < BitNum);

	const ValueType mask = (1 << bit);
	if (v)
	{
		_value |= mask;
	}
	else
	{
		_value &= ~mask;
	}

	_regulation();
}

template <int BitNum>
bool Num<BitNum>::getBit(int bit) const
{
	assert(bit >= 0);
	assert(bit < BitNum);
	return bool(_value & (1 << bit));
}

template <int BitNum>
Num<BitNum>& Num<BitNum>::operator=(const ValueType& value)
{
	setRegulation(value);
	return *this;
}

template <int BitNum>
bool Num<BitNum>::operator==(const ValueType& value) const
{
	return _value == value;
}

template <int BitNum>
Num<BitNum> Num<BitNum>::operator+(const Num<BitNum>& num) const
{
	return Num<BitNum>(_value + num._value, true);
}

template <int BitNum>
Num<BitNum + 1> Num<BitNum>::extendAdd(const Num<BitNum>& num) const
{
	return Num<BitNum + 1>(_value + num._value);
}

template <int BitNum>
Num<BitNum> Num<BitNum>::signKeepAdd(const Num<BitNum>& num) const
{
	return extendAdd(num).template changeBit<BitNum>();
}

template <int BitNum>
Num<BitNum> Num<BitNum>::signKeepAdd(const Num<BitNum>& num, const int& rightShift) const
{
	return (extendAdd(num) >> rightShift).template changeBit<BitNum>();
}

template <int BitNum>
Num<BitNum> Num<BitNum>::operator-() const
{
	return Num<BitNum>(-_value);
}

template <int BitNum>
Num<BitNum> Num<BitNum>::operator-(const Num<BitNum>& num) const
{
	return Num<BitNum>(_value - num._value, true);
}

template <int BitNum>
Num<BitNum + 1> Num<BitNum>::extendSub(const Num<BitNum>& num) const
{
	return Num<BitNum + 1>(_value - num._value);
}

template <int BitNum>
Num<BitNum> Num<BitNum>::signKeepSub(const Num<BitNum>& num) const
{
	return extendSub(num).template changeBit<BitNum>();
}

template <int BitNum>
Num<BitNum> Num<BitNum>::signKeepSub(const Num<BitNum>& num, const int& rightShift) const
{
	return (extendSub(num) >> rightShift).template changeBit<BitNum>();
}

template <int BitNum>
Num<BitNum> Num<BitNum>::operator>>(const int& i) const
{
	return Num<BitNum>(_value >> i);
}

template <int BitNum>
Num<BitNum> Num<BitNum>::operator<<(const int& i) const
{
	//注意，这里的左移操作会保持符号不变;
	//同时左移可能导致溢出，所以需要进行regulation

	//if BitNum==12, newSignMask=2048=0b 1000 0000 0000
	constexpr ValueType newSignMask = 1 << (BitNum - 1);
	if (_value >= 0)
	{
		return Num<BitNum>((_value << i) & (~newSignMask), true);
	}
	else
	{
		return Num<BitNum>((_value << i) | newSignMask, true);
	}
}

template <int BitNum>
template <int NewBitNum>
Num<NewBitNum> Num<BitNum>::changeBit() const
{
	IF_CONSTEXPRESS(NewBitNum >= BitNum)
	{
		//填充符号位的操作不必显示进行，因为理论上 _value 必然是
		//_regulation过的，左侧多余位数本来就填充有符号位
		return Num<NewBitNum>(_value);
	}
	else
	{
		//if NewBitNum==12, newSignMask=2048=0b 1000 0000 0000
		constexpr ValueType newSignMask = 1 << (NewBitNum - 1);

		if (_value >= 0)
		{
			return Num<NewBitNum>(_value & (~newSignMask), true);
		}
		{
			return Num<NewBitNum>(_value | newSignMask, true);
		}
	}
}

template <int BitNum>
bool Num<BitNum>::operator[](const int& bit) const
{
	assert(bit >= 0);
	assert(bit < BitNum);

	//if bit==11, bitMask=2048=0b 1000 0000 0000
	const ValueType bitMask = 1 << bit;
	return bool(_value & bitMask);
}

template <int BitNum>
constexpr int Num<BitNum>::getBitNum()
{
	return BitNum;
}

template <int BitNum>
typename Num<BitNum>::ValueType Num<BitNum>::getRawValue() const
{
	return _value;
}

template <int BitNum>
void Num<BitNum>::_regulation()
{
	//if BitNum==12, signMask=2048=0b 1000 0000 0000
	constexpr ValueType signMask = 1 << (BitNum - 1);
	const ValueType sign = _value & signMask;

	if (sign)
	{
		//负数，需要在左侧全部补1
		//if BitNum==12, mask2=-4096=0b 1111 0000 0000 0000
		constexpr ValueType mask = ~((1 << BitNum) - 1);
		_value |= mask;
	}
	else
	{
		//正数，需要去除溢出部分
		//if BitNum==12, mask=4095=0b 1111 1111 1111
		constexpr ValueType mask = (1 << BitNum) - 1;
		_value &= mask;
	}
}

/**************************************************************/

template <int BitNum>
Inst<BitNum>::Inst(const ValueType& value)
{
	set(value);
}

template <int BitNum>
void Inst<BitNum>::set(const ValueType& value)
{
	assert(value < (uint64_t(1) << BitNum));
	_value = value;
}

template <int BitNum>
Inst<BitNum>& Inst<BitNum>::operator=(const ValueType& value)
{
	set(value);
	return *this;
}

template <int BitNum>
typename Inst<BitNum>::ValueType Inst<BitNum>::getRaw() const
{
	return _value;
}

template <int BitNum>
bool Inst<BitNum>::get(int bit) const
{
	assert(bit >= 0);
	assert(bit < BitNum);
	return bool(_value & (1 << bit));
}

template <int BitNum>
typename Inst<BitNum>::ValueType Inst<BitNum>::get(int bitEnd, int bitBeg) const
{
	assert(bitBeg >= 0);
	assert(bitEnd >= bitBeg);
	assert(bitEnd < BitNum);

	const ValueType mask1 = (1 << (bitEnd + 1)) - 1;
	const ValueType mask2 = (1 << bitBeg) - 1;
	const ValueType mask = mask1 - mask2;

	const ValueType t = _value & mask;
	return t >> bitBeg;
}

template <int BitNum>
void Inst<BitNum>::setBit(int bit, const bool& v)
{
	assert(bit >= 0);
	assert(bit < BitNum);

	const ValueType mask = (1 << bit);
	if (v)
	{
		_value |= mask;
	}
	else
	{
		_value &= ~mask;
	}
}

template <int BitNum>
void Inst<BitNum>::setBit(int bitEnd, int bitBeg, const ValueType& v)
{
	assert(bitBeg >= 0);
	assert(bitEnd >= bitBeg);
	assert(bitEnd < BitNum);

	assert(v < (ValueType(1) << (bitEnd - bitBeg + 1)));

	const ValueType mask1 = (1 << (bitEnd + 1)) - 1;
	const ValueType mask2 = (1 << bitBeg) - 1;
	const ValueType mask = mask1 - mask2;

	const ValueType v_bitUsed = (v << bitBeg);
	_value &= ~mask;
	_value |= v_bitUsed;
}

template <int BitNum>
bool Inst<BitNum>::operator==(const ValueType& value) const
{
	return _value == value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator==(const Inst<BitNum2>& inst) const
{
	return _value == inst._value;
}

template <int BitNum>
bool Inst<BitNum>::operator!=(const ValueType& value) const
{
	return _value != value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator!=(const Inst<BitNum2>& inst) const
{
	return _value != inst._value;
}

template <int BitNum>
bool Inst<BitNum>::operator<(const ValueType& value) const
{
	return _value < value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator<(const Inst<BitNum2>& inst) const
{
	return _value < inst._value;
}

template <int BitNum>
bool Inst<BitNum>::operator>(const ValueType& value) const
{
	return _value > value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator>(const Inst<BitNum2>& inst) const
{
	return _value > inst._value;
}

template <int BitNum>
bool Inst<BitNum>::operator<=(const ValueType& value) const
{
	return _value <= value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator<=(const Inst<BitNum2>& inst) const
{
	return _value <= inst._value;
}

template <int BitNum>
bool Inst<BitNum>::operator>=(const ValueType& value) const
{
	return _value >= value;
}

template <int BitNum>
template <int BitNum2>
bool Inst<BitNum>::operator>=(const Inst<BitNum2>& inst) const
{
	return _value >= inst._value;
}