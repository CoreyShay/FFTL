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

#include "../defs.h"

#include "ComplexNumber.h"
#include "MathCommon.h"
#include "../Containers/Array.h"

#define FFTL_STAGE_TIMERS 0


#if defined(FFTL_ENABLE_PROFILING)
#	include "../Platform/Timer.h"
#endif

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4324) // structure was padded due to alignment specifier
#endif


namespace FFTL
{

enum class FFT_TwiddleType
{
	COMPLEX_REAL,
	COMPLEX_IMAG,
	REAL_REAL, // As in real FFT, real number component (cosine)
	REAL_IMAG
};

//	MSVC freaks out if arrays bigger than a certain size are computed using constexpr
#if defined(_MSC_VER)
#	if _MSC_VER < 1920
constexpr uint FFT_MAX_TWIDDLES_CONSTEXPR = 10;
constexpr uint FFT_MAX_BITREVERSE_CONSTEXPR = 11;
#	else
constexpr uint FFT_MAX_TWIDDLES_CONSTEXPR = 11;
constexpr uint FFT_MAX_BITREVERSE_CONSTEXPR = 12;
#	endif
#else
constexpr uint FFT_MAX_TWIDDLES_CONSTEXPR = 12;
constexpr uint FFT_MAX_BITREVERSE_CONSTEXPR = 13;
#endif

template <FFT_TwiddleType TWIDDLE_TYPE, uint M, typename T>
class FFTL_NODISCARD FFT_TwiddlesContainer
{
private:
	template <FFT_TwiddleType, uint, typename> friend class FFT_TwiddlesContainer;
	template <uint, typename, typename> friend class FFT_Base;
	template <uint, typename, typename> friend class FFT;
	template <uint, typename, typename> friend class FFT_Real_Base;
	template <uint, typename, typename> friend class FFT_Real;

	template <uint, typename, bool> friend class FFT_Twiddles;

	//	Precomputed constants
	static constexpr uint N = 1 << M;
	static constexpr uint ALIGNVAL = alignof(T) * (N >= 8 ? 8 : N >= 4 ? 4 : 1);

	constexpr FFT_TwiddlesContainer();

	alignas(ALIGNVAL) FixedArray<T, N> m_Twiddles;
};

template <uint M, typename T, bool USE_CONSTEXPR = (M <= FFT_MAX_TWIDDLES_CONSTEXPR)>
class FFTL_NODISCARD FFT_Twiddles
{
private:
	template <uint, typename, typename> friend class FFT_Base;
	template <uint, typename, typename> friend class FFT;
	template <uint, typename, typename> friend class FFT_Real_Base;
	template <uint, typename, typename> friend class FFT_Real;

	static constexpr uint N = 1 << (M);

	static constexpr const FixedArray<T, N>& GetCplxR() { return sm_TwiddlesR.m_Twiddles; }
	static constexpr const FixedArray<T, N>& GetCplxI() { return sm_TwiddlesI.m_Twiddles; }

	static constexpr const FixedArray<T, N>& GetRealR() { return sm_RealTwiddlesR.m_Twiddles; }
	static constexpr const FixedArray<T, N>& GetRealI() { return sm_RealTwiddlesI.m_Twiddles; }

	//	Twiddle factors are always split into 2 arrays for real and imaginary to keep consistent with the highly vectorized 1d transforms.
	static constexpr FFT_TwiddlesContainer<FFT_TwiddleType::COMPLEX_REAL, M, T> sm_TwiddlesR{ };
	static constexpr FFT_TwiddlesContainer<FFT_TwiddleType::COMPLEX_IMAG, M, T> sm_TwiddlesI{ };
	static constexpr FFT_TwiddlesContainer<FFT_TwiddleType::REAL_REAL, M, T> sm_RealTwiddlesR{ };
	static constexpr FFT_TwiddlesContainer<FFT_TwiddleType::REAL_IMAG, M, T> sm_RealTwiddlesI{ };
};

template <uint M, typename T>
class FFTL_NODISCARD FFT_Twiddles<M, T, false>
{
private:
	template <uint, typename, typename> friend class FFT_Base;
	template <uint, typename, typename> friend class FFT;
	template <uint, typename, typename> friend class FFT_Real_Base;
	template <uint, typename, typename> friend class FFT_Real;

	static constexpr uint N = Pow2<M>();

