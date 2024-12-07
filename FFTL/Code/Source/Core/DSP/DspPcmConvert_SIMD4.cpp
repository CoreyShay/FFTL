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

#include "../_pch_Core.h"
#include "DspPcmConvert.h"

#include "../Math/MathCommon.h"
#include "../Platform/CpuInfo.h"

#if defined(FFTL_SSE)
#	include <tmmintrin.h>
#	include <smmintrin.h>
#endif


namespace FFTL
{



void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = 1.f / 128;
	constexpr size_t step = 16;
	
	const size_t vecSampleCount = (totalSampleCount / step) * step;

#if defined(FFTL_SSE2)
	const auto vZero = _mm_setzero_si128();
	const auto vScale = _mm_set1_ps(fScale);
	const auto vOffset = _mm_set1_epi8(128u);

	__m128i vs32_00_03, vs32_04_07, vs32_08_11, vs32_12_15;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;
	
		const auto x = _mm_sub_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i*>(pInBuffer + i)), vOffset);

		//	Sign extend by unpacking to the high word of each element, then shifting right
		const auto vs16_00_07 = _mm_unpacklo_epi8(vZero, x);
		const auto vs16_08_15 = _mm_unpackhi_epi8(vZero, x);

		vs32_00_03 = _mm_unpacklo_epi16(vZero, vs16_00_07);
		vs32_04_07 = _mm_unpackhi_epi16(vZero, vs16_00_07);
		vs32_08_11 = _mm_unpacklo_epi16(vZero, vs16_08_15);
		vs32_12_15 = _mm_unpackhi_epi16(vZero, vs16_08_15);
		vs32_00_03 = _mm_srai_epi32(vs32_00_03, 24);
		vs32_04_07 = _mm_srai_epi32(vs32_04_07, 24);
		vs32_08_11 = _mm_srai_epi32(vs32_08_11, 24);
		vs32_12_15 = _mm_srai_epi32(vs32_12_15, 24);

		const auto vf32_00_03 = _mm_cvtepi32_ps(vs32_00_03);
		const auto vf32_04_07 = _mm_cvtepi32_ps(vs32_04_07);
		const auto vf32_08_11 = _mm_cvtepi32_ps(vs32_08_11);
		const auto vf32_12_15 = _mm_cvtepi32_ps(vs32_12_15);

		const auto vOut0_3 = _mm_mul_ps(vf32_00_03, vScale);
		const auto vOut4_7 = _mm_mul_ps(vf32_04_07, vScale);
		const auto vOut8_B = _mm_mul_ps(vf32_08_11, vScale);
		const auto vOutC_F = _mm_mul_ps(vf32_12_15, vScale);

		_mm_storeu_ps(pDst + 0x0, vOut0_3);
		_mm_storeu_ps(pDst + 0x4, vOut4_7);
		_mm_storeu_ps(pDst + 0x8, vOut8_B);
		_mm_storeu_ps(pDst + 0xC, vOutC_F);
	}
#elif defined(FFTL_ARM_NEON)
	const auto vScale = vdupq_n_f32(fScale);
	const auto vOffset = vdup_n_u8(128u);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto vs8_00_07 = vreinterpret_s8_u8( vadd_u8(vld1_u8(pSrc + 0), vOffset) );
		const auto vs8_08_15 = vreinterpret_s8_u8( vadd_u8(vld1_u8(pSrc + 8), vOffset) );

		const auto vs16_00_07 = vmovl_s8(vs8_00_07);
		const auto vs16_08_15 = vmovl_s8(vs8_08_15);

		const auto vs32_00_04 = vmovl_s16(vget_low_s16(vs16_00_07));
		const auto vs32_04_07 = vmovl_s16(vget_high_s16(vs16_00_07));
		const auto vs32_08_11 = vmovl_s16(vget_low_s16(vs16_08_15));
		const auto vs32_12_15 = vmovl_s16(vget_high_s16(vs16_08_15));

		const auto vf32_00_03 = vcvtq_f32_s32(vs32_00_04);
		const auto vf32_04_07 = vcvtq_f32_s32(vs32_04_07);
		const auto vf32_08_11 = vcvtq_f32_s32(vs32_08_11);
		const auto vf32_12_15 = vcvtq_f32_s32(vs32_12_15);

		const auto vOut00_03 = vmulq_f32(vf32_00_03, vScale);
		const auto vOut04_07 = vmulq_f32(vf32_04_07, vScale);
		const auto vOut08_11 = vmulq_f32(vf32_08_11, vScale);
		const auto vOut12_15 = vmulq_f32(vf32_12_15, vScale);

		vst1q_f32(pDst + 0x0, vOut00_03);
		vst1q_f32(pDst + 0x4, vOut04_07);
		vst1q_f32(pDst + 0x8, vOut08_11);
		vst1q_f32(pDst + 0xC, vOut12_15);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		const auto ff32 = ss8 * fScale;
		pOutBuffer[i] = ff32;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = 1.f / 32768;
	constexpr size_t step = 8;

	const size_t vecSampleCount = ( totalSampleCount / step ) * step;

