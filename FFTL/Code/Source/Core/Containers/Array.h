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
#include <utility> // std::move
#include <memory>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4324) // structure was padded due to alignment specifier
#endif

// Arrays
namespace FFTL
{


template <typename T, size_t T_N>
class FFTL_NODISCARD FixedArray
{
public:

	class riter
	{
	public:
		FFTL_FORCEINLINE constexpr riter(T* p) : m_{ p } {}
		FFTL_FORCEINLINE constexpr riter& operator++() { --m_; return *this; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr T& operator->() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr T& operator*() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const riter& lhs, const riter& rhs) { return lhs.m_ == rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const riter& lhs, const riter& rhs) { return lhs.m_ != rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const riter& lhs, const T* rhs) { return lhs.m_ == rhs; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const riter& lhs, const T* rhs) { return lhs.m_ != rhs; }
	private:
		T* m_;
	};
	class criter
	{
	public:
		FFTL_FORCEINLINE constexpr criter(const T* p) : m_{ p } {}
		FFTL_FORCEINLINE constexpr criter& operator++() { --m_; return *this; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr const T& operator->() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr const T& operator*() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const criter& lhs, const criter& rhs) { return lhs.m_ == rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const criter& lhs, const criter& rhs) { return lhs.m_ != rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const criter& lhs, const T* rhs) { return lhs.m_ == rhs; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const criter& lhs, const T* rhs) { return lhs.m_ != rhs; }
	private:
		const T* m_;
	};


	T m_data[T_N];

	using _array_type = decltype(m_data);
	using difference_type = typename std::pointer_traits<T*>::difference_type;

	template <typename ...E> constexpr FixedArray(E&&...e) : m_data{ std::forward<E>(e)... } {}
//	constexpr FixedArray(const T(&r)[T_N]);
	FFTL_NODISCARD constexpr T& operator[](difference_type n) { FFTL_ASSERT(n < static_cast<difference_type>(T_N)); return data()[n]; }
	FFTL_NODISCARD constexpr const T& operator[](difference_type n) const { FFTL_ASSERT(n < static_cast<difference_type>(T_N)); return data()[n]; }
	FFTL_NODISCARD constexpr T* operator+(difference_type n) { FFTL_ASSERT(n < static_cast<difference_type>(T_N)); return data() + n; }
	FFTL_NODISCARD constexpr const T* operator+(difference_type n) const { FFTL_ASSERT(n < static_cast<difference_type>(T_N)); return data() + n; }

	FFTL_NODISCARD constexpr static size_t size() { return T_N; }

	FFTL_NODISCARD constexpr const T* begin() const { return data(); }
	FFTL_NODISCARD constexpr const T* end() const { return data() + size(); }
	FFTL_NODISCARD constexpr T* begin() { return data(); }
	FFTL_NODISCARD constexpr T* end() { return data() + size(); }
	FFTL_NODISCARD constexpr criter rbegin() const { return end() - 1; }
	FFTL_NODISCARD constexpr criter rend() const { return begin() - 1; }
	FFTL_NODISCARD constexpr riter rbegin() { return end() - 1; }
	FFTL_NODISCARD constexpr riter rend() { return begin() - 1; }

	FFTL_NODISCARD constexpr const _array_type& data() const { return m_data; }
	FFTL_NODISCARD constexpr _array_type& data() { return m_data; }

	FFTL_NODISCARD constexpr operator const T*() { return data(); }
	FFTL_NODISCARD constexpr operator T*() { return data(); }
};

template <typename T, size_t T_MAX_SIZE>
class Array
{
public:

