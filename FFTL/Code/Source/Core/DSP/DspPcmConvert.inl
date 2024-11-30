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

#include "../Platform/CpuInfo.h"


namespace FFTL
{

class DspPcmConvert_Default
{
protected:
	friend void DspConvertPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount);

	friend void DspConvertPcm( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);

	friend void DspConvertPcm( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);

	friend u32 DspConvertPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

	friend void IntegerConvert(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	friend void IntegerConvert(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	friend void IntegerConvert(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

	friend void DspDeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspDeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount);



	static void CvtPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount);

	static void CvtPcm( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);

	static void CvtPcm( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);

	static u32 CvtPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

	static void IntCvt(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	static void IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	static void IntCvt(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

	static void DeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount);
	static void DeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount);
};

class DspPcmConvert_SIMD4 : public DspPcmConvert_Default
{
protected:
	friend void DspConvertPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount);

	friend void DspConvertPcm( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);

	friend void DspConvertPcm( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(s16* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);

	friend u32 DspConvertPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

	friend void IntegerConvert(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	friend void IntegerConvert(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	friend void IntegerConvert(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

	friend void DspDeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount);
	friend void DspDeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount);



	static void CvtPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount);
//	static void CvtPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount);
//	static void CvtPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount);

	static void CvtPcm( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);

	static void IntCvt(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	static void IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	static void IntCvt(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

	static void CvtPcm( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(s16* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);

	static u32 CvtPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

	static void DeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount);
	static void DeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount);
};

//	Note: These are defined on all platforms to allow compilation via if constexpr. Non-SSE platforms will fail to link if functions are actually called.
class DspPcmConvert_SSE4 : public DspPcmConvert_SIMD4
{
protected:
	friend u32 DspConvertPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	friend u32 DspConvertPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

	static u32 CvtPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
	static u32 CvtPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
};

//	Note: These are defined on all platforms to allow compilation via if constexpr. Non-SSE platforms will fail to link if functions are actually called.
class DspPcmConvert_SIMD8 : public DspPcmConvert_SIMD4
{
protected:
	friend void DspConvertPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	friend void DspConvertPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

	friend void IntegerConvert(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	friend void IntegerConvert(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);

	static void CvtPcm(f32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
	static void CvtPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);// Not ready for use yet

	static void IntCvt(s32* pOutBuffer, const u8*  pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
	static void IntCvt(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
};

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4702) // warning C4702: unreachable code
#endif

inline void DspConvertPcm(f32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x8(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2))
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x8(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2))
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	//	24 bit conversion performs sub-optimally using the implemented AVX2 function.
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x8(); supported != CpuInfo::Supported::NO && false)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2))
		{
			DspPcmConvert_SIMD8::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount)
{
	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount)
{
	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE3(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE3))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspConvertPcm(s16* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
}

inline u32 DspConvertPcmDither(u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
#if defined(FFTL_ARM_NEON)
	return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
#else
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE4))
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE3(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE3))
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
#endif
}

inline u32 DspConvertPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
#if defined(FFTL_ARM_NEON)
	return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
#else
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE4))
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE3(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE3))
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}

	DspPcmConvert_Default::CvtPcm(pOutBuffer, pInBuffer, totalSampleCount);
#endif
}

inline u32 DspConvertPcmDither(u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE4))
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}

	return DspPcmConvert_Default::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
}

inline u32 DspConvertPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE4))
		{
			return DspPcmConvert_SSE4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			return DspPcmConvert_SIMD4::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
		}
	}

	return DspPcmConvert_Default::CvtPcmDither(pOutBuffer, pInBuffer, totalSampleCount, ditherSeed);
}

inline void IntegerConvert(s32* pOutBuffer, const u8* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x8(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD8::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2))
		{
			DspPcmConvert_SIMD8::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void IntegerConvert(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x8(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD8::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::AVX2))
		{
			DspPcmConvert_SIMD8::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_I32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE2))
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void IntegerConvert(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SSE3(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE3))
		{
			DspPcmConvert_SIMD4::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
			return;
		}
	}
	
	DspPcmConvert_Default::IntCvt(pOutBuffer, pInBuffer, totalSampleCount);
}

inline void DspDeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_F32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::DeInterleave2(pOutBuffer0, pOutBuffer1, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE))
		{
			DspPcmConvert_SIMD4::DeInterleave2(pOutBuffer0, pOutBuffer1, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::DeInterleave2(pOutBuffer0, pOutBuffer1, pInBuffer, totalSampleCount);
}

inline void DspDeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount)
{
	if constexpr (constexpr auto supported = CpuInfo::GetSupports_SIMD_F32x4(); supported != CpuInfo::Supported::NO)
	{
		if constexpr (supported == CpuInfo::Supported::YES)
		{
			DspPcmConvert_SIMD4::DeInterleave4(pOutBuffer0, pOutBuffer1, pOutBuffer2, pOutBuffer3, pInBuffer, totalSampleCount);
			return;
		}
		else if (CpuInfo::GetIsArchitectureEnabled(CpuInfo::Architecture::SSE))
		{
			DspPcmConvert_SIMD4::DeInterleave4(pOutBuffer0, pOutBuffer1, pOutBuffer2, pOutBuffer3, pInBuffer, totalSampleCount);
			return;
		}
	}

	DspPcmConvert_Default::DeInterleave4(pOutBuffer0, pOutBuffer1, pOutBuffer2, pOutBuffer3, pInBuffer, totalSampleCount);
}

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

}