#if defined(FFTL_SSE2)
	const auto vZero = _mm_setzero_si128();
	const auto vScale = _mm_set1_ps(fScale);

	__m128i a, b;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pInBuffer + i));
		auto* pDst = pOutBuffer + i;

		//	Sign extend by unpacking to the high word of each element, then shifting right
		a = _mm_unpacklo_epi16(vZero, x);
		b = _mm_unpackhi_epi16(vZero, x);
		a = _mm_srai_epi32(a, 16);
		b = _mm_srai_epi32(b, 16);

		const auto af = _mm_cvtepi32_ps(a);
		const auto bf = _mm_cvtepi32_ps(b);

		const auto vOut0_3 = _mm_mul_ps(af, vScale);
		const auto vOut4_7 = _mm_mul_ps(bf, vScale);

		_mm_storeu_ps(pDst + 0, vOut0_3);
		_mm_storeu_ps(pDst + 4, vOut4_7);
	}
#elif defined(FFTL_ARM_NEON)
	const auto vScale = vdupq_n_f32(fScale);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto x = vld1_s16(pSrc + 0);
		const auto y = vld1_s16(pSrc + 4);

		const auto a = vmovl_s16(x);
		const auto b = vmovl_s16(y);

		const auto af = vcvtq_f32_s32(a);
		const auto bf = vcvtq_f32_s32(b);

		const auto vOut0_3 = vmulq_f32(af, vScale);
		const auto vOut4_7 = vmulq_f32(bf, vScale);

		vst1q_f32(pDst + 0, vOut0_3);
		vst1q_f32(pDst + 4, vOut4_7);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i] * fScale;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 23));
	constexpr size_t step = 16;

	const size_t vecSampleCount = ( totalSampleCount / step ) * step;

#if defined(FFTL_SSE2)
	const auto vScale = _mm_set1_ps(fScale);
	const auto mask0 = _mm_setr_epi8(-1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11);
	const auto mask1 = _mm_setr_epi8(-1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = _mm_srai_epi32(_mm_shuffle_epi8(sa, mask0), 8);
		const auto db = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 8);
		const auto dc = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 8);
		const auto dd = _mm_srai_epi32(_mm_shuffle_epi8(sc, mask1), 8);

		//	Convert to float
		const auto vOut00_03 = _mm_mul_ps(_mm_cvtepi32_ps(da), vScale);
		const auto vOut04_07 = _mm_mul_ps(_mm_cvtepi32_ps(db), vScale);
		const auto vOut08_11 = _mm_mul_ps(_mm_cvtepi32_ps(dc), vScale);
		const auto vOut12_15 = _mm_mul_ps(_mm_cvtepi32_ps(dd), vScale);

		//	Store
		_mm_storeu_ps(pDst +  0, vOut00_03);
		_mm_storeu_ps(pDst +  4, vOut04_07);
		_mm_storeu_ps(pDst +  8, vOut08_11);
		_mm_storeu_ps(pDst + 12, vOut12_15);
	}

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	const auto vScale = vdupq_n_f32(fScale);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 8);
		const auto db = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 8);
		const auto dc = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 8);
		const auto dd = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 8);

		//	Convert to float
		const auto vOut00_03 = vmulq_f32(vcvtq_f32_s32(da), vScale);
		const auto vOut04_07 = vmulq_f32(vcvtq_f32_s32(db), vScale);
		const auto vOut08_11 = vmulq_f32(vcvtq_f32_s32(dc), vScale);
		const auto vOut12_15 = vmulq_f32(vcvtq_f32_s32(dd), vScale);

		//	Store
		vst1q_f32(pDst +  0, vOut00_03);
		vst1q_f32(pDst +  4, vOut04_07);
		vst1q_f32(pDst +  8, vOut08_11);
		vst1q_f32(pDst + 12, vOut12_15);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = (static_cast<s32>(pInBuffer[i])) * fScale;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1u << 31u));
	constexpr size_t step = 8;

	const auto vecSampleCount = ( totalSampleCount / step ) * step;

#if defined(FFTL_SSE2)
	const auto vScale = _mm_set1_ps(fScale);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc + 0));
		const auto b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc + 4));

		const auto af = _mm_cvtepi32_ps(a);
		const auto bf = _mm_cvtepi32_ps(b);

		const auto vOut0_3 = _mm_mul_ps(af, vScale);
		const auto vOut4_7 = _mm_mul_ps(bf, vScale);

		_mm_storeu_ps(pDst + 0, vOut0_3);
		_mm_storeu_ps(pDst + 4, vOut4_7);
	}
