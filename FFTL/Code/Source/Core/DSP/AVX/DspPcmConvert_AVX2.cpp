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

#include "_pch_AudioEngine.h"

//	If we aren't forcing compilation with AVX2, Windows platforms might support this anyway. Xbox platforms need this to link with debug builds.
#if defined(__AVX2__) || defined(FFTL_AVX2) || ( defined(FFTL_SSE) && defined(FFTL_PLATFORM_WINDOWS) ) || ( defined(FFTL_PLATFORM_XBOX) && defined(FFTL_ENABLE_ASSERT) )

#include "../DspPcmConvert.h"

#include "../../../Core/Math/MathCommon.h"
#include "../../../Core/Platform/CpuInfo.h"

#include <immintrin.h>


namespace FFTL
{

void DspPcmConvert_SIMD8::CvtPcm(f32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2));

	constexpr f32 fScale = 1.f / 128;
	constexpr size_t step = 32;
	const size_t vecSampleCount = (totalSampleCount / step) * step;
	const auto vZero = _mm256_setzero_si256();
	const auto vScale = _mm256_set1_ps(fScale);
	const auto vOffset = _mm256_set1_epi8(128u);

	__m256i vs32_00_07, vs32_08_15, vs32_16_23, vs32_24_31;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;
	
		const auto x = _mm256_sub_epi8(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(pInBuffer + i)), vOffset);

		//	Sign extend by unpacking to the high word of each element, then shifting right
		const auto vs16_00_15 = _mm256_unpacklo_epi8(vZero, x);
		const auto vs16_16_31 = _mm256_unpackhi_epi8(vZero, x);

		vs32_00_07 = _mm256_unpacklo_epi16(vZero, vs16_00_15);
		vs32_08_15 = _mm256_unpackhi_epi16(vZero, vs16_00_15);
		vs32_16_23 = _mm256_unpacklo_epi16(vZero, vs16_16_31);
		vs32_24_31 = _mm256_unpackhi_epi16(vZero, vs16_16_31);
		vs32_00_07 = _mm256_srai_epi32(vs32_00_07, 24);
		vs32_08_15 = _mm256_srai_epi32(vs32_08_15, 24);
		vs32_16_23 = _mm256_srai_epi32(vs32_16_23, 24);
		vs32_24_31 = _mm256_srai_epi32(vs32_24_31, 24);

		//	a and b will be out of order because AVX can't cross 128 bit lane boundaries
		const auto vf32_0_3 = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(vs32_00_07, vs32_08_15, 0 | (2 << 4)));
		const auto vf32_4_7 = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(vs32_16_23, vs32_24_31, 0 | (2 << 4)));
		const auto vf32_8_B = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(vs32_00_07, vs32_08_15, 1 | (3 << 4)));
		const auto vf32_C_F = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(vs32_16_23, vs32_24_31, 1 | (3 << 4)));

		const auto vOut0_3 = _mm256_mul_ps(vf32_0_3, vScale);
		const auto vOut4_7 = _mm256_mul_ps(vf32_4_7, vScale);
		const auto vOut8_B = _mm256_mul_ps(vf32_8_B, vScale);
		const auto vOutC_F = _mm256_mul_ps(vf32_C_F, vScale);

		_mm256_storeu_ps(pDst + 0x00, vOut0_3);
		_mm256_storeu_ps(pDst + 0x08, vOut4_7);
		_mm256_storeu_ps(pDst + 0x10, vOut8_B);
		_mm256_storeu_ps(pDst + 0x18, vOutC_F);
	}

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (auto i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		const auto ff32 = ss8 * fScale;
		pOutBuffer[i] = ff32;
	}

	//	Avoid AVX to SSE switching penalties
#if !( defined(__AVX__) || defined(FFTL_ASSUME_AVX) )
	_mm256_zeroupper();
#endif
}

