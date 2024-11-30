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

#include "../../_pch_Core.h"

#if defined(FFTL_SSE2)

#include "../DspPcmConvert.h"

#include "../../../Core/Math/MathCommon.h"
#include "../../../Core/Platform/CpuInfo.h"

#include <smmintrin.h>
#include <tmmintrin.h>


namespace FFTL
{




u32 DspPcmConvert_SSE4::CvtPcmDither(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 16)); //	16 bit reduction 24 -> 8
	constexpr f32 fMin = -128;
	constexpr f32 fMax = +127;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = _mm_set1_ps(fScale);

	auto vS = _mm_load_si128(reinterpret_cast<const __m128i*>(seed));

	const auto mask0 = _mm_setr_epi8(-1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11);
	const auto mask1 = _mm_setr_epi8(-1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15);
	const auto v8bitOffset = _mm_set1_epi8(-128);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(sa, mask0), 8),							vScale, vS);
		const auto db = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 8),	vScale, vS);
		const auto dc = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 8),	vScale, vS);
		const auto dd = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(sc, mask1), 8),							vScale, vS);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vOut00_07 = _mm_packs_epi32(da, db);
		const auto vOut07_15 = _mm_packs_epi32(dc, dd);

		//	Converts 16 signed 16 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		auto vOut00_15 = _mm_packs_epi16(vOut00_07, vOut07_15);
		vOut00_15 = _mm_add_epi8(vOut00_15, v8bitOffset);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vOut00_15);
	}

	_mm_store_si128(reinterpret_cast<__m128i*>(seed), vS);

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		//	Wrap around
		const auto n = i & 3;

		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
		const s32 sample32 = static_cast<s32>(*pSrc);
		const f32 scaled = sample32 * fScale;

		const auto fDither = DitherFloat(seed[n], n);
		auto fDithered = scaled + fDither;
		fDithered = fDithered + 0.5f - static_cast<f32>(fDithered < 0); // push for rounding before truncation.
		fDithered = Clamp(fDithered, fMin, fMax);
		const s32 sOffset = static_cast<s32>(fDithered) + 128;
		*pDst = static_cast<u8>(sOffset);
	}

	return seed[0];
}

u32 DspPcmConvert_SSE4::CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 8)); //	8 bit reduction 24 -> 16
	constexpr f32 fMin = -32768;
	constexpr f32 fMax = +32767;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = _mm_set1_ps(fScale);

	auto vS = _mm_load_si128(reinterpret_cast<const __m128i*>(seed));

	const auto mask0 = _mm_setr_epi8(-1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11);
	const auto mask1 = _mm_setr_epi8(-1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(sa, mask0), 8),							vScale, vS);
		const auto db = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 8),	vScale, vS);
		const auto dc = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 8),	vScale, vS);
		const auto dd = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(sc, mask1), 8),							vScale, vS);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vOut00_07 = _mm_packs_epi32(da, db);
		const auto vOut07_15 = _mm_packs_epi32(dc, dd);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 0), vOut00_07);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 8), vOut07_15);
	}

	_mm_store_si128(reinterpret_cast<__m128i*>(seed), vS);

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		//	Wrap around
		const auto n = i & 3;

		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
		const s32 sample32 = static_cast<s32>(*pSrc);
		const f32 scaled = sample32 * fScale;

		const auto fDither = DitherFloat(seed[n], n);
		auto fDithered = scaled + fDither;
		fDithered = fDithered + 0.5f - static_cast<f32>(fDithered < 0); // push for rounding before truncation.
		fDithered = Clamp(fDithered, fMin, fMax);
		*pDst = static_cast<s16>(fDithered);
	}

	return seed[0];
}

u32 DspPcmConvert_SSE4::CvtPcmDither(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1u << 7u);

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	constexpr size_t step = 16;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = _mm_set1_ps(fScale);
	const auto vOffset = _mm_set1_epi8(128u);

	auto vS = _mm_load_si128(reinterpret_cast<const __m128i*>(seed));

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = _mm_loadu_ps(pSrc + 0x0);
		const auto vIn4_7 = _mm_loadu_ps(pSrc + 0x4);
		const auto vIn8_B = _mm_loadu_ps(pSrc + 0x8);
		const auto vInC_F = _mm_loadu_ps(pSrc + 0xC);

		//	Converts 16 floats to 16 32 bit integers with rounding
		const auto vInt0_3 = Vec4AddDitherFloat32(vIn0_3, vScale, vS);
		const auto vInt4_7 = Vec4AddDitherFloat32(vIn4_7, vScale, vS);
		const auto vInt8_B = Vec4AddDitherFloat32(vIn8_B, vScale, vS);
		const auto vIntC_F = Vec4AddDitherFloat32(vInC_F, vScale, vS);

		//	Converts 16 signed 32 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		const auto vs08_0_F = _mm_packs_epi16( _mm_packs_epi32(vInt0_3, vInt4_7), _mm_packs_epi32(vInt8_B, vIntC_F) );
		const auto vu08_0_F = _mm_add_epi8(vs08_0_F, vOffset);

		//	Unaligned Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vu08_0_F);
	}

	_mm_store_si128(reinterpret_cast<__m128i*>(seed), vS);

	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1;

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		//	Wrap around
		const auto n = i & 3;

		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
		const f32 src = *pSrc;
		const f32 scaled = src * fScale;

		const auto fDither = DitherFloat(seed[n], n);
		auto fDithered = scaled + fDither;
		fDithered = fDithered + 0.5f - static_cast<f32>(fDithered < 0); // push for rounding before truncation.
		fDithered = Clamp(fDithered, fMin, fMax);
		const s32 sOffset = static_cast<s32>(fDithered) + 128;
		*pDst = static_cast<u8>(sOffset);
	}

	return seed[0];
}

u32 DspPcmConvert_SSE4::CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1 << 15);
	constexpr size_t step = 8;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = _mm_set1_ps(fScale);

	auto vS = _mm_load_si128(reinterpret_cast<const __m128i*>(seed));

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = _mm_loadu_ps(pSrc + 0x0);
		const auto vIn4_7 = _mm_loadu_ps(pSrc + 0x4);

		//	Converts 16 floats to 16 32 bit integers with rounding
		const auto vInt0_3 = Vec4AddDitherFloat32(vIn0_3, vScale, vS);
		const auto vInt4_7 = Vec4AddDitherFloat32(vIn4_7, vScale, vS);

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		const auto vConverted = _mm_packs_epi32(vInt0_3, vInt4_7);

		//	Unaligned Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vConverted);
	}

	_mm_store_si128(reinterpret_cast<__m128i*>(seed), vS);

	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1;

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		//	Wrap around
		const auto n = i & 3;

		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
		const f32 src = *pSrc;
		const f32 scaled = src * fScale;

		const auto fDither = DitherFloat(seed[n], n);
		auto fDithered = scaled + fDither;

		fDithered = fDithered + 0.5f - static_cast<f32>(fDithered < 0); // push for rounding before truncation.
		fDithered = Clamp(fDithered, fMin, fMax);
		*pDst = static_cast<s16>(fDithered);
	}

	return seed[0];
}


}

#endif // defined(FFTL_SSE2)