#elif defined(FFTL_ARM_NEON)
	const auto vScale = vdupq_n_f32(fScale);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto a = vld1q_s32(pSrc + 0);
		const auto b = vld1q_s32(pSrc + 4);

		const auto af = vcvtq_f32_s32(a);
		const auto bf = vcvtq_f32_s32(b);

		const auto vOut0_3 = vmulq_f32(af, vScale);
		const auto vOut4_7 = vmulq_f32(bf, vScale);

		vst1q_f32(pDst + 0, vOut0_3);
		vst1q_f32(pDst + 4, vOut4_7);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<f32>(pInBuffer[i]) * fScale;
	}
}
/*
void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount)
{
	constexpr f64 fScale = 1.0 / (1ull << 63ull);
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const f64 f = static_cast<f64>(pInBuffer[i]) * fScale;
		pOutBuffer[i] = static_cast<f32>(f);
	}
}

void DspPcmConvert_SIMD4::CvtPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<f32>(pInBuffer[i]);
	}
}
*/
void DspPcmConvert_SIMD4::CvtPcm(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1u << 7u);

#if defined(FFTL_SSE2)
	constexpr size_t step = 16;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = _mm_set1_ps(fScale);
	const auto vOffset = _mm_set1_epi8(128u);

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
		const auto vInt0_3 = _mm_cvtps_epi32(_mm_mul_ps(vIn0_3, vScale));
		const auto vInt4_7 = _mm_cvtps_epi32(_mm_mul_ps(vIn4_7, vScale));
		const auto vInt8_B = _mm_cvtps_epi32(_mm_mul_ps(vIn8_B, vScale));
		const auto vIntC_F = _mm_cvtps_epi32(_mm_mul_ps(vInC_F, vScale));

		//	Converts 16 signed 32 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		const auto vs08_0_F = _mm_packs_epi16( _mm_packs_epi32(vInt0_3, vInt4_7), _mm_packs_epi32(vInt8_B, vIntC_F) );
		const auto vu08_0_F = _mm_add_epi8(vs08_0_F, vOffset);

		//	Unaligned Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vu08_0_F);
	}
#elif defined(FFTL_ARM_NEON)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = vdupq_n_f32(fScale);
	const auto vOffset = vdup_n_u8(128);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = vld1q_f32(pSrc + 0);
		const auto vIn4_7 = vld1q_f32(pSrc + 4);

		//	Converts 4 floats to 4 32 bit integers with rounding (twice for 8)
		const auto vs32_0_3 = V4fRoundToVfi(vmulq_f32(vIn0_3, vScale));
		const auto vs32_4_7 = V4fRoundToVfi(vmulq_f32(vIn4_7, vScale));

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_0_3 = vqmovn_s32(vs32_0_3);
		const auto vs16_4_7 = vqmovn_s32(vs32_4_7);
		const auto vs16_0_7 = vcombine_s16(vs16_0_3, vs16_4_7);
		const auto vs08_0_7 = vqmovn_s16(vs16_0_7);
		const auto vu08_0_7 = vadd_u8( vreinterpret_u8_s8(vs08_0_7), vOffset );

		//	Unaligned Store
		vst1_u8(pDst, vu08_0_7);
	}
#else
	constexpr size_t vecSampleCount = 0;
#endif

	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1;

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		f32 const src = *pSrc;
		f32 scaled = src * fScale;
		scaled = (scaled > 0) ? scaled + 0.5f : scaled - 0.5f; // push for rounding before truncation.
		scaled = Clamp(scaled, fMin, fMax);
		const s32 sOffset = static_cast<s32>(scaled) + 128;
		*pDst = static_cast<u8>(sOffset);
	}
}

void DspPcmConvert_SIMD4::CvtPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1u << 15u);
	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1; // 0x4EFFFFFF

#if defined(FFTL_SSE2)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = _mm_set1_ps(fScale);

	__m128i vConverted = _mm_setzero_si128();

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = _mm_loadu_ps(pSrc + 0);
		const auto vIn4_7 = _mm_loadu_ps(pSrc + 4);

		//	Converts 4 floats to 4 32 bit integers with rounding (twice for 8)
		const auto vInt0_3 = _mm_cvtps_epi32(_mm_mul_ps(vIn0_3, vScale));
		const auto vInt4_7 = _mm_cvtps_epi32(_mm_mul_ps(vIn4_7, vScale));

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		vConverted = _mm_packs_epi32(vInt0_3, vInt4_7);

		//	Unaligned Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vConverted);
	}

	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto vIn0_3 = _mm_load_ss(pSrc);
		const auto vInt0_3 = _mm_cvtps_epi32(_mm_mul_ss(vIn0_3, vScale));
		vConverted = _mm_packs_epi32(vInt0_3, vConverted);//2nd param doesn't matter
		*pDst = static_cast<s16>(_mm_extract_epi16(vConverted, 0));
	}

