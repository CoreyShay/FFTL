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

#if __has_include("../../_pch_Core.h")
	#include "_pch_Core.h"
#endif

#include "../defs.h"

#include "DspPcmConvert.h"
#include "../Math/MathCommon.h"


namespace FFTL
{


void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = 1.f / 128;

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		const auto ff32 = ss8 * fScale;
		pOutBuffer[i] = ff32;
	}
}

void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = 1.f / 32768;

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i] * fScale;
	}
}

void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1<<23));

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = (static_cast<s32>(pInBuffer[i])) * fScale;
	}
}

void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1u << 31u));

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<f32>(pInBuffer[i]) * fScale;
	}
}

void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount)
{
	constexpr f64 fScale = 1.0 / (1ull << 63ull);

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const f64 f = static_cast<f64>(pInBuffer[i]) * fScale;
		pOutBuffer[i] = static_cast<f32>(f);
	}
}

void DspPcmConvert_Default::CvtPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<f32>(pInBuffer[i]);
	}
}

void DspPcmConvert_Default::CvtPcm(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1u << 7u);

	const f32 fMin = -128.f;
	const f32 fMax = +127.f;

	for (size_t i = 0; i < totalSampleCount; i += 1)
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

void DspPcmConvert_Default::CvtPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	constexpr f32 fScale = static_cast<f32>(1u << 15u);
	constexpr f32 fMin = -32768.f;
	constexpr f32 fMax = +32767.f;

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const auto* pSrc = pInBuffer + i;
		auto* pDst = pOutBuffer + i;

		f32 const src = *pSrc;
		f32 scaled = src * fScale;
		scaled = (scaled > 0) ? scaled + 0.5f : scaled - 0.5f; // push for rounding before truncation.
		scaled = Clamp(scaled, fMin, fMax);
		*pDst = static_cast<s16>(scaled);
	}
}

void DspPcmConvert_Default::CvtPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	//	Note: Clamps the output from -2147483648 to +2147483520, which leaves 127 samples unused at the positive end.
	// To use the full range, double precision must be used.

	constexpr f32 fScale = static_cast<f32>( 1u << 31u );
	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = 2147483520.f; // 0x4EFFFFFF

	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		f32 fOut = pInBuffer[i];
		fOut *= fScale;
		fOut = Clamp(fOut, fMin, fMax);
		s32 nOut = static_cast<s32>(fOut);
		pOutBuffer[i] = nOut;
	}
}

void DspPcmConvert_Default::CvtPcm(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
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

void DspPcmConvert_Default::CvtPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
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

void DspPcmConvert_Default::CvtPcm(s16* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const u8 uu8 = pInBuffer[i];
		const s8 ss8 = static_cast<s8>((uu8 - 128u));
		const s16 ss16 = static_cast<s16>(ss8) << 8;
		pOutBuffer[i] = ss16;
	}
}

u32 DspPcmConvert_Default::CvtPcmDither(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 16)); //	16 bit reduction 24 -> 8
	constexpr f32 fMin = -128;
	constexpr f32 fMax = +127;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	for (size_t i = 0; i < totalSampleCount; i += 1)
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

u32 DspPcmConvert_Default::CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1.0 / (1 << 8)); //	8 bit reduction 24 -> 16
	constexpr f32 fMin = -32768;
	constexpr f32 fMax = +32767;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	for (size_t i = 0; i < totalSampleCount; i += 1)
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

u32 DspPcmConvert_Default::CvtPcmDither(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1 << 7);
	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	//	Finish remaining samples non-vectorized
	for (size_t i = 0; i < totalSampleCount; i += 1)
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

u32 DspPcmConvert_Default::CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	constexpr f32 fScale = static_cast<f32>(1 << 15);
	constexpr f32 fMin = -fScale;
	constexpr f32 fMax = fScale - 1;

	alignas(16) u32 seed[4] = { ditherSeed + 0, ditherSeed + 1, ditherSeed + 2, ditherSeed + 3 };

	//	Finish remaining samples non-vectorized
	for (size_t i = 0; i < totalSampleCount; i += 1)
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

void DspPcmConvert_Default::IntCvt(s32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		const auto uu8 = pInBuffer[i];
		const auto ss8 = static_cast<s8>((uu8 - 128u));
		pOutBuffer[i] = ss8;
	}
}

void DspPcmConvert_Default::IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = pInBuffer[i];
	}
}

void DspPcmConvert_Default::IntCvt(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i += 1)
	{
		pOutBuffer[i] = static_cast<s32>(pInBuffer[i]);
	}
}

void DspPcmConvert_Default::DeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i++)
	{
		const auto* pSrc = pInBuffer + i * 2;

		pOutBuffer0[i] = pSrc[0];
		pOutBuffer1[i] = pSrc[1];
	}
}

void DspPcmConvert_Default::DeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount)
{
	for (size_t i = 0; i < totalSampleCount; i++)
	{
		const auto* pSrc = pInBuffer + i * 4;

		pOutBuffer0[i] = pSrc[0];
		pOutBuffer1[i] = pSrc[1];
		pOutBuffer2[i] = pSrc[2];
		pOutBuffer3[i] = pSrc[3];
	}
}


}
