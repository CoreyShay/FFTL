#pragma once

// Fixed array string template

#include "../defs.h"

#include "Casts.h"
#include "../Math/MathCommon.h"
#include "../Containers/Array.h"
#include <cstring>
#include <limits>
#include <cctype> // tolower

#if defined(FFTL_SSE)
#	include <xmmintrin.h>
#endif
#if defined(FFTL_SSE2)
#	include <emmintrin.h>
#endif
#if defined(FFTL_SSE3)
#	include <pmmintrin.h>
#endif
#if defined(FFTL_SSE4)
#	include <smmintrin.h>
#endif
#if defined(FFTL_AVX)
#	include <immintrin.h>
#endif
#if defined(FFTL_ARM_NEON)
#	include <arm_neon.h>
#endif

#include "../Containers/Array.h"


namespace FFTL
{


//	Compares blocks of memory of a size known at compile time in the fastest way possible.
// Powers of 2 sizes <= 64 are branchless.
template<size_t T_BYTE_COUNT>
FFTL_NODISCARD bool CompareBytesEQ(const void* a, const void* b);

//	Copies the number of bytes in the template param from pSrc to pDst.
// Uses MemCopy, except sometimes just a few, fast, tiny instructions if the size is optimal.
template<size_t T_BYTE_COUNT>
FFTL_NODISCARD void* CopyBytes(void* pDst, const void* pSrc);





template<typename T>
FFTL_FORCEINLINE T* MemCopy(T* pDst, const T* pSrc, size_t uCount)
{
	return static_cast<T*>(memcpy(pDst, pSrc, uCount * sizeof(T)));
}

template<typename T, size_t N_DST, size_t N_SRC>
FFTL_FORCEINLINE T* MemCopy(T(&dst)[N_DST], const T(&src)[N_SRC])
{
	return static_cast<T*>(memcpy(dst, src, Min(sizeof(dst), sizeof(src))));
}

template<typename T, size_t N_DST, size_t N_SRC>
FFTL_FORCEINLINE T* MemCopy(FixedArray<T, N_DST>& dst, const FixedArray<T, N_SRC>& src)
{
	return static_cast<T*>(memcpy(&dst, &src, Min(sizeof(dst), sizeof(src))));
}

template<typename T>
FFTL_FORCEINLINE T* MemZero(T* pDst, size_t uCount)
{
	return static_cast<T*>(memset(pDst, 0, uCount * sizeof(T)));
}

template<typename T, size_t N>
FFTL_FORCEINLINE T* MemZero(T(&dst)[N])
{
	return static_cast<T*>(memset(dst, 0, sizeof(dst)));
}

template<typename T, size_t N>
FFTL_FORCEINLINE T* MemZero(FixedArray<T, N>& dst)
{
	return static_cast<T*>(memset(&dst, 0, sizeof(dst)));
}

template<typename T>
FFTL_FORCEINLINE T* MemZero(T& dst)
{
	return static_cast<T*>(memset(&dst, 0, sizeof(dst)));
}

template<uint T_ALIGNMENT, typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T AlignForward(const T& offset)
{
	static_assert((T_ALIGNMENT & (T_ALIGNMENT - 1)) == 0, "Alignment must be power of 2");
	return (offset + (T_ALIGNMENT - 1)) & ~(T_ALIGNMENT - 1);
}
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T AlignForward(size_t alignment, const T& offset)
{
	FFTL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "Alignment must be power of 2");
	return (offset + (alignment - 1)) & ~(alignment - 1);
}

template<uint T_ALIGNMENT, typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T AlignBackward(const T& offset)
{
	static_assert((T_ALIGNMENT & (T_ALIGNMENT - 1)) == 0, "Alignment must be power of 2");
	return offset & ~(T_ALIGNMENT - 1);
}
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T AlignBackward(size_t alignment, const T& offset)
{
	FFTL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "Alignment must be power of 2");
	return offset & ~(alignment - 1);
}

template<typename T, size_t N>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr size_t ArraySize(const T(&)[N])
{
	return N;
}

template <typename T, size_t N>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr size_t ArraySize(const FixedArray<T, N>&)
{
	return N;
}

template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<1>(const void* a, const void* b)
{
	return *reinterpret_cast<const byte*>(a) == *reinterpret_cast<const byte*>(b);
}