#elif defined(FFTL_ARM_NEON)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = vdupq_n_f32(fScale);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = vld1q_f32(pSrc + 0);
		const auto vIn4_7 = vld1q_f32(pSrc + 4);

		//	Converts 4 floats to 4 32 bit integers with rounding (twice for 8)
		const auto vInt0_3 = V4fRoundToVfi(vmulq_f32(vIn0_3, vScale));
		const auto vInt4_7 = V4fRoundToVfi(vmulq_f32(vIn4_7, vScale));

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_0_3 = vqmovn_s32(vInt0_3);
		const auto vs16_4_7 = vqmovn_s32(vInt4_7);

		//	Unaligned Store
		vst1_s16(pDst + 0, vs16_0_3);
		vst1_s16(pDst + 4, vs16_4_7);
	}

	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn = vld1q_lane_f32(pSrc, vdupq_n_f32(0), 0);

		//	Converts float to 32 bit integers with rounding
		const auto vInt = V4fRoundToVfi(vmulq_f32(vIn, vScale));

		//	Converts signed 32 bit integer to signed 16 bit integer with automatic saturation (clamping)
		const auto vs16 = vqmovn_s32(vInt);

		//	Unaligned Store
		vst1_lane_s16(pDst, vs16, 0);
	}
#else
	constexpr size_t vecSampleCount = 0;
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		f32 fOut = pInBuffer[i];
		fOut *= fScale;
		fOut = Clamp(fOut, fMin, fMax);
		s16 nOut = static_cast<s16>(fOut + 0.5f - static_cast<f32>(fOut < 0));
		pOutBuffer[i] = nOut;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	//	Note: Clamps the output from -2147483648 to +2147483520, which leaves 127 samples unused at the positive end.
	// To use the full range, double precision must be used.

	constexpr f32 fScale = static_cast<f32>(1u << 31u);
	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = 2147483520.f; // 0x4EFFFFFF

#if defined(FFTL_SSE2)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = _mm_set1_ps(fScale);
	const auto vMin = _mm_set1_ps(fMin);
	const auto vMax = _mm_set1_ps(fMax);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto a = _mm_loadu_ps(pSrc + 0);
		const auto b = _mm_loadu_ps(pSrc + 4);

		//	Scaled up
		const auto as = _mm_mul_ps(a, vScale);
		const auto bs = _mm_mul_ps(b, vScale);

		//	Clamped
		const auto ac = _mm_max_ps(_mm_min_ps(as, vMax), vMin);
		const auto bc = _mm_max_ps(_mm_min_ps(bs, vMax), vMin);

		//	Convert to 32 bit signed integers
		const auto vOut0_3 = _mm_cvtps_epi32(ac);
		const auto vOut4_7 = _mm_cvtps_epi32(bc);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 0), vOut0_3);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 4), vOut4_7);
	}

#elif defined(FFTL_ARM_NEON)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = vdupq_n_f32(fScale);
	const auto vMin = vdupq_n_f32(fMin);
	const auto vMax = vdupq_n_f32(fMax);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto a = vld1q_f32(pSrc + 0);
		const auto b = vld1q_f32(pSrc + 4);

		//	Scaled up
		const auto as = vmulq_f32(a, vScale);
		const auto bs = vmulq_f32(b, vScale);

		//	Clamped
		const auto ac = vmaxq_f32(vminq_f32(as, vMax), vMin);
		const auto bc = vmaxq_f32(vminq_f32(bs, vMax), vMin);

		//	Convert to 32 bit signed integers
		const auto vOut0_3 = V4fRoundToVfi(ac);
		const auto vOut4_7 = V4fRoundToVfi(bc);

		//	Store
		vst1q_s32(pDst + 0, vOut0_3);
		vst1q_s32(pDst + 4, vOut4_7);
	}
#else
	constexpr size_t vecSampleCount = 0;
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		f32 fOut = pInBuffer[i];
		fOut *= fScale;
		fOut = Clamp(fOut, fMin, fMax);
		s32 nOut = static_cast<s32>(fOut + 0.5f - static_cast<f32>(fOut < 0));
		pOutBuffer[i] = nOut;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 16;
	const size_t vecSampleCount = ( totalSampleCount / step ) * step;

#if defined(FFTL_SSE2)
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

		const auto da = _mm_srai_epi32(_mm_shuffle_epi8(sa, mask0), 24);
		const auto db = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 24);
		const auto dc = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 24);
		const auto dd = _mm_srai_epi32(_mm_shuffle_epi8(sc, mask1), 24);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vOut00_07 = _mm_packs_epi32(da, db);
		const auto vOut07_15 = _mm_packs_epi32(dc, dd);
		
		//	Converts 16 signed 16 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		auto vOut00_15 = _mm_packs_epi16(vOut00_07, vOut07_15);
		vOut00_15 = _mm_add_epi8(vOut00_15, v8bitOffset);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vOut00_15);
	}

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	const auto v8bitOffset = vdup_n_s8(-128);
	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 24);
		const auto db = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 24);
		const auto dc = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 24);
		const auto dd = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 24);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_00_03 = vqmovn_s32(da);
		const auto vs16_04_07 = vqmovn_s32(db);
		const auto vs16_08_11 = vqmovn_s32(dc);
		const auto vs16_12_15 = vqmovn_s32(dd);

		//	Converts 16 signed 16 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		const auto vs16_00_07 = vqmovn_s16(vcombine_s16(vs16_00_03, vs16_04_07));
		const auto vs16_08_15 = vqmovn_s16(vcombine_s16(vs16_08_11, vs16_12_15));

		//	Unaligned Store
		vst1_s8(reinterpret_cast<s8*>(pDst + 0), vadd_s8(vs16_00_07, v8bitOffset));
		vst1_s8(reinterpret_cast<s8*>(pDst + 8), vadd_s8(vs16_08_15, v8bitOffset));
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
#if defined(FFTL_LITTLEENDIAN)
		const s8 dst = *reinterpret_cast<const s8*>( reinterpret_cast<const byte*>(pSrc) + 2 );