	static const FixedArray<T, N>& GetCplxR() { return sm_TwiddlesR.m_Twiddles; }
	static const FixedArray<T, N>& GetCplxI() { return sm_TwiddlesI.m_Twiddles; }

	static const FixedArray<T, N>& GetRealR() { return sm_RealTwiddlesR.m_Twiddles; }
	static const FixedArray<T, N>& GetRealI() { return sm_RealTwiddlesI.m_Twiddles; }

	//	Twiddle factors are always split into 2 arrays for real and imaginary to keep consistent with the highly vectorized 1d transforms.
	static const inline FFT_TwiddlesContainer<FFT_TwiddleType::COMPLEX_REAL, M, T> sm_TwiddlesR{ };
	static const inline FFT_TwiddlesContainer<FFT_TwiddleType::COMPLEX_IMAG, M, T> sm_TwiddlesI{ };
	static const inline FFT_TwiddlesContainer<FFT_TwiddleType::REAL_REAL, M, T> sm_RealTwiddlesR{ };
	static const inline FFT_TwiddlesContainer<FFT_TwiddleType::REAL_IMAG, M, T> sm_RealTwiddlesI{ };
};

template <uint M, typename T_BR>
class FFTL_NODISCARD FFT_BitreverseContainer
{
private:
	template <uint, bool> friend class FFT_Bitreversal;

	static constexpr uint N = 1 << M;

	constexpr FFT_BitreverseContainer();

	FixedArray<T_BR, N> m_;
};

template <uint M, bool USE_CONSTEXPR = (M <= FFT_MAX_BITREVERSE_CONSTEXPR)>
class FFTL_NODISCARD FFT_Bitreversal
{
private:
	template <uint, typename, typename> friend class FFT_Base;
	template <uint, typename, typename> friend class FFT;
	template <uint, typename, typename> friend class FFT_Real_Base;
	template <uint, typename, typename> friend class FFT_Real;

	static constexpr uint N = 1 << M;
	using T_BR = typename std::conditional<M <= 8, u8, typename std::conditional<M <= 16, u16, u32>::type>::type; // Ensures T_BR is the smallest possible unsigned integer type

	constexpr FFT_Bitreversal() = delete;

	static constexpr FFTL_FORCEINLINE const FixedArray<T_BR, N>& Get() { return m_.m_; }

	static constexpr FFT_BitreverseContainer<M, T_BR> m_{ };
};

template <uint M>
class FFTL_NODISCARD FFT_Bitreversal<M, false>
{
private:
	template <uint, typename, typename> friend class FFT_Base;
	template <uint, typename, typename> friend class FFT;
	template <uint, typename, typename> friend class FFT_Real_Base;
	template <uint, typename, typename> friend class FFT_Real;

	static constexpr uint N = 1 << M;
	using T_BR = typename std::conditional<M <= 8, u8, typename std::conditional<M <= 16, u16, u32>::type>::type; // Ensures T_BR is the smallest possible unsigned integer type

	constexpr FFT_Bitreversal() = delete;

	static FFTL_FORCEINLINE const FixedArray<T_BR, N>& Get() { return m_.m_; }

	static const inline FFT_BitreverseContainer<M, T_BR> m_{ };
};


enum enFftWindowType
{
	kWindowRectangular,
	kWindowTriangular,
	kWindowHanning,
	kWindowHamming,
	kWindowBlackman,
	kWindowVorbis,

	kFftNumWindowTypes
};


template <uint M, typename T, typename T_Twiddle = T>
class FFTL_NODISCARD FFT_Base
{
public:

	using cxT = cxNumber<T>;
	using cxTInternal = cxNumber<T_Twiddle>;
	using T_BR = typename FFT_Bitreversal<M>::T_BR;

	//	Precomputed constants
	static constexpr uint N = 1 << M;
	static constexpr uint N_2 = N >> 1;

	class WindowCoefficients
	{
	public:
		WindowCoefficients(enFftWindowType windowType, uint uWindowWidth = N) { Compute(windowType, uWindowWidth); }
		void Compute(enFftWindowType windowType, uint uWidth = N);

		FixedArray<T_Twiddle, N> m_C;
	};

	FFT_Base() = delete;