template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<2>(const void* a, const void* b)
{
#pragma pack(push, 1)
	struct _ScopedPackedInt { u16 u; };
#pragma pack(pop)

	return reinterpret_cast<const _ScopedPackedInt*>(a)->u == reinterpret_cast<const _ScopedPackedInt*>(b)->u;
}

template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<4>(const void* a, const void* b)
{
#pragma pack(push, 1)
	struct _ScopedPackedInt { u32 u; };
#pragma pack(pop)
	return reinterpret_cast<const _ScopedPackedInt*>(a)->u == reinterpret_cast<const _ScopedPackedInt*>(b)->u;
}

template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<8>(const void* a, const void* b)
{
#pragma pack(push, 1)
	struct _ScopedPackedInt { u64 u; };
#pragma pack(pop)

	return reinterpret_cast<const _ScopedPackedInt*>(a)->u == reinterpret_cast<const _ScopedPackedInt*>(b)->u;
}

#if defined(FFTL_SSE2)
template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<16>(const void* a, const void* b)
{
	const __m128i vA = _mm_loadu_si128(reinterpret_cast<const __m128i*>(a));
	const __m128i vB = _mm_loadu_si128(reinterpret_cast<const __m128i*>(b));
	return _mm_movemask_epi8(_mm_cmpeq_epi8(vA, vB)) == 0xffff;
}

template<>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ<32>(const void* a, const void* b)
{
#if defined(FFTL_AVX2)
	const __m256i vA = _mm256_loadu_si256((const __m256i*)a);
	const __m256i vB = _mm256_loadu_si256((const __m256i*)b);
	return _mm256_movemask_epi8(_mm256_cmpeq_epi8(vA, vB)) == static_cast<int>(0xffffffff);
#else
	const __m128i vA0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(a) + 0);
	const __m128i vB0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(b) + 0);
	const __m128i vA1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(a) + 1);
	const __m128i vB1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(b) + 1);
	const __m128i vCmp0 = _mm_cmpeq_epi8(vA0, vB0);
	const __m128i vCmp1 = _mm_cmpeq_epi8(vA1, vB1);
	return _mm_movemask_epi8(_mm_and_si128(vCmp0, vCmp1)) == 0xffff;
#endif
}

#endif//FFTL_SSE2


template<size_t T_BYTE_COUNT>
FFTL_NODISCARD FFTL_FORCEINLINE bool CompareBytesEQ(const void* a, const void* b)
{
	return memcmp(a, b, T_BYTE_COUNT) == 0;
}

template<size_t T_BYTE_COUNT>
FFTL_FORCEINLINE void* CopyBytes(void* pDst, const void* pSrc)
{
	return MemCopy(static_cast<byte*>(pDst), static_cast<const byte*>(pSrc), T_BYTE_COUNT);
}

template<>
FFTL_FORCEINLINE void* CopyBytes<1>(void* pDst, const void* pSrc)
{
	*reinterpret_cast<u8*>(pDst) = *reinterpret_cast<const u8*>(pSrc);
	return pDst;
}

template<>
FFTL_FORCEINLINE void* CopyBytes<2>(void* pDst, const void* pSrc)
{
#pragma pack(push, 1)
	struct _ScopedPackedInt
	{
		u16 u;
	};
#pragma pack(pop)

	reinterpret_cast<_ScopedPackedInt*>(pDst)->u = reinterpret_cast<const _ScopedPackedInt*>(pSrc)->u;
	return pDst;
}

template<>
FFTL_FORCEINLINE void* CopyBytes<4>(void* pDst, const void* pSrc)
{
#pragma pack(push, 1)
	struct _ScopedPackedInt
	{
		u32 u;
	};
#pragma pack(pop)

	reinterpret_cast<_ScopedPackedInt*>(pDst)->u = reinterpret_cast<const _ScopedPackedInt*>(pSrc)->u;
	return pDst;
}

FFTL_NODISCARD inline bool ScanForAnyNonzero(const f32* pBuffer, size_t nCount)
{
	for (uint i = 0; i < nCount; ++i)
	{
		if (pBuffer[i] != 0.f)
			return true;
	}
	return false;
}

FFTL_NODISCARD inline bool ScanForAnyAboveThreshold(const f32* pBuffer, size_t nCount, f32 fThreshold)
{
	for (uint i = 0; i < nCount; ++i)
	{
		if (Abs(pBuffer[i]) >= fThreshold)
			return true;
	}
	return false;
}