#else
		const s8 dst = *reinterpret_cast<const s8*>(pSrc);
#endif
		const u8 sOffset = static_cast<u8>(dst + 128);

		*pDst = sOffset;
	}
}

u32 DspPcmConvert_SIMD4::CvtPcmDither(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 16)); //	16 bit reduction 24 -> 8
	constexpr f32 fMin = -128;
	constexpr f32 fMax = +127;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

#if defined(FFTL_SSE2)
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
		const auto db = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 8), vScale, vS);
		const auto dc = Vec4AddDitherInt32(_mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 8), vScale, vS);
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

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = vdupq_n_f32(fScale);
	auto vS = vld1q_u32(seed);

	const auto v8bitOffset = vdup_n_u8(128);
	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 8), vScale, vS);
		const auto db = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 8), vScale, vS);
		const auto dc = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 8), vScale, vS);
		const auto dd = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 8), vScale, vS);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_00_03 = vqmovn_s32(da);
		const auto vs16_04_07 = vqmovn_s32(db);
		const auto vs16_08_11 = vqmovn_s32(dc);
		const auto vs16_12_15 = vqmovn_s32(dd);

		//	Converts 16 signed 16 bit integers to 16 signed 8 bit integers with automatic saturation (clamping)
		const auto vs16_00_07 = vqmovn_s16(vcombine_s16(vs16_00_03, vs16_04_07));
		const auto vs16_08_15 = vqmovn_s16(vcombine_s16(vs16_08_11, vs16_12_15));

		//	Unaligned Store
		vst1_u8(pDst + 0, vadd_u8(vreinterpret_u8_s8(vs16_00_07), v8bitOffset));
		vst1_u8(pDst + 8, vadd_u8(vreinterpret_u8_s8(vs16_08_15), v8bitOffset));
	}

	vst1q_u32(seed, vS);

#else
	constexpr size_t vecSampleCount = 0;
#endif

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

void DspPcmConvert_SIMD4::CvtPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 16;
	const size_t vecSampleCount = ( totalSampleCount / step ) * step;

#if defined(FFTL_SSE2)
	const auto mask0 = _mm_setr_epi8(-1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11);
	const auto mask1 = _mm_setr_epi8(-1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = _mm_srai_epi32(_mm_shuffle_epi8(sa, mask0), 16);
		const auto db = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask0), 16);
		const auto dc = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask0), 16);
		const auto dd = _mm_srai_epi32(_mm_shuffle_epi8(sc, mask1), 16);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vOut00_07 = _mm_packs_epi32(da, db);
		const auto vOut07_15 = _mm_packs_epi32(dc, dd);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vOut00_07);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst), vOut07_15);
	}

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 16);
		const auto db = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 16);
		const auto dc = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 16);
		const auto dd = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 16);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_00_03 = vqmovn_s32(da);
		const auto vs16_04_07 = vqmovn_s32(db);
		const auto vs16_08_11 = vqmovn_s32(dc);
		const auto vs16_12_15 = vqmovn_s32(dd);

		//	Unaligned Store
		vst1_s16(pDst +  0, vs16_00_03);
		vst1_s16(pDst +  4, vs16_04_07);
		vst1_s16(pDst +  8, vs16_08_11);
		vst1_s16(pDst + 12, vs16_12_15);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;
#if defined(FFTL_LITTLEENDIAN)
		const s16 dst = *reinterpret_cast<const s16*>( reinterpret_cast<const byte*>(pSrc) + 1 );
#else
		const s16 dst = *reinterpret_cast<const s16*>(pSrc);
#endif
		*pDst = dst;
	}
}

void DspPcmConvert_SIMD4::CvtPcm(s16* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 16;
	
	const size_t vecSampleCount = (totalSampleCount / step) * step;

#if defined(FFTL_SSE2)
	const auto vZero = _mm_setzero_si128();
	const auto vOffset = _mm_set1_epi16(-(1 << 15));

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;
	
		const auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pInBuffer + i));

		//	Unpack to the high word of each element, then apply the correction offset
		const auto vs16_00_07 = _mm_add_epi16(_mm_unpacklo_epi8(vZero, x), vOffset);
		const auto vs16_08_15 = _mm_add_epi16(_mm_unpackhi_epi8(vZero, x), vOffset);

		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 0x0), vs16_00_07);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 0x8), vs16_08_15);
	}
