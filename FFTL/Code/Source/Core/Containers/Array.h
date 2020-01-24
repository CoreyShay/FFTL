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

#include <new>

// Arrays


namespace FFTL
{

#if defined(_MSC_VER)
#	define FFTL_FIXED_ARRAY_USES_STD_ARRAY 1
#endif

template <typename T, size_t T_N>
class FixedArray
{
public:
#if !defined(FFTL_FIXED_ARRAY_USES_STD_ARRAY)
	T m_data[T_N];
#else
	std::array<T, T_N> m_data;
#endif

	constexpr FFTL_FORCEINLINE FixedArray() : m_data() {}
	constexpr FFTL_FORCEINLINE FixedArray(const T(&r)[T_N]);
//	constexpr FFTL_FORCEINLINE FixedArray(std::initializer_list<T> l);
	constexpr FFTL_FORCEINLINE T& operator[](size_t n) { FFTL_ASSERT(n < T_N); return data()[n]; }
	constexpr FFTL_FORCEINLINE const T& operator[](size_t n) const { FFTL_ASSERT(n < T_N); return data()[n]; }
#if !defined(FFTL_64BIT)//	Strange compiler error about ambiguity in 32 bit builds
	constexpr FFTL_FORCEINLINE T& operator[](int n) { return data()[static_cast<size_t>(n)]; }
	constexpr FFTL_FORCEINLINE const T& operator[](int n) const { return data()[static_cast<size_t>(n)]; }
#endif
	constexpr FFTL_FORCEINLINE T* operator+(size_t n) { FFTL_ASSERT(n < T_N); return data() + n; }
	constexpr FFTL_FORCEINLINE const T* operator+(size_t n) const { FFTL_ASSERT(n < T_N); return data() + n; }

	constexpr FFTL_FORCEINLINE static size_t size() { return T_N; }
	constexpr FFTL_FORCEINLINE const T* begin() const { return data(); }
	constexpr FFTL_FORCEINLINE const T* end() const { return data() + size(); }
	constexpr FFTL_FORCEINLINE T* begin() { return data(); }
	constexpr FFTL_FORCEINLINE T* end() { return data() + size(); }

	constexpr FFTL_FORCEINLINE const T* data() const;
	constexpr FFTL_FORCEINLINE T* data();
};

template <typename T, size_t T_N>
constexpr FFTL_FORCEINLINE FixedArray<T, T_N>::FixedArray(const T(&r)[T_N])
{
	for (size_t i = 0; i < T_N; ++i)
		m_data[i] = r[i];
}
//template <typename T, size_t T_N>
//constexpr FFTL_FORCEINLINE FixedArray<T, T_N>::FixedArray(std::initializer_list<T> l)
//#if defined(FFTL_FIXED_ARRAY_USES_STD_ARRAY)
//	: m_data{ *reinterpret_cast<const std::array<T, T_N>*>(l.begin()) }
//#endif
////	: m_data{ l }
//{
//	FFTL_ASSERT(l.size() == T_N);
//#if !defined(FFTL_FIXED_ARRAY_USES_STD_ARRAY)
//	T* p = &m_data[0];
//	for (const T& t : l)
//		*p++ = t;
//#endif
//}

template <typename T, size_t T_N>
constexpr FFTL_FORCEINLINE const T* FixedArray<T, T_N>::data() const
{
#if !defined(FFTL_FIXED_ARRAY_USES_STD_ARRAY)
	return m_data;
#else
	return m_data.data();
#endif
}
template <typename T, size_t T_N>
constexpr FFTL_FORCEINLINE T* FixedArray<T, T_N>::data()
{
#if !defined(FFTL_FIXED_ARRAY_USES_STD_ARRAY)
	return m_data;
#else
	return m_data.data();
#endif
}

template <typename T, size_t T_MAX_SIZE>
class Array
{
public:
	FFTL_FORCEINLINE Array() : m_Data(), m_size(0) {}
	FFTL_FORCEINLINE Array(size_t nSize) : m_Data(), m_size(nSize) { for (size_t i = 0; i < nSize; ++i) { T* pData = reinterpret_cast<T*>(m_Data + i); ::new(pData) T(); } }
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
		FFTL_ASSERT(m_size > 0);
		T* pData = reinterpret_cast<T*>(m_Data + m_size--);
		pData->~T();
	}
	FFTL_FORCEINLINE T* erase(T* pItem)
	{
		FFTL_ASSERT(m_size > 0);
		FFTL_ASSERT(pItem >= m_Data && pItem < m_Data + m_size);
		m_size--;
		for (T* pNew = pItem; pNew < m_Data + m_size; ++pNew)
		{
			pNew->~T();
			::new(pNew) T(std::move(pNew[1]));
		}
		return pItem;
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