	static void TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput);
	static void TransformForward(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void TransformForward_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI); // 2nd half of cxInput is assumed to be all zeros
	static void TransformForwardApplyWindow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput, const WindowCoefficients& coeff);

	static void TransformInverse(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput, bool bApplyBitReverse = true);
	static void TransformInverse(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) { TransformForward(fInI, fInR, fOutI, fOutR); }

	//	In-place, no bit reversal or windowing
	static void TransformForward_InPlace_DIF(FixedArray<cxT, N>& cxInOut);
	static void TransformForward_InPlace_DIF(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI);

	//	In-place, no bit reversal or windowing. No divide by N.
	static void TransformInverse_InPlace_DIT(FixedArray<cxT, N>& cxInOut);
	static void TransformInverse_InPlace_DIT(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI);

	static void ApplyWindow(FixedArray<T, N>& fInOut, const WindowCoefficients& coeff);
	static void ApplyWindow(FixedArray<cxT, N>& cxInOut, const WindowCoefficients& coeff);

	static void TransformForward_Slow(const FixedArray<T, N>& fInput, FixedArray<cxT, N>& cxOutput);
	static void TransformForward_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput);
	static void TransformInverse_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutTime);
	static void TransformInverse_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput);

	static void ApplyBitReverseAndInterleave(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N * 2>& fOut);

	static void PrintSetupInfo();
	void PrintTimerInfo(uint iterationCount = 1) const;

	FFTL_NODISCARD FFTL_FORCEINLINE static constexpr const FixedArray<T_BR, N>& GetBitReverseIndices() { return FFT_Bitreversal<M>::Get(); }
	FFTL_NODISCARD FFTL_FORCEINLINE static constexpr T_BR GetBitReverseIndex(uint n) { return FFT_Bitreversal<M>::Get()[n]; }

protected:

	static void CalculateButterfly_DIT(const cxT& cxU, cxT* pcxCurr, cxT* pcxNext);
	static void CalculateButterfly_DIF(const cxT& cxU, cxT* pcxCurr, cxT* pcxNext);
	static void CalculateButterfly_Unity(cxT* pcxCurr, cxT* pcxNext);
	static void CalculateButterfly_DIT(const T& fUReal, const T& fUImag, T* pfCurReal, T* pfCurImag, T* pfNextReal, T* pfNextImag);
	static void CalculateButterfly_DIF(const T& fUReal, const T& fUImag, T* pfCurReal, T* pfCurImag, T* pfNextReal, T* pfNextImag);
	static void CalculateButterfly_Unity(T* pfCurReal, T* pfCurImag, T* pfNextReal, T* pfNextImag);
	static void CalculateButterfly_Unity(const T& fCurR, const T& fCurI, const T& fNextR, const T& fNextI, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI);

	template <uint STAGE_CURRENT, uint STAGE_BEGIN, bool INVERSE> static void Transform_Main_DIT(FixedArray<cxT, N>& cxOutput);
	template <uint STAGE_CURRENT, uint STAGE_BEGIN, bool INVERSE> static void Transform_Main_DIF(FixedArray<cxT, N>& cxOutput);
	template <uint STAGE_CURRENT, uint STAGE_BEGIN> static void Transform_Main_DIT(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	template <uint STAGE_CURRENT, uint STAGE_BEGIN> static void Transform_Main_DIF(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);

#if FFTL_STAGE_TIMERS
public:
	mutable FixedArray<u64, M> m_StageTimers;
	mutable u64 m_PreProcessTimer;
	mutable u64 m_PostProcessTimer;
#endif
};

//	fft_Single class performs FFT's without attempting to vectorize using SIMD instructions,
// however, if T is a SIMD type with the appropriate operators overloaded, you basically get
// that for free, eg, 4 channels at once if T is of type f32_4.
template <uint M, typename T, typename T_Twiddle = T>
class FFTL_NODISCARD FFT : public FFT_Base<M, T, T_Twiddle>
{
};