void DspPcmConvert_SIMD8::CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2));
	
	constexpr f32 fScale = 1.f / 32768;
	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;
	const auto vZero = _mm256_setzero_si256();
	const auto vScale = _mm256_set1_ps(fScale);

	__m256i a, b;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;

		const auto x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pInBuffer + i));

		//	Sign extend by unpacking to the high word of each element, then shifting right
		a = _mm256_unpacklo_epi16(vZero, x);
		b = _mm256_unpackhi_epi16(vZero, x);
		a = _mm256_srai_epi32(a, 16);
		b = _mm256_srai_epi32(b, 16);

		//	a and b will be out of order because AVX can't cross 128 bit lane boundaries
		const auto af = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(a, b, 0 | (2 << 4)));
		const auto bf = _mm256_cvtepi32_ps(_mm256_permute2f128_si256(a, b, 1 | (3 << 4)));

		const auto vOut0_3 = _mm256_mul_ps(af, vScale);
		const auto vOut4_7 = _mm256_mul_ps(bf, vScale);

		_mm256_storeu_ps(pDst + 0, vOut0_3);
		_mm256_storeu_ps(pDst + 8, vOut4_7);
	}

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (auto i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i] * fScale;
	}

	//	Avoid AVX to SSE switching penalties
#if !( defined(__AVX__) || defined(FFTL_ASSUME_AVX) )
	_mm256_zeroupper();
#endif
}

//	Don't use this. It's slow. Fixme.
void DspPcmConvert_SIMD8::CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2));

	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 23));
	const auto vScale = _mm256_set1_ps(fScale);

	auto fnDo16Samples = [vScale](f32* pOutBuffer, const s24* pInBuffer)
	{
		const auto vScaleSSE = _mm256_castps256_ps128(vScale);
		const auto mask = _mm_setr_epi8(-1, 0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11);

		const auto* pSrc = reinterpret_cast<const __m128i*>(pInBuffer);
		auto* pDst = pOutBuffer;

		const auto sa = _mm_loadu_si128(pSrc + 0);
		const auto sb = _mm_loadu_si128(pSrc + 1);
		const auto sc = _mm_loadu_si128(pSrc + 2);

		const auto da = _mm_srai_epi32(_mm_shuffle_epi8(sa, mask), 8);
		const auto db = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sb, sa, 12), mask), 8);
		const auto dc = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sb,  8), mask), 8);
		const auto dd = _mm_srai_epi32(_mm_shuffle_epi8(_mm_alignr_epi8(sc, sc,  4), mask), 8);

		//	Convert to float
		const auto vOut00_03 = _mm_mul_ps(_mm_cvtepi32_ps(da), vScaleSSE);
		const auto vOut04_07 = _mm_mul_ps(_mm_cvtepi32_ps(db), vScaleSSE);
		const auto vOut08_11 = _mm_mul_ps(_mm_cvtepi32_ps(dc), vScaleSSE);
		const auto vOut12_15 = _mm_mul_ps(_mm_cvtepi32_ps(dd), vScaleSSE);

		//	Store
		_mm_storeu_ps(pDst +  0, vOut00_03);
		_mm_storeu_ps(pDst +  4, vOut04_07);
		_mm_storeu_ps(pDst +  8, vOut08_11);
		_mm_storeu_ps(pDst + 12, vOut12_15);
	};

	//	First 16 samples SSE style
	fnDo16Samples(pOutBuffer, pInBuffer);

	//	Next samples do AVX, where each load will discard 4 bytes at the start and end of each load
	constexpr size_t step = 16;
	const size_t vecSampleCount = ((totalSampleCount / step) * step) - 16;
	{
		const auto mask = _mm256_setr_epi8(-1, 4, 5, 6, -1, 7, 8, 9, -1, 10, 11, 12, -1, 13, 14, 15, -1, 16, 17, 18, -1, 19, 20, 21, -1, 22, 23, 24, -1, 25, 26, 27);
		for (size_t i = 16; i < vecSampleCount; i += step)
		{
			const byte* pByteBuffer = reinterpret_cast<const byte*>(pInBuffer + i);
			auto* pDst = pOutBuffer + i;

			const auto vs24_00_07 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pByteBuffer -  4));
			const auto vs24_07_15 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pByteBuffer - 24));

			const auto vs32_00_07 = _mm256_srai_epi32(_mm256_shuffle_epi8(vs24_00_07, mask), 8);
			const auto vs32_07_15 = _mm256_srai_epi32(_mm256_shuffle_epi8(vs24_07_15, mask), 8);

			//	Convert to float
			const auto vf32_00_07 = _mm256_mul_ps(_mm256_cvtepi32_ps(vs32_00_07), vScale);
			const auto vf32_07_15 = _mm256_mul_ps(_mm256_cvtepi32_ps(vs32_07_15), vScale);

			//	Store
			_mm256_storeu_ps(pDst + 0, vf32_00_07);
			_mm256_storeu_ps(pDst + 8, vf32_07_15);
		}
	}

	//	Last 16 samples SSE style
	fnDo16Samples(pOutBuffer + vecSampleCount, pInBuffer + vecSampleCount);

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = (static_cast<s32>(pInBuffer[i])) * fScale;
	}

	//	Avoid AVX to SSE switching penalties