	class riter
	{
	public:
		FFTL_FORCEINLINE constexpr riter(T* p) : m_{ p } {}
		FFTL_FORCEINLINE constexpr riter& operator++() { --m_; return *this; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr T& operator->() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr T& operator*() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const riter& lhs, const riter& rhs) { return lhs.m_ == rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const riter& lhs, const riter& rhs) { return lhs.m_ != rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const riter& lhs, const T* rhs) { return lhs.m_ == rhs; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const riter& lhs, const T* rhs) { return lhs.m_ != rhs; }
	private:
		T* m_;
	};
	class criter
	{
	public:
		FFTL_FORCEINLINE constexpr criter(const T* p) : m_{ p } {}
		FFTL_FORCEINLINE constexpr criter& operator++() { --m_; return *this; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr const T& operator->() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE constexpr const T& operator*() const { return *m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const criter& lhs, const criter& rhs) { return lhs.m_ == rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const criter& lhs, const criter& rhs) { return lhs.m_ != rhs.m_; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const criter& lhs, const T* rhs) { return lhs.m_ == rhs; }
		FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const criter& lhs, const T* rhs) { return lhs.m_ != rhs; }
	private:
		const T* m_;
	};

	using difference_type = typename std::pointer_traits<T*>::difference_type;

	Array() = default;
	Array(size_t nSize) : m_data(), m_size(nSize) { for (size_t i = 0; i < nSize; ++i) { T* pData = reinterpret_cast<T*>(m_data + i); ::new(pData) T(); } }
	FFTL_NODISCARD T& operator[](difference_type n) { FFTL_ASSERT(n < static_cast<difference_type>(m_size)); return *reinterpret_cast<T*>(m_data[n].m_Bytes); }
	FFTL_NODISCARD const T& operator[](difference_type n) const { FFTL_ASSERT(n < static_cast<difference_type>(m_size)); return *reinterpret_cast<const T*>(m_data[n].m_Bytes); }
	FFTL_NODISCARD T* operator+(difference_type n) { FFTL_ASSERT(n < static_cast<difference_type>(m_size)); return *reinterpret_cast<T*>(m_data + n); }
	FFTL_NODISCARD const T* operator+(difference_type n) const { FFTL_ASSERT(n < static_cast<difference_type>(m_size)); return *reinterpret_cast<const T*>(m_data + n); }

	FFTL_NODISCARD size_t size() const { return m_size; }

	FFTL_NODISCARD const T* begin() const { return reinterpret_cast<const T*>(m_data); }
	FFTL_NODISCARD const T* end() const { return reinterpret_cast<const T*>(m_data + size()); }
	FFTL_NODISCARD T* begin() { return reinterpret_cast<T*>(m_data); }
	FFTL_NODISCARD T* end() { return reinterpret_cast<T*>(m_data + size()); }
	FFTL_NODISCARD constexpr criter rbegin() const { return end() - 1; }
	FFTL_NODISCARD constexpr criter rend() const { return begin() - 1; }
	FFTL_NODISCARD constexpr riter rbegin() { return end() - 1; }
	FFTL_NODISCARD constexpr riter rend() { return begin() - 1; }

	template<class... _Valty>
	decltype(auto) emplace_back(_Valty&&... _Val)
	{
		FFTL_ASSERT(m_size < T_MAX_SIZE);
		T* pData = reinterpret_cast<T*>(m_data + m_size++);
		::new(pData) T(std::forward<_Valty>(_Val)...);
		return *pData;
	}
	void push_back(const T& r)
	{
		FFTL_ASSERT(m_size < T_MAX_SIZE);
		T* pData = reinterpret_cast<T*>(m_data + m_size++);
		::new(pData) T(r);
	}
	void erase_back()
	{
		FFTL_ASSERT(m_size > 0);
		T* pData = reinterpret_cast<T*>(m_data + m_size--);
		pData->~T();
	}
	T* erase(T* pItem)
	{
		FFTL_ASSERT(m_size > 0);
		FFTL_ASSERT(pItem >= m_data && pItem < m_data + m_size);
		m_size--;
		for (T* pNew = pItem; pNew < m_data + m_size; ++pNew)
		{
			pNew->~T();
			::new(pNew) T(std::move(pNew[1]));
		}
		return pItem;
	}

	struct alignas(T) tAllocStub { byte m_Bytes[sizeof(T)]; };
	tAllocStub m_data[T_MAX_SIZE];
private:
	size_t m_size = 0;
};

template <typename T, u32 T_N>
class alignas(8) FixedArray_Aligned8 : public FixedArray<T, T_N>
{
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

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