#if FFTL_SIMD_F32x4
//	fft_SIMD class performs single channel float FFT's only, but uses SIMD instructions
// if the compiler supports them, usually speeding up the process considerably.
template <uint M>
class FFT<M, f32, f32> : public FFT_Base<M, f32, f32>
{
public:
	using T = f32;
	using T_Twiddle = T;
	using cxT = cxNumber<T>;
	using FFT_Base<M, T, T_Twiddle>::N;
	using FFT_Base<M, T, T_Twiddle>::N_2;
	using T_BR = typename FFT_Base<M, T, T_Twiddle>::T_BR;
	using FFT_Base<M, T, T_Twiddle>::GetBitReverseIndex;

#if FFTL_STAGE_TIMERS
	using FFT_Base<M, T, T_Twiddle>::m_StageTimers;
	using FFT_Base<M, T, T_Twiddle>::m_PreProcessTimer;
	using FFT_Base<M, T, T_Twiddle>::m_PostProcessTimer;
#endif

	FFT() = delete;

	//	Transforms that perform bit reversal, out of place.
	static void TransformForward(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void TransformForward_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI); // 2nd half of cxInput is assumed to be all zero
	static void TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput) { FFT_Base<M, T, T_Twiddle>::TransformForward(cxInput, cxOutput); }// TODO, optimize
	static void TransformInverse(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);

	//	Forward transform outputs in bit reversed order. Inverse transform assumes input in bit-reversed order, outputs in normal order.
	static void TransformForward_InPlace_DIF(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI);
	static void TransformInverse_InPlace_DIT(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI);
	static void TransformForward_InPlace_DIF(FixedArray<cxT, N>& cxInOut) { FFT_Base<M, f32, f32>::TransformForward_InPlace_DIF(cxInOut); }// TODO, optimize
	static void TransformForward_InPlace_DIT(FixedArray<cxT, N>& cxInOut) { FFT_Base<M, f32, f32>::TransformForward_InPlace_DIT(cxInOut); }// TODO, optimize

	static void ApplyBitReverseAndInterleave(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N * 2>& fOut);

protected:
	static void Transform_Stage0_BR(const FixedArray<T, N>& fInReal, const FixedArray<T, N>& fInImag, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void Transform_Stage0_BR(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	static void Transform_Stage0_BR_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI); // 2nd half of cxInput is assumed to be all zero

	template <uint STAGE_CURRENT, uint STAGE_BEGIN> static void Transform_Main_DIT(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);
	template <uint STAGE_CURRENT, uint STAGE_BEGIN> static void Transform_Main_DIF(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI);

	static void Calculate4Butterflies_DIT_Stage0(T* pfReal, T* pfImag);
	static void Calculate4Butterflies_DIF_Stage0(T* pfReal, T* pfImag);
	static void Calculate4Butterflies_DIT_Stage0(f32_4_In vCurR, f32_4_In vNextR, f32_4_In vCurI, f32_4_In vNextI, T* pfReal, T* pfImag);
	static void Calculate4Butterflies_DIT_Stage1(f32_4_In vUR, f32_4_In vUI, T* pfReal, T* pfImag);
	static void Calculate4Butterflies_DIF_Stage1(f32_4_In vUR, f32_4_In vUI, T* pfReal, T* pfImag);

	template <typename V> static void CalculateVButterflies_DIT(const V& vUR, const V& vUI, T* pfCurReal, T* pfCurImag, T* pfNextReal, T* pfNextImag);
	template <typename V> static void CalculateVButterflies_DIF(const V& vUR, const V& vUI, T* pfCurReal, T* pfCurImag, T* pfNextReal, T* pfNextImag);
};
#endif


template <uint M, typename T, typename T_Twiddle = T>
class FFTL_NODISCARD FFT_Real_Base
{
public:

	using cxT = cxNumber<T>;
	using cxTInternal = cxNumber<T_Twiddle>;
	using sm_fft = FFT<M - 1, T, T_Twiddle>;

	//	Precomputed constants
	static constexpr uint N = 1 << M;
	static constexpr uint N_2 = N >> 1;
	static constexpr uint N_4 = N >> 2;

	FFT_Real_Base() = delete;

	static void TransformForward(const FixedArray<T, N>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI);
	static void TransformForward_1stHalf(const FixedArray<T, N_2>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI); // 2nd half of fTimeIn is assumed to be all zeros
	static void TransformInverse(const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut);
	static void TransformInverse_ClobberInput(FixedArray<T, N_2>& fFreqInR, FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut);

