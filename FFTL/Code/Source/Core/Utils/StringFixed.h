#pragma once

// Fixed array string template

#include "../defs.h"

#include "../Containers/Array.h"
#include <cstring>


namespace FFTL
{


template <uint N>
class StringFixed : public FixedArray<char, N>
{
public:
	StringFixed()
	{
		this->m_data[0] = 0;
	}
	StringFixed(const char* psz)
	{
#if defined(_MSC_VER)
		strcpy_s(this->data(), N, psz);
#else
		strncpy(this->data(), psz, N);
#endif
	}
	StringFixed& operator=(const char* psz)
	{
#if defined(_MSC_VER)
		strcpy_s(this->data(), N, psz);
#else
		strncpy(this->data(), psz, N);
#endif
		return *this;
	}

	FFTL_NODISCARD operator const char*() const { return this->data(); }
	FFTL_NODISCARD operator char*() { return this->data(); }
};

#if defined(FFTL_WCHAR)
template <uint N>
class WStringFixed : public FixedArray<wchar_t, N>
{
public:
	WStringFixed()
	{
		this->m_data[0] = 0;
	}
	WStringFixed(const wchar_t* psz)
	{
#if defined(_MSC_VER)
		wcscpy_s(this->data(), N, psz);
#else
		wcsncpy(this->data(), psz, N);
#endif
	}
	WStringFixed& operator=(const wchar_t* psz)
	{
#if defined(_MSC_VER)
		wcscpy_s(this->data(), N, psz);
#else
		wcsncpy(this->data(), psz, N);
#endif
		return *this;
	}

	WStringFixed(const char* psz)
	{
		const char* p = psz;
		uint i = 0;
		for (; *p && i < N-1; ++p, ++i)
			this->m_data[i] = *p;
		this->m_data[i] = 0;
	}
	WStringFixed& operator=(const char* psz)
	{
		const char* p = psz;
		uint i = 0;
		for (; *p && i < N-1; ++p, ++i)
			this->m_data[i] = *p;
		this->m_data[i] = 0;
		return *this;
	}

	FFTL_NODISCARD operator const wchar_t*() const { return this->data(); }
	FFTL_NODISCARD operator wchar_t*() { return this->data(); }
};

template <uint N>
using TStringFixed = WStringFixed<N>;
#else
template <uint N>
using TStringFixed = StringFixed<N>;
#endif

/// default values recommended by http://isthe.com/chongo/tech/comp/fnv/
FFTL_NODISCARD constexpr u32 StringHash(uint oneChar, u32 hash = 0x01000193)
{
	//	Use 64 bit integers to prevent Warning C4307 '+': integral constant overflow with constant evaluated string literals
	constexpr u64 PRIME = 0x01000193; //   16777619
	return static_cast<u32>((oneChar ^ hash) * PRIME);
}

/// hash a C-style string
FFTL_NODISCARD constexpr u32 StringHash(const char* text, u32 uHash)
{
	FFTL_ASSERT(text);

	if (*text == 0)
		return 0;

	uHash = StringHash(static_cast<uint>(*text++), uHash);

	while (*text != 0)
		uHash = StringHash(static_cast<uint>(*text++), uHash);
	return uHash;
}
FFTL_NODISCARD constexpr u32 StringHash(const char* text)
{
	constexpr u32 uHash = 0x01000193;
	return StringHash(text, uHash);
}

#if defined(FFTL_WCHAR)
FFTL_NODISCARD constexpr u32 StringHash(const wchar_t* text, u32 uHash)
{
	FFTL_ASSERT(text);

	if (*text == 0)
		return 0;

	uHash = StringHash(static_cast<uint>(*text++), uHash);

	while (*text != 0)
		uHash = StringHash(static_cast<uint>(*text++), uHash);
	return uHash;
}
FFTL_NODISCARD constexpr u32 StringHash(const wchar_t* text)
{
	constexpr u32 uHash = 0x01000193;
	return StringHash(text, uHash);
}
#endif


}