#elif defined(FFTL_ARM_NEON)
	const auto vOffset = vdup_n_u8(128u);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto vs8_00_07 = vreinterpret_s8_u8( vsub_u8(vld1_u8(pSrc + 0), vOffset) );
		const auto vs8_08_15 = vreinterpret_s8_u8( vsub_u8(vld1_u8(pSrc + 8), vOffset) );

		const auto vs16_00_07 = vshlq_n_s16(vmovl_s8(vs8_00_07), 8);
		const auto vs16_08_15 = vshlq_n_s16(vmovl_s8(vs8_08_15), 8);

		vst1q_s16(pDst + 0x0, vs16_00_07);
		vst1q_s16(pDst + 0x8, vs16_08_15);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const u8 uu8 = pInBuffer[i];
		const s8 ss8 = static_cast<s8>((uu8 - 128u));
		const s16 ss16 = static_cast<s16>(ss8) << 8;
		pOutBuffer[i] = ss16;
	}
}

u32 DspPcmConvert_SIMD4::CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 8)); //	8 bit reduction 24 -> 16
	constexpr f32 fMin = -32768;
	constexpr f32 fMax = +32767;
	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

#if defined(FFTL_SSE2)
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

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	const auto vScale = vdupq_n_f32(fScale);
	auto vS = vld1q_u32(seed);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 8), vScale, vS);
		const auto db = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 8), vScale, vS);
		const auto dc = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 8), vScale, vS);
		const auto dd = Vec4AddDitherInt32(vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 8), vScale, vS);

		//	Converts 16 signed 32 bit integers to 16 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_00_03 = vqmovn_s32(da);
		const auto vs16_04_07 = vqmovn_s32(db);
		const auto vs16_08_11 = vqmovn_s32(dc);
		const auto vs16_12_15 = vqmovn_s32(dd);

		//	Unaligned Store
		vst1_s16(pDst +  0, vs16_00_03);
		vst1_s16(pDst +  4, vs16_04_07);
		vst1_s16(pDst +  8, vs16_08_11);
		vst1_s16(pDst + 12, vs16_12_15);
	}

	vst1q_u32(seed, vS);

#else
	constexpr size_t vecSampleCount = 0;
#endif

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

u32 DspPcmConvert_SIMD4::CvtPcmDither(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1u << 7u);
	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

#if defined(FFTL_SSE2)
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

#elif defined(FFTL_ARM_NEON)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = vdupq_n_f32(fScale);
	const auto vOffset = vdup_n_u8(128);
	auto vS = vld1q_u32(seed);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = vld1q_f32(pSrc + 0);
		const auto vIn4_7 = vld1q_f32(pSrc + 4);

		//	Converts 4 floats to 4 32 bit integers with rounding (twice for 8)
		const auto vs32_0_3 = Vec4AddDitherFloat32(vIn0_3, vScale, vS);
		const auto vs32_4_7 = Vec4AddDitherFloat32(vIn4_7, vScale, vS);

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_0_3 = vqmovn_s32(vs32_0_3);
		const auto vs16_4_7 = vqmovn_s32(vs32_4_7);
		const auto vs16_0_7 = vcombine_s16(vs16_0_3, vs16_4_7);
		const auto vs08_0_7 = vqmovn_s16(vs16_0_7);
		const auto vu08_0_7 = vadd_u8( vreinterpret_u8_s8(vs08_0_7), vOffset );

		//	Unaligned Store
		vst1_u8(pDst, vu08_0_7);
	}

	vst1q_u32(seed, vS);
#else
	constexpr size_t vecSampleCount = 0;
#endif

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

u32 DspPcmConvert_SIMD4::CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1 << 15);

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

#if defined(FFTL_SSE2)
	constexpr size_t step = 8;
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

#elif defined(FFTL_ARM_NEON)
	constexpr size_t step = 8;
	const auto vecSampleCount = (totalSampleCount / step) * step;
	const auto vScale = vdupq_n_f32(fScale);
	auto vS = vld1q_u32(seed);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		//	Unaligned loads
		const auto vIn0_3 = vld1q_f32(pSrc + 0);
		const auto vIn4_7 = vld1q_f32(pSrc + 4);

		//	Converts 4 floats to 4 32 bit integers with rounding (twice for 8)
		const auto vs32_0_3 = Vec4AddDitherFloat32(vIn0_3, vScale, vS);
		const auto vs32_4_7 = Vec4AddDitherFloat32(vIn4_7, vScale, vS);

		//	Converts 8 signed 32 bit integers to 8 signed 16 bit integers with automatic saturation (clamping)
		const auto vs16_0_3 = vqmovn_s32(vs32_0_3);
		const auto vs16_4_7 = vqmovn_s32(vs32_4_7);

		//	Unaligned Store
		vst1_s16(pDst + 0, vs16_0_3);
		vst1_s16(pDst + 4, vs16_4_7);
	}

	vst1q_u32(seed, vS);
#else
	constexpr size_t vecSampleCount = 0;
#endif

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