	FFTL_NODISCARD FFTL_FORCEINLINE static const T_Twiddle& GetTwiddleReal(uint n) { return FFT_Twiddles<M - 2, T_Twiddle>::GetRealR()[n]; }
	FFTL_NODISCARD FFTL_FORCEINLINE static const T_Twiddle& GetTwiddleImag(uint n) { return FFT_Twiddles<M - 2, T_Twiddle>::GetRealI()[n]; }
	FFTL_NODISCARD FFTL_FORCEINLINE static const T_Twiddle* GetTwiddleRealPtr(uint n) { return FFT_Twiddles<M - 2, T_Twiddle>::GetRealR() + n; }
	FFTL_NODISCARD FFTL_FORCEINLINE static const T_Twiddle* GetTwiddleImagPtr(uint n) { return FFT_Twiddles<M - 2, T_Twiddle>::GetRealI() + n; }
};


template <uint M, typename T, typename T_Twiddle = T>
class FFT_Real : public FFT_Real_Base<M, T, T_Twiddle>
{
public:
	FFT_Real() = delete;
};

#if FFTL_SIMD_F32x4
template <uint M>
class FFTL_NODISCARD FFT_Real<M, f32, f32> : public FFT_Real_Base<M, f32, f32>
{
public:
	constexpr FFT_Real<M, f32, f32>() = delete;

	using T = f32;
	using T_Twiddle = T;
	using cxT = cxNumber<T>;
	using sm_fft = FFT<M - 1, T, T_Twiddle>;

	//	Usings from base class
	using FFT_Real_Base<M, T, T_Twiddle>::N;
	using FFT_Real_Base<M, T, T_Twiddle>::N_2;
	using FFT_Real_Base<M, T, T_Twiddle>::N_4;
	using FFT_Real_Base<M, T, T_Twiddle>::GetTwiddleReal;
	using FFT_Real_Base<M, T, T_Twiddle>::GetTwiddleImag;
	using FFT_Real_Base<M, T, T_Twiddle>::GetTwiddleRealPtr;
	using FFT_Real_Base<M, T, T_Twiddle>::GetTwiddleImagPtr;

	static void TransformForward(const FixedArray<T, N>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI);
	static void TransformForward_1stHalf(const FixedArray<T, N_2>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI); // 2nd half of fTimeIn is assumed to be all zeros
	static void TransformInverse(const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut);
	static void TransformInverse_ClobberInput(FixedArray<T, N_2>& fFreqInR, FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut);

private:
	static void PostProcessForward(FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI);
	static void PreProcessInverse(FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI, const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI);
};
#endif

class FFTL_NODISCARD FFT_RealV_Base
{
public:
	virtual void TransformForward(const f32* fTimeIn, f32* fFreqOutR, f32* fFreqOutI) const = 0;
	virtual void TransformInverse(const f32* fFreqInR, const f32* fFreqInI, f32* fTimeOut) const = 0;
	virtual void TransformInverse_ClobberInput(f32* fFreqInR, f32* fFreqInI, f32* fTimeOut) const = 0;
};

//	Use this class if you need an FFT where the usage M is only known at runtime,
// eg, use FFT_RealV_Base
template <uint M>
class FFTL_NODISCARD FFT_RealV : public FFT_RealV_Base, public FFT_Real<M, f32>
{
public:
	//	Precomputed constants
	static constexpr uint N = 1 << M;
	static constexpr uint N_2 = N >> 1;
	static constexpr uint N_4 = N >> 2;

	void TransformForward(const f32* fTimeIn, f32* fFreqOutR, f32* fFreqOutI) const override;
	void TransformInverse(const f32* fFreqInR, const f32* fFreqInI, f32* fTimeOut) const override;
	void TransformInverse_ClobberInput(f32* fFreqInR, f32* fFreqInI, f32* fTimeOut) const override;
};

//	This convolver class only processes real data, using a real FFT of size 2N, which itself
// utilizes a complex FFT of size N.
template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle = T>
class FFTL_NODISCARD Convolver
{
public:
	using cxT = cxNumber<T>;
	static constexpr uint N = 1 << M;
	static constexpr uint N_2 = N >> 1;
	static constexpr uint _2N = N << 1;
	static constexpr size_t SingleKernelSize_Bytes() { return sizeof(Kernel); }

	struct Kernel
	{
		FixedArray_Aligned32<f32, _2N> t;

		const FixedArray_Aligned32<f32, N>& r() const { return *reinterpret_cast<const FixedArray_Aligned32<f32, N>*>(t + 0); }
		const FixedArray_Aligned32<f32, N>& i() const { return *reinterpret_cast<const FixedArray_Aligned32<f32, N>*>(t + N); }