FFTL_NODISCARD inline bool ScanForConsecutiveZeroes(const f32* pBuffer, size_t nCount, size_t nThresholdCountCount)
{
	size_t nZeroCount = 0;
	for (uint i = 0; i < nCount; ++i)
	{
		nZeroCount += pBuffer[i] == 0.f ? 1 : 0;
	}
	return nZeroCount >= nThresholdCountCount;
}

template <typename T>
constexpr void Swap(T& a, T& b)
{
#if 0
	T temp = a;
	a = b;
	b = temp;
#else
	std::swap(a, b);
#endif
}

template <typename T>
inline void ByteReverse(T& obj)
{
	constexpr size_t objSize = sizeof(obj);
	byte* pFnt = static_cast<byte*>(&obj);
	byte* pEnd = static_cast<byte*>(&obj) + objSize - 1;
	for (; pFnt < pEnd; ++pFnt, --pEnd)
		Swap(*pFnt, *pEnd);
}

inline void ByteReverse(void* pObj, size_t objSize)
{
	byte* pFnt = static_cast<byte*>(pObj);
	byte* pEnd = static_cast<byte*>(pObj) + objSize - 1;
	for (; pFnt < pEnd; ++pFnt, --pEnd)
		Swap(*pFnt, *pEnd);
}

FFTL_NODISCARD FFTL_FORCEINLINE bool StringEquals(const char* pszA, const char* pszB)
{
	return strcmp(pszA, pszB) == 0;
}
FFTL_NODISCARD FFTL_FORCEINLINE bool StringIEquals(const char* pszA, const char* pszB)
{
#if defined(_POSIX_VERSION)
	return strcasecmp(pszA, pszB) == 0;
#else
	return _stricmp(pszA, pszB) == 0;
#endif
}
#if defined(FFTL_WCHAR)
FFTL_NODISCARD FFTL_FORCEINLINE bool StringEquals(const wchar_t* pszA, const wchar_t* pszB)
{
	return wcscmp(pszA, pszB) == 0;
}
FFTL_NODISCARD inline bool StringEquals(const wchar_t* pszA, const char* pszB)
{
	for (; ; ++pszA, ++pszB)
	{
		const auto a = *pszA;
		const auto b = *pszB;
		if (a != b)
			return false;
		if (a == 0)
			break;
	}
	return true;
}
FFTL_NODISCARD inline bool StringEquals(const char* pszA, const wchar_t* pszB)
{
	for (; ; ++pszA, ++pszB)
	{
		const auto a = *pszA;
		const auto b = *pszB;
		if (a != b)
			return false;
		if (a == 0)
			break;
	}
	return true;
}
FFTL_NODISCARD FFTL_FORCEINLINE bool StringIEquals(const wchar_t* pszA, const wchar_t* pszB)
{
#if defined(_POSIX_VERSION)
	return wcscasecmp(pszA, pszB) == 0;
#else
	return _wcsicmp(pszA, pszB) == 0;
#endif
}
FFTL_NODISCARD inline bool StringIEquals(const wchar_t* pszA, const char* pszB)
{
	for (; ; ++pszA, ++pszB)
	{
		const auto a = tolower(*pszA);
		const auto b = tolower(*pszB);
		if (a != b)
			return false;
		if (a == 0)
			break;
	}
	return true;
}
FFTL_NODISCARD inline bool StringIEquals(const char* pszA, const wchar_t* pszB)
{
	for (; ; ++pszA, ++pszB)
	{
		const auto a = tolower(*pszA);
		const auto b = tolower(*pszB);
		if (a != b)
			return false;
		if (a == 0)
			break;
	}
	return true;
}

inline size_t ToASCII(char* s, const wchar_t* w, size_t uSize)
{
	const wchar_t* p = w;
	char* q = s;
	char* q0 = q;
	for (size_t i = 0; i < uSize; ++i)
	{
		const wchar_t c = *p++;
		if (c == 0)
			break;
		if (c < 0x080)
			*q++ = safestatic_cast<char>(c);
		else if (c < 0x800)
		{
			*q++ = safestatic_cast<char>(0xC0 + (c >> 6));
			*q++ = safestatic_cast<char>(0x80 + (c & 63));
		}
		else
		{
			*q++ = safestatic_cast<char>(0xE0 + (c >> 12));
			*q++ = safestatic_cast<char>(0x80 + ((c >> 6) & 63));
			*q++ = safestatic_cast<char>(0x80 + (c & 63));
		}
	}
	*q = 0;
	return q - q0;
}

#endif

}