void DspPcmConvert_SIMD4::IntCvt(s32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 16;

	const size_t vecSampleCount = (totalSampleCount / step) * step;

#if defined(FFTL_SSE2)
	const auto vZero = _mm_setzero_si128();
	const auto vOffset = _mm_set1_epi8(128u);

	__m128i vs32_00_03, vs32_04_07, vs32_08_11, vs32_12_15;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;
	
		const auto x = _mm_sub_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i*>(pInBuffer + i)), vOffset);

		//	Sign extend by unpacking to the high word of each element, then shifting right
		const auto vs16_00_07 = _mm_unpacklo_epi8(vZero, x);
		const auto vs16_08_15 = _mm_unpackhi_epi8(vZero, x);

		vs32_00_03 = _mm_unpacklo_epi16(vZero, vs16_00_07);
		vs32_04_07 = _mm_unpackhi_epi16(vZero, vs16_00_07);
		vs32_08_11 = _mm_unpacklo_epi16(vZero, vs16_08_15);
		vs32_12_15 = _mm_unpackhi_epi16(vZero, vs16_08_15);
		vs32_00_03 = _mm_srai_epi32(vs32_00_03, 24);
		vs32_04_07 = _mm_srai_epi32(vs32_04_07, 24);
		vs32_08_11 = _mm_srai_epi32(vs32_08_11, 24);
		vs32_12_15 = _mm_srai_epi32(vs32_12_15, 24);

		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  0), vs32_00_03);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  4), vs32_04_07);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  8), vs32_08_11);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 12), vs32_12_15);
	}

#elif defined(FFTL_ARM_NEON)
	const auto vOffset = vdup_n_u8(128u);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto vs8_00_07 = vreinterpret_s8_u8( vadd_u8(vld1_u8(pSrc + 0), vOffset) );
		const auto vs8_08_15 = vreinterpret_s8_u8( vadd_u8(vld1_u8(pSrc + 8), vOffset) );

		const auto vs16_00_07 = vmovl_s8(vs8_00_07);
		const auto vs16_08_15 = vmovl_s8(vs8_08_15);

		const auto vs32_00_04 = vmovl_s16(vget_low_s16(vs16_00_07));
		const auto vs32_04_07 = vmovl_s16(vget_high_s16(vs16_00_07));
		const auto vs32_08_11 = vmovl_s16(vget_low_s16(vs16_08_15));
		const auto vs32_12_15 = vmovl_s16(vget_high_s16(vs16_08_15));

		vst1q_s32(pDst +  0, vs32_00_04);
		vst1q_s32(pDst +  4, vs32_04_07);
		vst1q_s32(pDst +  8, vs32_08_11);
		vst1q_s32(pDst + 12, vs32_12_15);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		pOutBuffer[i] = ss8;
	}
}

void DspPcmConvert_SIMD4::IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 8;

	const size_t vecSampleCount = (totalSampleCount / step) * step;

#if defined(FFTL_SSE2)
	const auto vZero = _mm_setzero_si128();

	__m128i a, b;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pInBuffer + i));
		auto* pDst = pOutBuffer + i;

		//	Sign extend by unpacking to the high word of each element, then shifting right
		a = _mm_unpacklo_epi16(vZero, x);
		b = _mm_unpackhi_epi16(vZero, x);
		a = _mm_srai_epi32(a, 16);
		b = _mm_srai_epi32(b, 16);

		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 0), a);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 4), b);
	}

#elif defined(FFTL_ARM_NEON)
	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		const auto x = vld1_s16(pSrc + 0);
		const auto y = vld1_s16(pSrc + 4);

		const auto vOut0_3 = vmovl_s16( x );
		const auto vOut4_7 = vmovl_s16( y );

		vst1q_s32(pDst + 0, vOut0_3);
		vst1q_s32(pDst + 4, vOut4_7);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i];
	}
}

void DspPcmConvert_SIMD4::IntCvt(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 16;

	const size_t vecSampleCount = (totalSampleCount / step) * step;

#if defined(FFTL_SSE2)
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE3));
	const auto mask = _mm_setr_epi8(-1, 0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = _mm_srai_epi32(_mm_shuffle_epi8(sa, mask), 8);
		const auto db = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask), 8);
		const auto dc = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask), 8);
		const auto dd = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sc,  4), mask), 8);

		//	Store
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  0), da);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  4), db);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst +  8), dc);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(pDst + 12), dd);
	}

#elif defined(FFTL_ARM_NEON) && defined(__clang__)
	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = reinterpret_cast<const s8*>(pInBuffer + i);
		auto* pDst = pOutBuffer + i;

		const auto sa = vld1q_s8(pSrc +  0);
		const auto sb = vld1q_s8(pSrc + 16);
		const auto sc = vld1q_s8(pSrc + 32);

		const auto da = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sa, -1,  0,  1,  2, -1,  3,  4,  5, -1,  6,  7,  8, -1,  9, 10, 11)), 8);
		const auto db = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sa, sb, -1, 12, 13, 14, -1, 15, 16, 17, -1, 18, 19, 20, -1, 21, 22, 23)), 8);
		const auto dc = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sb, sc, -1,  8,  9, 10, -1, 11, 12, 13, -1, 14, 15, 16, -1, 17, 18, 19)), 8);
		const auto dd = vshrq_n_s32(vreinterpretq_s32_s8(__builtin_shufflevector(sc, sc, -1,  4,  5,  6, -1,  7,  8,  9, -1, 10, 11, 12, -1, 13, 14, 15)), 8);

		//	Store
		vst1q_s32(pDst +  0, da);
		vst1q_s32(pDst +  4, db);
		vst1q_s32(pDst +  8, dc);
		vst1q_s32(pDst + 12, dd);
	}