		FixedArray_Aligned32<f32, N>& r() { return *reinterpret_cast<FixedArray_Aligned32<f32, N>*>(t + 0); }
		FixedArray_Aligned32<f32, N>& i() { return *reinterpret_cast<FixedArray_Aligned32<f32, N>*>(t + N); }
	};

	Convolver();
	~Convolver() = default;

	void Convolve(FixedArray_Aligned32<T, N>& fInOutput, const Kernel* pKernelArray_FD, size_t kernelArraySize);
	void Convolve(FixedArray_Aligned32<T, N>& fInOutput, const Kernel* pKernelArrayA_FD, size_t kernelArraySizeA, T fGainA, const Kernel* pKernelArrayB_FD, size_t kernelArraySizeB, T fGainB);

	//	Only performs FFT, convolution, and IFFT necessary to fill the fInOutout buffer with the data needed right now.
	void ConvolveInitial_FirstStage(const FixedArray_Aligned32<T, N>& fInput, const Kernel* pKernelArray_FD, size_t kernelArraySize);
	void ConvolveInitial_FirstStage(const FixedArray_Aligned32<T, N>& fInput, const Kernel* pKernelArrayA_FD, size_t kernelArraySizeA, T fGainA, const Kernel* pKernelArrayB_FD, size_t kernelArraySizeB, T fGainB);
	void ConvolveInitial_LastStage(FixedArray_Aligned32<T, N>& fOutput);

	//	Resumes convolution started via ConvolveInitial.
	void ConvolveResumePartial(const Kernel* pKernelArray_FD, size_t kernelArraySize, size_t startKernelIndex, size_t endKernelIndex);
	void ConvolveResumePartial(const Kernel* pKernelArrayA_FD, size_t kernelArraySizeA, T fGainA, const Kernel* pKernelArrayB_FD, size_t kernelArraySizeB, T fGainB, size_t startKernelIndex, size_t endKernelIndex);

	FFTL_NODISCARD size_t GetLeftoverKernels() const { return m_LeftoverKernelCount; }

	static uint InitKernel(Kernel* pKernelOutput_FD, const T* pKernelInput_TD, size_t kernelLength);

	//	Our FFT computer
	typedef FFT_Real<M + 1, T, T_Twiddle> sm_fft;

protected:
	static void ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY); //	output = inX * inY
	static void ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inW); //	output = inX * inY + inW
	static void ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inW, T fGainY); //	output = inX * (inY * fGainY) + inW
	static void ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inZ, const Kernel& inW, T fGainY, T fGainZ); //	output = inX * (inY * fGainY + inZ * fGainZ) + inW
	static void AddArrays(FixedArray_Aligned32<T, N>& output, const FixedArray_Aligned32<T, N>& inA, const FixedArray_Aligned32<T, N>& inB);

	FixedArray_Aligned32<Kernel, T_MAX_KERNELS> m_AccumulationBuffer;
	FixedArray_Aligned32<T, N> m_PrevTail;
	Kernel m_tempBufferA, m_tempBufferB;
	Kernel m_inputSignal_FD; // Frequency domain (Fourier transform) of the last input signal
	size_t m_LeftoverKernelCount = 0;
	bool m_bInputSignalHasData = false;
	bool m_bConvolutionFrameComplete = false;

#if defined(FFTL_ENABLE_PROFILING)
	Timer m_timerFftForward;
	Timer m_timerFftInverse;
	Timer m_timerConvolution;
#endif
};



template <typename T, uint T_N, uint T_KERNEL_LENGTH>
class FFTL_NODISCARD Convolver_Slow
{
public:

	Convolver_Slow() { MemZero(m_AccumulationBuffer); }

	void SetKernel(const FixedArray<T, T_KERNEL_LENGTH>& fKernel) { MemCopy(m_Kernel, fKernel); }
	void Convolve(const FixedArray<T, T_N>& fInput, FixedArray<T, T_N>& fOutput);

private:
	FixedArray<T, T_KERNEL_LENGTH> m_Kernel;
	FixedArray<T, T_N + T_KERNEL_LENGTH> m_AccumulationBuffer;
};


} // namespace FFTL


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#include "FFT.inl"