#if !( defined(__AVX__) || defined(FFTL_ASSUME_AVX) )
	_mm256_zeroupper();
#endif
}

void DspPcmConvert_SIMD8::IntCvt(s32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2));

	constexpr size_t step = 32;
	const size_t vecSampleCount = (totalSampleCount / step) * step;
	const auto vZero = _mm256_setzero_si256();
	const auto vOffset = _mm256_set1_epi8(128u);

	__m256i vs32_00_07, vs32_08_15, vs32_16_23, vs32_24_31;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;
	
		const auto x = _mm256_sub_epi8(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(pInBuffer + i)), vOffset);

		//	Sign extend by unpacking to the high word of each element, then shifting right
		const auto vs16_00_15 = _mm256_unpacklo_epi8(vZero, x);
		const auto vs16_16_31 = _mm256_unpackhi_epi8(vZero, x);

		vs32_00_07 = _mm256_unpacklo_epi16(vZero, vs16_00_15);
		vs32_08_15 = _mm256_unpackhi_epi16(vZero, vs16_00_15);
		vs32_16_23 = _mm256_unpacklo_epi16(vZero, vs16_16_31);
		vs32_24_31 = _mm256_unpackhi_epi16(vZero, vs16_16_31);
		vs32_00_07 = _mm256_srai_epi32(vs32_00_07, 24);
		vs32_08_15 = _mm256_srai_epi32(vs32_08_15, 24);
		vs32_16_23 = _mm256_srai_epi32(vs32_16_23, 24);
		vs32_24_31 = _mm256_srai_epi32(vs32_24_31, 24);

		//	a and b will be out of order because AVX can't cross 128 bit lane boundaries
		const auto vf32_0_3 = _mm256_permute2f128_si256(vs32_00_07, vs32_08_15, 0 | (2 << 4));
		const auto vf32_4_7 = _mm256_permute2f128_si256(vs32_16_23, vs32_24_31, 0 | (2 << 4));
		const auto vf32_8_B = _mm256_permute2f128_si256(vs32_00_07, vs32_08_15, 1 | (3 << 4));
		const auto vf32_C_F = _mm256_permute2f128_si256(vs32_16_23, vs32_24_31, 1 | (3 << 4));

		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 0x00), vf32_0_3);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 0x08), vf32_4_7);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 0x10), vf32_8_B);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 0x18), vf32_C_F);
	}

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		pOutBuffer[i] = ss8;
	}

	//	Avoid AVX to SSE switching penalties
#if !( defined(__AVX__) || defined(FFTL_ASSUME_AVX) )
	_mm256_zeroupper();
#endif
}

void DspPcmConvert_SIMD8::IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	FFTL_ASSERT(CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2));
	
	constexpr size_t step = 16;
	const size_t vecSampleCount = (totalSampleCount / step) * step;
	const auto vZero = _mm256_setzero_si256();

	__m256i a, b;

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		auto* pDst = pOutBuffer + i;

		const auto x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pInBuffer + i));

		//	Sign extend by unpacking to the high word of each element, then shifting right
		a = _mm256_unpacklo_epi16(vZero, x);
		b = _mm256_unpackhi_epi16(vZero, x);
		a = _mm256_srai_epi32(a, 16);
		b = _mm256_srai_epi32(b, 16);

		//	a and b will be out of order because AVX can't cross 128 bit lane boundaries
		const auto vOut0_3 = _mm256_permute2f128_si256(a, b, 0 | (2 << 4));
		const auto vOut4_7 = _mm256_permute2f128_si256(a, b, 1 | (3 << 4));

		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 0), vOut0_3);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(pDst + 8), vOut4_7);
	}

	//	Finish remaining samples non-vectorized
	FFTL_LOOP_VECTORIZE_DISABLE
	for (size_t i = vecSampleCount; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i];
	}

	//	Avoid AVX to SSE switching penalties
#if !( defined(__AVX__) || defined(FFTL_ASSUME_AVX) )
	_mm256_zeroupper();
#endif
}


}


#endif // defined(__AVX2__) || defined(FFTL_AVX2) || ( defined(FFTL_SSE) && defined(FFTL_PLATFORM_WINDOWS) )