#endif

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<s32>(pInBuffer[i]);
	}
}

void DspPcmConvert_SIMD4::DeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 8;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i * 2;
		auto* pDst0 = pOutBuffer0 + i;
		auto* pDst1 = pOutBuffer1 + i;

		//	c = channel, s = sample, src_cs_cs_cs_cs
		const auto src_00_10_01_11 = FFTL::f32x4::LoadU(pSrc + 0x0);
		const auto src_02_12_03_13 = FFTL::f32x4::LoadU(pSrc + 0x4);
		const auto src_04_14_05_15 = FFTL::f32x4::LoadU(pSrc + 0x8);
		const auto src_06_16_07_17 = FFTL::f32x4::LoadU(pSrc + 0xC);

		const auto src_00_01_02_03 = FFTL::Permute<0, 2, 4, 6>(src_00_10_01_11, src_02_12_03_13);
		const auto src_10_11_12_13 = FFTL::Permute<1, 3, 5, 7>(src_00_10_01_11, src_02_12_03_13);
		const auto src_20_21_22_23 = FFTL::Permute<0, 2, 4, 6>(src_04_14_05_15, src_06_16_07_17);
		const auto src_30_31_32_33 = FFTL::Permute<1, 3, 5, 7>(src_04_14_05_15, src_06_16_07_17);

		src_00_01_02_03.StoreU(pDst0 + 0);
		src_10_11_12_13.StoreU(pDst1 + 0);
		src_20_21_22_23.StoreU(pDst0 + 4);
		src_30_31_32_33.StoreU(pDst1 + 4);
	}

	//	Finish remaining samples non-vectorized
	for (size_t i = vecSampleCount; i < totalSampleCount; i++)
	{
		const auto* pSrc = pInBuffer + i * 2;

		pOutBuffer0[i] = pSrc[0];
		pOutBuffer1[i] = pSrc[1];
	}
}

void DspPcmConvert_SIMD4::DeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr size_t step = 4;
	const size_t vecSampleCount = (totalSampleCount / step) * step;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* pSrc = pInBuffer + i * 4;
		auto* pDst0 = pOutBuffer0 + i;
		auto* pDst1 = pOutBuffer1 + i;
		auto* pDst2 = pOutBuffer2 + i;
		auto* pDst3 = pOutBuffer3 + i;

		//	c = channel, s = sample, src_cs_cs_cs_cs
		const auto src_00_10_20_30 = FFTL::f32x4::LoadU(pSrc + 0x0);
		const auto src_01_11_21_31 = FFTL::f32x4::LoadU(pSrc + 0x4);
		const auto src_02_12_22_32 = FFTL::f32x4::LoadU(pSrc + 0x8);
		const auto src_03_13_23_33 = FFTL::f32x4::LoadU(pSrc + 0xC);

		const auto src_00_20_01_21 = FFTL::Permute<0, 2, 4, 6>(src_00_10_20_30, src_01_11_21_31);
		const auto src_10_30_11_31 = FFTL::Permute<1, 3, 5, 7>(src_00_10_20_30, src_01_11_21_31);
		const auto src_02_22_03_23 = FFTL::Permute<0, 2, 4, 6>(src_02_12_22_32, src_03_13_23_33);
		const auto src_12_32_13_33 = FFTL::Permute<1, 3, 5, 7>(src_02_12_22_32, src_03_13_23_33);

		const auto src_00_01_02_03 = FFTL::Permute<0, 2, 4, 6>(src_00_20_01_21, src_02_22_03_23);
		const auto src_20_21_22_23 = FFTL::Permute<1, 3, 5, 7>(src_00_20_01_21, src_02_22_03_23);
		const auto src_10_11_12_31 = FFTL::Permute<0, 2, 4, 6>(src_10_30_11_31, src_12_32_13_33);
		const auto src_30_31_32_33 = FFTL::Permute<1, 3, 5, 7>(src_10_30_11_31, src_12_32_13_33);

		src_00_01_02_03.StoreU(pDst0);
		src_10_11_12_31.StoreU(pDst1);
		src_20_21_22_23.StoreU(pDst2);
		src_30_31_32_33.StoreU(pDst3);
	}

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i++)
	{
		const auto* pSrc = pInBuffer + i * 4;

		pOutBuffer0[i] = pSrc[0];
		pOutBuffer1[i] = pSrc[1];
		pOutBuffer2[i] = pSrc[2];
		pOutBuffer3[i] = pSrc[3];
	}
}


}
