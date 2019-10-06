/*

Original author:
Corey Shay
corey@signalflowtechnologies.com

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "defs.h"

// Arrays


namespace FFTL
{


template <typename T, size_t T_N>
class FixedArray
{
public:
	T data[T_N];

	FFTL_FORCEINLINE FixedArray() = default;
	FFTL_FORCEINLINE T& operator[](size_t n) { FFTL_ASSERT(n < T_N); return data[n]; }
	FFTL_FORCEINLINE const T& operator[](size_t n) const { FFTL_ASSERT(n < T_N); return data[n]; }
	FFTL_FORCEINLINE T* operator+(size_t n) { FFTL_ASSERT(n < T_N); return data + n; }
	FFTL_FORCEINLINE const T* operator+(size_t n) const { FFTL_ASSERT(n < T_N); return data + n; }

	FFTL_FORCEINLINE static constexpr size_t size() { return T_N; }
	FFTL_FORCEINLINE const T* begin() const { return data; }
	FFTL_FORCEINLINE const T* end() const { return data + size(); }
	FFTL_FORCEINLINE T* begin() { return data; }
	FFTL_FORCEINLINE T* end() { return data + size(); }
};

template <typename T, size_t T_MAX_SIZE>
class Array
{
public:
	FFTL_FORCEINLINE Array() : m_size(0) {}
	FFTL_FORCEINLINE T& operator[](size_t n) { FFTL_ASSERT(n < T_MAX_SIZE); return *reinterpret_cast<T*>(m_Data[n].m_Bytes); }
	FFTL_FORCEINLINE const T& operator[](size_t n) const { FFTL_ASSERT(n < T_MAX_SIZE); *reinterpret_cast<const T*>(m_Data[n].m_Bytes); }
	FFTL_FORCEINLINE T* operator+(size_t n) { FFTL_ASSERT(n < T_MAX_SIZE); return *reinterpret_cast<T*>(m_Data + n); }
	FFTL_FORCEINLINE const T* operator+(size_t n) const { FFTL_ASSERT(n < T_MAX_SIZE); return *reinterpret_cast<const T*>(m_Data + n); }

	FFTL_FORCEINLINE size_t size() const { return m_size; }
	FFTL_FORCEINLINE const T* begin() const { return reinterpret_cast<const T*>(m_Data); }
	FFTL_FORCEINLINE const T* end() const { return reinterpret_cast<const T*>(m_Data + size()); }
	FFTL_FORCEINLINE T* begin() { return reinterpret_cast<T*>(m_Data); }
	FFTL_FORCEINLINE T* end() { return reinterpret_cast<T*>(m_Data + size()); }

	FFTL_FORCEINLINE void push_back(const T& r)
	{
		T* pData = reinterpret_cast<T*>(m_Data + m_size++);
		::new(pData) T(r);
	}
	FFTL_FORCEINLINE void erase_back()
	{
		T* pData = reinterpret_cast<T*>(m_Data + m_size--);
		pData->~T();
	}

	struct alignas(T) tAllocStub { byte m_Bytes[sizeof(T)]; };
	tAllocStub m_Data[T_MAX_SIZE];
private:
	size_t m_size;
};

template <typename T, u32 T_N>
class alignas(16) FixedArray_Aligned16 : public FixedArray<T, T_N>
{
};

template <typename T, u32 T_N>
class alignas(32) FixedArray_Aligned32 : public FixedArray<T, T_N>
{
};

template <typename T, u32 T_N>
class alignas(64) FixedArray_Aligned64 : public FixedArray<T, T_N>
{
};



}



