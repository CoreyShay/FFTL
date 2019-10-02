/*

Original author:
Corey Shay
cshay892@gmail.com

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

#ifndef _DFT_H
#define _DFT_H

#include "defs.h"
#include "ComplexNumber.h"
#include "Math.h"
#include "Containers/Array.h"

#define FFTL_STAGE_TIMERS 0

#if 0
#include <Windows.h>
inline void FFTL_Printf(const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	char _buf[256];
	vsprintf_s(_buf, format, arglist);
	OutputDebugStringA(_buf);
	va_end(arglist);
}
#else
#include <stdio.h>
#define FFTL_Printf printf
#endif

#if FFTL_STAGE_TIMERS
#include <Windows.h>
#endif




namespace FFTL
{

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
class FFT_Base
{
public:
	typedef u16 T_BR; // can be made larger if size greater than 65536 is needed.

	typedef cxNumber<T> cxT;
	typedef cxNumber<T_Twiddle> cxTInternal;

	//	Precomputed constants
	static const uint N = 1<<M;
	static const uint N_div_2 = N>>1;

	class WindowCoefficients
	{
	public:
		WindowCoefficients(enFftWindowType windowType, uint uWindowWidth = N) { Compute(windowType, uWindowWidth); }
		void Compute(enFftWindowType windowType, uint uWidth = N);

		FixedArray<T_Twiddle,N> m_C;
	};

	FFT_Base();

	void TransformForward(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput) const;
	void TransformForward(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void TransformForward(const FixedArray<cxT,N>& cxInput, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void TransformForwardApplyWindow(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput, const WindowCoefficients& coeff) const;

	void TransformInverse(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput, bool bApplyBitReverse=true) const;
	void TransformInverse(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const { TransformForward(fInI, fInR, fOutI, fOutR); }

	//	In-place, no bit reversal or windowing
	void TransformForward_InPlace_DIF(FixedArray<cxT,N>& cxInOut) const;
	void TransformForward_InPlace_DIF(FixedArray<T,N>& fInOutR, FixedArray<T,N>& fInOutI) const;

	//	In-place, no bit reversal or windowing. No divide by N.
	void TransformInverse_InPlace_DIT(FixedArray<cxT,N>& cxInOut) const;
	void TransformInverse_InPlace_DIT(FixedArray<T,N>& fInOutR, FixedArray<T,N>& fInOutI) const;

	static void ApplyWindow(FixedArray<T,N>& fInOut, const WindowCoefficients& coeff);
	static void ApplyWindow(FixedArray<cxT,N>& cxInOut, const WindowCoefficients& coeff);

	static void TransformForward_Slow(const FixedArray<T,N>& fInput, FixedArray<cxT,N>& cxOutput);
	static void TransformForward_Slow(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput);
	static void TransformInverse_Slow(const FixedArray<cxT,N>& cxInput, FixedArray<T,N>& fOutTime);
	static void TransformInverse_Slow(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput);

	void ApplyBitReverseAndInterleave(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N*2>& fOut) const;

	void PrintSetupInfo() const;
	void PrintTimerInfo(uint iterationCount = 1) const;

	FFTL_FORCEINLINE const T_Twiddle& GetTwiddleReal(uint n) const { return m_TwiddleFactorsR[n]; }
	FFTL_FORCEINLINE const T_Twiddle* GetTwiddleRealPtr(uint n) const { return m_TwiddleFactorsR + n; }
	FFTL_FORCEINLINE const T_Twiddle& GetTwiddleImag(uint n) const { return m_TwiddleFactorsI[n]; }
	FFTL_FORCEINLINE const T_Twiddle* GetTwiddleImagPtr(uint n) const { return m_TwiddleFactorsI + n; }

	FFTL_FORCEINLINE const FixedArray<T_BR,N>& GetBitReverseIndices() const { return m_BitReverseIndices; }
	FFTL_FORCEINLINE T_BR GetBitReverseIndex(uint n) const { return m_BitReverseIndices[n]; }

protected:

	static void CalculateButterfly_DIT(const cxT& cxU, cxT* __restrict pcxCurr, cxT* __restrict pcxNext);
	static void CalculateButterfly_DIF(const cxT& cxU, cxT* __restrict pcxCurr, cxT* __restrict pcxNext);
	static void CalculateButterfly_Unity(cxT* __restrict pcxCurr, cxT* __restrict pcxNext);
	static void CalculateButterfly_DIT(const T& fUReal, const T& fUImag, T* __restrict pfCurReal, T* __restrict pfCurImag, T* __restrict pfNextReal, T* __restrict pfNextImag);
	static void CalculateButterfly_DIF(const T& fUReal, const T& fUImag, T* __restrict pfCurReal, T* __restrict pfCurImag, T* __restrict pfNextReal, T* __restrict pfNextImag);
	static void CalculateButterfly_Unity(T* __restrict pfCurReal, T* __restrict pfCurImag, T* __restrict pfNextReal, T* __restrict pfNextImag);
	static void CalculateButterfly_Unity(const T& fCurR, const T& fCurI, const T& fNextR, const T& fNextI, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI);

	void ComputeBitReversalIndices();
	void ComputeTwiddleFactors();

	void Transform_Main_DIT(FixedArray<cxT,N>& cxOutput, bool bSkipStage0 = false) const;
	void Transform_Main_DIF(FixedArray<cxT,N>& cxOutput) const;
	void Transform_Main_DIT(FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI, bool bSkipStage0 = false) const;
	void Transform_Main_DIF(FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;

	//	Twiddle factors are always split into 2 arrays for real and imaginary to keep consistent with the highly vectorized 1d transforms.
	FixedArray_Aligned32<T_Twiddle,N> m_TwiddleFactorsR;
	FixedArray_Aligned32<T_Twiddle,N> m_TwiddleFactorsI;

	FixedArray<T_BR,N> m_BitReverseIndices;

#if FFTL_STAGE_TIMERS
public:
	mutable LARGE_INTEGER m_StageTimers[M];
	mutable LARGE_INTEGER m_PreProcessTimer;
	mutable LARGE_INTEGER m_PostProcessTimer;
#endif
};

//	fft_Single class performs FFT's without attempting to vectorize using SIMD instructions,
// however, if T is a SIMD type with the appropriate operators overloaded, you basically get
// that for free, eg, 4 channels at once if T is of type f32_4.
template <uint M, typename T, typename T_Twiddle = T>
class FFT : public FFT_Base<M, T, T_Twiddle>
{
public:
};

#if FFTL_SIMD4
//	fft_SIMD class performs single channel float FFT's only, but uses SIMD instructions
// if the compiler supports them, usually speeding up the process considerably.
template <uint M>
class FFT<M, f32, f32> : public FFT_Base<M, f32, f32>
{
public:
	typedef f32 T;
	typedef T T_Twiddle;
	typedef cxNumber<T> cxT;
	using FFT_Base<M, T, T_Twiddle>::N;
	using FFT_Base<M, T, T_Twiddle>::N_div_2;
	using FFT_Base<M, T, T_Twiddle>::m_BitReverseIndices;
	typedef typename FFT_Base<M, T, T_Twiddle>::T_BR T_BR;

	FFT();

	//	Transforms that perform bit reversal, out of place.
	void TransformForward(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void TransformForward(const FixedArray<cxT,N>& cxInput, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void TransformForward(const FixedArray<cxT,N>& cxInput, FixedArray<cxT,N>& cxOutput) const { FFT_Base<M,T,T_Twiddle>::TransformForward(cxInput, cxOutput); }// TODO, optimize
	void TransformInverse(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;

	//	Forward transform outputs in bit reversed order. Inverse transform assumes input in bit-reversed order, outputs in normal order.
	void TransformForward_InPlace_DIF(FixedArray<T,N>& fInOutR, FixedArray<T,N>& fInOutI) const;
	void TransformInverse_InPlace_DIT(FixedArray<T,N>& fInOutR, FixedArray<T,N>& fInOutI) const;
	void TransformForward_InPlace_DIF(FixedArray<cxT,N>& cxInOut) const { FFT_Base<M, f32, f32>::TransformForward_InPlace_DIF(cxInOut); }// TODO, optimize
	void TransformForward_InPlace_DIT(FixedArray<cxT,N>& cxInOut) const { FFT_Base<M, f32, f32>::TransformForward_InPlace_DIT(cxInOut); }// TODO, optimize

	void ApplyBitReverseAndInterleave(const FixedArray<T,N>& fInR, const FixedArray<T,N>& fInI, FixedArray<T,N*2>& fOut) const;

protected:
	void Transform_Stage0(FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void Transform_Stage0_BR(const FixedArray<T,N>& fInReal, const FixedArray<T,N>& fInImag, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void Transform_Stage0_BR(const FixedArray<cxT,N>& cxInput, FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;
	void Transform_Main_DIT(FixedArray<T,N>& fOutR, FixedArray<T,N>& fOutI) const;

	static void Calculate4Butterflies_DIT_Stage0(T* __restrict pfReal, T* __restrict pfImag);
	static void Calculate4Butterflies_DIF_Stage0(T* __restrict pfReal, T* __restrict pfImag);
	static void Calculate4Butterflies_DIT_Stage0(f32_4Arg vCurR, f32_4Arg vNextR, f32_4Arg vCurI, f32_4Arg vNextI, T* __restrict pfReal, T* __restrict pfImag);
	static void Calculate4Butterflies_DIT_Stage1(f32_4Arg vUR, f32_4Arg vUI, T* __restrict pfReal, T* __restrict pfImag);
	static void Calculate4Butterflies_DIF_Stage1(f32_4Arg vUR, f32_4Arg vUI, T* __restrict pfReal, T* __restrict pfImag);

	template <typename V> static void CalculateVButterflies_DIT(const V& vUR, const V& vUI, T* __restrict pfCurReal, T* __restrict pfCurImag, T* __restrict pfNextReal, T* __restrict pfNextImag);
	template <typename V> static void CalculateVButterflies_DIF(const V& vUR, const V& vUI, T* __restrict pfCurReal, T* __restrict pfCurImag, T* __restrict pfNextReal, T* __restrict pfNextImag);
};
#endif


template <uint M, typename T, typename T_Twiddle = T>
class FFT_Real_Base
{
public:

	typedef cxNumber<T> cxT;
	typedef cxNumber<T_Twiddle> cxTInternal;

	//	Precomputed constants
	static const uint N = 1<<M;
	static const uint N_2 = N>>1;
	static const uint N_4 = N>>2;

	FFT_Real_Base();

	void TransformForward(const FixedArray<T,N>& fTimeIn, FixedArray<T,N_2>& fFreqOutR, FixedArray<T,N_2>& fFreqOutI) const;
	void TransformInverse(const FixedArray<T,N_2>& fFreqInR, const FixedArray<T,N_2>& fFreqInI, FixedArray<T,N>& fTimeOut) const;
	void TransformInverse_ClobberInput(FixedArray<T,N_2>& fFreqInR, FixedArray<T,N_2>& fFreqInI, FixedArray<T,N>& fTimeOut) const;

	FFTL_FORCEINLINE const T_Twiddle& GetTwiddleReal(uint n) const { return m_PostTwiddlesR[n]; }
	FFTL_FORCEINLINE const T_Twiddle* GetTwiddleRealPtr(uint n) const { return m_PostTwiddlesR + n; }
	FFTL_FORCEINLINE const T_Twiddle& GetTwiddleImag(uint n) const { return m_PostTwiddlesI[n]; }
	FFTL_FORCEINLINE const T_Twiddle* GetTwiddleImagPtr(uint n) const { return m_PostTwiddlesI + n; }

	void PrintTimerInfo(uint iterationCount = 1) const { m_fft.PrintTimerInfo(iterationCount); }

public:
	FFT<M-1, T, T_Twiddle> m_fft;
private:
	FixedArray_Aligned32<T_Twiddle,N_4> m_PostTwiddlesR;
	FixedArray_Aligned32<T_Twiddle,N_4> m_PostTwiddlesI;
};


template <uint M, typename T, typename T_Twiddle = T>
class FFT_Real : public FFT_Real_Base<M, T, T_Twiddle>
{
public:
};

#if FFTL_SIMD4
template <uint M>
class FFT_Real<M, f32, f32> : public FFT_Real_Base<M, f32, f32>
{
public:
	typedef f32 T;
	typedef cxNumber<T> cxT;

	//	Precomputed constants
	static const uint N = 1<<M;
	static const uint N_2 = N>>1;
	static const uint N_4 = N>>2;

//	FFT_Real();

	void TransformForward(const FixedArray<T,N>& fTimeIn, FixedArray<T,N_2>& fFreqOutR, FixedArray<T,N_2>& fFreqOutI) const;
	void TransformInverse(const FixedArray<T,N_2>& fFreqInR, const FixedArray<T,N_2>& fFreqInI, FixedArray<T,N>& fTimeOut) const;
	void TransformInverse_ClobberInput(FixedArray<T,N_2>& fFreqInR, FixedArray<T,N_2>& fFreqInI, FixedArray<T,N>& fTimeOut) const;
};
#endif



//	This convolver class only processes real data, effectively more than doubling its speed,
// rather than zeroing out imaginary data. This way it only needs to use a complex FFT the same
// size as its input, rather than a double size one. In this way, no data is thrown away either.
template <uint M, uint T_MAX_KERNELS, typename T, typename T_Twiddle = T>
class Convolver
{
public:
	typedef cxNumber<T> cxT;
	static const uint N = 1<<M;
	static const uint _2N = N<<1;

	Convolver();
	~Convolver();

	void InitKernel(const void* pKernelInput, size_t byteSize);
	void Convolve(FixedArray<T,N>& fInOutput);
	void Destroy();

private:
	//	Our FFT computer
	static const FFT<M, T, T_Twiddle> sm_FFT;

	struct Kernel
	{
		FixedArray<cxT,N> c;
		FixedArray<cxT,N>& AsComplex() { return c; }
		FixedArray<T,_2N>& AsScalar() { return *reinterpret_cast<FixedArray<T,_2N>*>(&c); }
	};

	FixedArray<Kernel, T_MAX_KERNELS> m_KernelArray_FD;
	FixedArray<T, T_MAX_KERNELS*N> m_AccumulationBuffer;
	uint m_KernelCount;
};

template <typename T, uint T_N, uint T_KERNEL_LENGTH>
class Convolver_Slow
{
public:

	Convolver_Slow() { memset(&m_AccumulationBuffer, 0, sizeof(m_AccumulationBuffer)); }

	void SetKernel(const FixedArray<T, T_KERNEL_LENGTH>& fKernel) { memcpy(&m_Kernel, &fKernel, sizeof(m_Kernel)); }
	void Convolve(const FixedArray<T,T_N>& fInput, FixedArray<T,T_N>& fOutput);

private:
	FixedArray<T, T_KERNEL_LENGTH> m_Kernel;
	FixedArray<T, T_N+T_KERNEL_LENGTH> m_AccumulationBuffer;
};

} // namespace FFTL



#include "fft.inl"


#endif // _DFT_H
