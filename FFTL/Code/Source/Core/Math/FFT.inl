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

#ifndef _FFTL_FFT_INL
#define _FFTL_FFT_INL

#include "../Platform/Log.h"
#include "../Utils/Casts.h"
#include "../Utils/StdFunctions.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4127 ) //conditional expression is constant
#endif

namespace FFTL
{


/*
All the below implementations of the FFT were originally inspired from a bit of BASIC code written by
Steven W. Smith in his book "The Scientist and Engineer's Guide to Digital Signal Processing", and
then heavily modified for speed especially on SSE+ platforms.
http://www.dspguide.com/ch12/3.htm


1000 'THE FAST FOURIER TRANSFORM
1010 'Upon entry, N% contains the number of points in the DFT, REX[ ] and
1020 'IMX[ ] contain the real and imaginary parts of the input. Upon return,
1030 'REX[ ] and IMX[ ] contain the DFT output. All signals run from 0 to N%-1.
1040 '
1050 PI = 3.14159265 'Set constants
1060 NM1% = N%-1
1070 ND2% = N%/2
1080 M% = CINT(LOG(N%)/LOG(2))
1090 J% = ND2%
1100 '
1110 FOR I% = 1 TO N%-2 'Bit reversal sorting
1120 	IF I% >= J% THEN GOTO 1190
1130 	TR = REX[J%]
1140 	TI = IMX[J%]
1150 	REX[J%] = REX[I%]
1160 	IMX[J%] = IMX[I%]
1170 	REX[I%] = TR
1180 	IMX[I%] = TI
1190 	K% = ND2%
1200 	IF K% > J% THEN GOTO 1240
1210 	J% = J%-K%
1220 	K% = K%/2
1230 	GOTO 1200
1240 	J% = J%+K%
1250 NEXT I%
1260 `
1270 FOR L% = 1 TO M% 'Loop for each stage
1280 	LE% = CINT(2^L%)
1290 	LE2% = LE%/2
1300 	UR = 1
1310 	UI = 0
1320 	SR = COS(PI/LE2%) 'Calculate sine & cosine values
1330 	SI = -SIN(PI/LE2%)
1340 	FOR J% = 1 TO LE2% 'Loop for each sub DFT
1350 		JM1% = J%-1
1360 		FOR I% = JM1% TO NM1% STEP LE% 'Loop for each butterfly
1370 			IP% = I%+LE2%
1380 			TR = REX[IP%]*UR - IMX[IP%]*UI 'Butterfly calculation
1390 			TI = REX[IP%]*UI + IMX[IP%]*UR
1400 			REX[IP%] = REX[I%]-TR
1410 			IMX[IP%] = IMX[I%]-TI
1420 			REX[I%] = REX[I%]+TR
1430 			IMX[I%] = IMX[I%]+TI
1440 		NEXT I%
1450 		TR = UR
1460 		UR = TR*SR - UI*SI
1470 		UI = TR*SI + UI*SR
1480 	NEXT J%
1490 NEXT L%
*/




template <FFT_TwiddleType TWIDDLE_TYPE, uint M, typename T>
constexpr FFT_TwiddlesContainer<TWIDDLE_TYPE, M, T>::FFT_TwiddlesContainer()
{
	const auto nsize = m_Twiddles.size();

	switch (TWIDDLE_TYPE)
	{
	case FFT_TwiddleType::COMPLEX_REAL:
		for (uint n = 0; n < nsize; ++n)
		{
			const long double fAngle = -PI_<long double> * static_cast<long double>(n) / static_cast<long double>(N);
			m_Twiddles[n] = static_cast<T>(math_constexpr::Cos(fAngle));
		}
		break;
	case FFT_TwiddleType::COMPLEX_IMAG:
		for (uint n = 0; n < nsize; ++n)
		{
			const long double fAngle = -PI_<long double> * static_cast<long double>(n) / static_cast<long double>(N);
			m_Twiddles[n] = static_cast<T>(math_constexpr::Sin(fAngle));
		}
		break;
	case FFT_TwiddleType::REAL_REAL:
		for (uint n = 0; n < nsize; ++n)
		{
			const long double fAngle = PI_<long double> * static_cast<long double>(n) / static_cast<long double>(2 * N);
			m_Twiddles[n] = static_cast<T>(-math_constexpr::Sin(fAngle));
		}
		break;
	case FFT_TwiddleType::REAL_IMAG:
		for (uint n = 0; n < nsize; ++n)
		{
			const long double fAngle = PI_<long double> * static_cast<long double>(n) / static_cast<long double>(2 * N);
			m_Twiddles[n] = static_cast<T>(-math_constexpr::Cos(fAngle));
		}
		break;
	}
}

template <uint M, typename T_BR>
constexpr FFT_BitreverseContainer<M, T_BR>::FFT_BitreverseContainer()
{
	//	Pre-compute bit-reveral indices
	for (uint i = 0; i < N; ++i)
	{
		const T_BR n = safestatic_cast<T_BR>(i);
		T_BR uReverseIndex = n;
		for (T_BR uRightIndex = 0, uLeftIndex = M - 1; uRightIndex < uLeftIndex; ++uRightIndex, --uLeftIndex)
		{
			const uint uShiftAmount = uLeftIndex - uRightIndex;
			uReverseIndex &= ~((1 << uRightIndex) | (1 << uLeftIndex));

			T_BR bitRight = (1 << uRightIndex) & n;
			T_BR bitLeft = (1 << uLeftIndex) & n;
			bitRight <<= uShiftAmount;
			bitLeft >>= uShiftAmount;
			uReverseIndex |= bitLeft | bitRight;
		}

		m_[n] = uReverseIndex;
	}
}


template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::ApplyWindow(FixedArray<T, N>& fInOut, const WindowCoefficients& coeff)
{
	for (uint n = 0; n < N; ++n)
		fInOut[n] *= coeff.m_C[n];
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::ApplyWindow(FixedArray<cxT, N>& cxInOut, const WindowCoefficients& coeff)
{
	for (uint n = 0; n < N; ++n)
		cxInOut[n] *= coeff.m_C[n];
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_DIT(const cxT& cxU, cxT* pcxCurr, cxT* pcxNext)
{
	const cxT W = *pcxNext * cxU;
	*pcxNext = *pcxCurr - W;
	*pcxCurr += W;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_Unity(cxT* pcxCurr, cxT* pcxNext)
{
	const cxT W = *pcxNext;
	*pcxNext = *pcxCurr - W;
	*pcxCurr += W;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_DIF(const cxT& cxU, cxT* pcxCurr, cxT* pcxNext)
{
	const cxT W = *pcxCurr - *pcxNext;
	*pcxCurr += *pcxNext;
	*pcxNext = W * cxU;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_DIT(const T& fUReal, const T& fUImag, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	const T Wr = *pfNextR*fUReal - *pfNextI*fUImag;
	const T Wi = *pfNextR*fUImag + *pfNextI*fUReal;

	*pfNextR = *pfCurR - Wr;
	*pfNextI = *pfCurI - Wi;

	*pfCurR += Wr;
	*pfCurI += Wi;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_Unity(T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	const T Wr = *pfNextR;
	const T Wi = *pfNextI;

	*pfNextR = *pfCurR - Wr;
	*pfNextI = *pfCurI - Wi;

	*pfCurR += Wr;
	*pfCurI += Wi;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_Unity(const T& fCurR, const T& fCurI, const T& fNextR, const T& fNextI, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	*pfCurR = fCurR + fNextR;
	*pfCurI = fCurI + fNextI;

	*pfNextR = fCurR - fNextR;
	*pfNextI = fCurI - fNextI;
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::CalculateButterfly_DIF(const T& fUReal, const T& fUImag, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	const T Wr = *pfCurR - *pfNextR;
	const T Wi = *pfCurI - *pfNextI;

	*pfCurR += *pfNextR;
	*pfCurI += *pfNextI;

	*pfNextR = Wr * fUReal - Wi * fUImag;
	*pfNextI = Wr * fUImag + Wi * fUReal;
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::PrintSetupInfo()
{
#if FFTL_PRINTSETUP
	const T_Twiddle _2Pi = (T_Twiddle)2 * PI_<T_Twiddle>;
	const T_Twiddle _1_div_N = (T_Twiddle)1 / (T_Twiddle)N;
	const T_Twiddle _2Pi_div_N = _2Pi * _1_div_N;
	const uint M = M;
	const uint N = N;

	FFTL_LOG_MSG("N = %u\n", N);
	FFTL_LOG_MSG("N_2 = %u\n", N_2);
	FFTL_LOG_MSG("M = %u\n", M);
	FFTL_LOG_MSG("twoPi = %f\n", _2Pi);
	FFTL_LOG_MSG("one_N = %f\n", _1_div_N);
	FFTL_LOG_MSG("twoPiOne_N = %f\n", _2Pi_div_N);

#if 0
	FFTL_LOG_MSG("M twiddles:\n");
	for (uint i = 0; i < _M; ++i)
	{
		FFTL_LOG_MSG("[%f, %f]; ", m_TwiddleFactors[i].r, m_TwiddleFactors[i].i);
	}

	FFTL_LOG_MSG("\nN twiddles:\n");
	//	Loop for each stage
	for (uint nStage = 0, nStageExp = 2; nStage < _M; ++nStage, nStageExp <<= 1)
	{
		const uint nStageExp_2 = nStageExp >> 1;

		//	Lookup the twiddle factor for phase angle
		const cxT& cxS = m_TwiddleFactors[nStage];
		cxT cxU(1, 0);

		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; ++nSubDFT)
		{
			FFTL_LOG_MSG("[%f, %f]; ", cxU.r, cxU.i);
			cxU *= cxS;
		}
		FFTL_LOG_MSG("\n\n");
	}
#endif
#endif // FFTL_PRINTSETUP
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::PrintTimerInfo(uint iterationCount) const
{
#if FFTL_STAGE_TIMERS

	if (m_PreProcessTimer != 0)
	{
		FFTL_LOG_MSG("\tPreProcess: %f us\n", Timer::ToMicroseconds(m_PreProcessTimer) / iterationCount);
		m_PreProcessTimer = 0;
	}

	for (int m = 0; m < M; ++m)
	{
		if (m_StageTimers[m] != 0)
		{
			FFTL_LOG_MSG("\tStage %i: %f us\n", m, Timer::ToMicroseconds(m_StageTimers[m]) / iterationCount);
			m_StageTimers[m] = 0;
		}
	}

	if (m_PostProcessTimer != 0)
	{
		FFTL_LOG_MSG("\tPostProcess: %f us\n", Timer::ToMicroseconds(m_PostProcessTimer) / iterationCount);
		m_PostProcessTimer = 0;
	}
#else
	(void)iterationCount;
#endif
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::WindowCoefficients::Compute(enFftWindowType windowType, uint uWidth)
{
	//	Zero out anything that might be lurking
	MemZero(m_C);

	//	Window function calculation
	switch (windowType)
	{
	default:
	case kWindowRectangular:
	{
		for (uint n = 0; n < uWidth; ++n)
			m_C[n] = (T_Twiddle)1;
	}
	break;
	case kWindowTriangular:
	{
		const uint uWidth_div_2 = uWidth >> 1;
		const T_Twiddle fWidth_div_2 = (T_Twiddle)uWidth_div_2;
		for (uint n = 0; n < uWidth_div_2; ++n)
			m_C[n] = m_C[uWidth - n - 1] = (T_Twiddle)n / (fWidth_div_2 - 1);
	}
	break;
	case kWindowHanning:
	{
		for (uint n = 0; n < uWidth; ++n)
			m_C[n] = T_Twiddle(0.50 - 0.50 * Cos(2.0 * PI_64 * n / (uWidth - 1)));
	}
	break;
	case kWindowHamming:
	{
		for (uint n = 0; n < uWidth; ++n)
			m_C[n] = T_Twiddle(0.54 - 0.46 * Cos(2.0 * PI_64 * n / (uWidth - 1)));
	}
	break;
	case kWindowBlackman:
	{
		for (uint n = 0; n < uWidth; ++n)
			m_C[n] = T_Twiddle(0.42 - 0.50 * Cos(2.0*PI_64*n / (uWidth - 1)) + 0.08 * Cos(4.0 * PI_64 * n / (uWidth - 1)));
	}
	break;
	case kWindowVorbis:
	{
		for (uint n = 0; n < uWidth; ++n)
			m_C[n] = T_Twiddle(Sin(0.5 * PI_64 * Square(Sin(PI_64 / (uWidth) * (n + 0.5)))));
	}
	break;
	}
}













template<typename T_TO, typename T_FROM>
FFTL_FORCEINLINE T_TO LoadTo(const T_FROM* a)
{
	return static_cast<T_TO>(*a);
}
template<>
FFTL_FORCEINLINE f32_4 LoadTo<f32_4, f32>(const f32* a)
{
	return f32_4::Splat(a);
}
template<>
FFTL_FORCEINLINE f32_8 LoadTo<f32_8, f32>(const f32* a)
{
	return f32_8::Splat(a);
}

template<typename T_TO, typename T_FROM>
constexpr FFTL_FORCEINLINE T_TO ConvertTo(const T_FROM& a)
{
	return static_cast<T_TO>(a);
}
template<>
FFTL_FORCEINLINE f32_4 ConvertTo<f32_4, f32>(const f32& a)
{
	return f32_4::Splat(a);
}
template<>
FFTL_FORCEINLINE f32_8 ConvertTo<f32_8, f32>(const f32& a)
{
	return f32_8::Splat(&a);
}








template <uint M, typename T, typename T_Twiddle>
template <uint STAGE_CURRENT, uint STAGE_BEGIN, bool INVERSE>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::Transform_Main_DIT(FixedArray<cxT, N>& cxOutput) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT > STAGE_BEGIN)
	{
		Transform_Main_DIT<STAGE_CURRENT - 1, STAGE_BEGIN, INVERSE>(cxOutput);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

	const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
	const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

	//	The first iteration of every stage will have a 1,0 twiddle factor, so we can save a few cycles
	for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
	{
		const uint uButterflyNext = uButterfly + nStageExp_2;

		T* pfCurR = &cxOutput[uButterfly].r;
		T* pfCurI = &cxOutput[uButterfly].i;
		T* pfNextR = &cxOutput[uButterflyNext].r;
		T* pfNextI = &cxOutput[uButterflyNext].i;

		FFTL_IF_CONSTEXPR (INVERSE)
			CalculateButterfly_Unity(pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
		else
			CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
	}

	//	Loop for each sub DFT. Note that we skip twiddle index 0 because it's the unity twiddle taken care of previously.
	for (uint nSubDFT = 1; nSubDFT < nStageExp_2; ++nSubDFT)
	{
		const T uR = LoadTo<T>(twiddlesReal + nSubDFT);
		const T uI = LoadTo<T>(twiddlesImag + nSubDFT);

		//	Loop for each butterfly
		for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;

			T* pfCurR = &cxOutput[uButterfly].r;
			T* pfCurI = &cxOutput[uButterfly].i;
			T* pfNextR = &cxOutput[uButterflyNext].r;
			T* pfNextI = &cxOutput[uButterflyNext].i;

			FFTL_IF_CONSTEXPR (INVERSE)
				CalculateButterfly_DIT(uR, uI, pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
			else
				CalculateButterfly_DIT(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}
}

template <uint M, typename T, typename T_Twiddle>
template <uint STAGE_CURRENT, uint STAGE_BEGIN, bool INVERSE>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::Transform_Main_DIF(FixedArray<cxT, N>& cxOutput) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT < STAGE_BEGIN)
	{
		Transform_Main_DIF<STAGE_CURRENT + 1, STAGE_BEGIN, INVERSE>(cxOutput);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

	const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
	const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

	//	Loop for each sub DFT
	for (int nSubDFT = nStageExp_2 - 1; nSubDFT >= 1; --nSubDFT)
	{
		const T uR = LoadTo<T>(twiddlesReal + nSubDFT);
		const T uI = LoadTo<T>(twiddlesImag + nSubDFT);

		//	Loop for each butterfly
		for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;

			T* pfCurR = &cxOutput[uButterfly].r;
			T* pfCurI = &cxOutput[uButterfly].i;
			T* pfNextR = &cxOutput[uButterflyNext].r;
			T* pfNextI = &cxOutput[uButterflyNext].i;
			
		FFTL_IF_CONSTEXPR (INVERSE)
			CalculateButterfly_DIF(uR, uI, pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
		else
			CalculateButterfly_DIF(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}

	//	The first iteration of every stage (or in this case, the last, going backwards) will have a 1,0 twiddle factor, so we can save a few cycles
	for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
	{
		const uint uButterflyNext = uButterfly + nStageExp_2;

		T* pfCurR = &cxOutput[uButterfly].r;
		T* pfCurI = &cxOutput[uButterfly].i;
		T* pfNextR = &cxOutput[uButterflyNext].r;
		T* pfNextI = &cxOutput[uButterflyNext].i;

		FFTL_IF_CONSTEXPR (INVERSE)
			CalculateButterfly_Unity(pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
		else
			CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
	}
}

template <uint M, typename T, typename T_Twiddle>
template <uint STAGE_CURRENT, uint STAGE_BEGIN>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::Transform_Main_DIT(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT > STAGE_BEGIN)
	{
		Transform_Main_DIT<STAGE_CURRENT - 1, STAGE_BEGIN>(fOutR, fOutI);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

	const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
	const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

	//	The first iteration of every stage will have a 1,0 twiddle factor, so we can save a few cycles
	for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
	{
		const uint uButterflyNext = uButterfly + nStageExp_2;
		T* pfCurR = fOutR + uButterfly;
		T* pfCurI = fOutI + uButterfly;
		T* pfNextR = fOutR + uButterflyNext;
		T* pfNextI = fOutI + uButterflyNext;
		CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
	}

	//	Loop for each sub DFT. Note that we skip twiddle index 0 because it's the unity twiddle taken care of previously.
	for (uint nSubDFT = 1; nSubDFT < nStageExp_2; ++nSubDFT)
	{
		const T uR = LoadTo<T>(twiddlesReal + nSubDFT);
		const T uI = LoadTo<T>(twiddlesImag + nSubDFT);

		//	Loop for each butterfly
		for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;
			T* pfCurR = fOutR + uButterfly;
			T* pfCurI = fOutI + uButterfly;
			T* pfNextR = fOutR + uButterflyNext;
			T* pfNextI = fOutI + uButterflyNext;
			CalculateButterfly_DIT(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}
}

template <uint M, typename T, typename T_Twiddle>
template <uint STAGE_CURRENT, uint STAGE_BEGIN>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::Transform_Main_DIF(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT < STAGE_BEGIN)
	{
		Transform_Main_DIF<STAGE_CURRENT + 1, STAGE_BEGIN>(fOutR, fOutI);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

	const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
	const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

	//	Loop for each sub DFT
	for (int nSubDFT = nStageExp_2 - 1; nSubDFT >= 1; --nSubDFT)
	{
		const T uR = LoadTo<T>(twiddlesReal + nSubDFT);
		const T uI = LoadTo<T>(twiddlesImag + nSubDFT);

		//	Loop for each butterfly
		for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;

			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateButterfly_DIF(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}

	//	The first iteration of every stage (or in this case, the last, going backwards) will have a 1,0 twiddle factor, so we can save a few cycles
	for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
	{
		const uint uButterflyNext = uButterfly + nStageExp_2;

		T* pfCurR = &fOutR[uButterfly];
		T* pfCurI = &fOutI[uButterfly];
		T* pfNextR = &fOutR[uButterflyNext];
		T* pfNextI = &fOutI[uButterflyNext];
		CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
	}
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformInverse_InPlace_DIT(FixedArray<cxT, N>& cxOutput)
{
	Transform_Main_DIT<M - 1, 0, true>(cxOutput);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::TransformInverse_InPlace_DIT(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI)
{
	Transform_Main_DIT<M - 1, 0>(fInOutI, fInOutR);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < N; n += 2)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);

		T* pfCurR = &cxOutput[n + 0].r;
		T* pfCurI = &cxOutput[n + 0].i;
		T* pfNextR = &cxOutput[n + 1].r;
		T* pfNextI = &cxOutput[n + 1].i;
		CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, cxInput[nR1].r, cxInput[nR1].i, pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_PreProcessTimer += timer.GetTicks();
#endif

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1, false>(cxOutput);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformForward(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < N; n += 2)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);

		T* pfCurR = fOutR + n;
		T* pfCurI = fOutI + n;
		T* pfNextR = fOutR + n + 1;
		T* pfNextI = fOutI + n + 1;
		CalculateButterfly_Unity(fInR[nR0], fInI[nR0], fInR[nR1], fInI[nR1], pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_PreProcessTimer += timer.GetTicks();
#endif

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < N; n += 2)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);

		T* pfCurR = fOutR + n;
		T* pfCurI = fOutI + n;
		T* pfNextR = fOutR + n + 1;
		T* pfNextI = fOutI + n + 1;
		CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, cxInput[nR1].r, cxInput[nR1].i, pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_PreProcessTimer += timer.GetTicks();
#endif

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformForward_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // 2nd half of cxInput is assumed to be all zero
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	const T zero = ConvertTo<T>(0);

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < N; n += 2)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
//		const uint nR1 = GetBitReverseIndex(n + 1);

		T* pfCurR = fOutR + n + 0;
		T* pfCurI = fOutI + n + 0;
		T* pfNextR = fOutR + n + 1;
		T* pfNextI = fOutI + n + 1;
//		CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, cxInput[nR1].r, cxInput[nR1].i, pfCurR, pfCurI, pfNextR, pfNextI);
		CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, zero, zero, pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_PreProcessTimer += timer.GetTicks();
#endif

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformForwardApplyWindow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput, const WindowCoefficients& coeff)
{
	//	Copy the input to the output with the bit reversal indices, and apply the window function
	for (uint n = 0; n < N; ++n)
	{
		const uint nR = GetBitReverseIndex(n);
		cxOutput[n] = cxInput[nR] * coeff.m_C[nR];
	}

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 0>(cxOutput);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::TransformInverse(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput, bool bApplyBitReverse)
{
//	const T _1_div_N = ConvertTo<T>(1 / (T_Twiddle)N);

	if (bApplyBitReverse)
	{
		//	Copy the input to the output with the bit reversal indices.
		for (uint n = 0; n < N; ++n)
		{
			const uint nR = GetBitReverseIndex(n);
			const cxT& rInput = cxInput[nR];
			cxOutput[n] = rInput;
		}
	}
	else
	{
		cxOutput = cxInput;
	}

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 0, true>(cxOutput);

	//	Divide everything by N and flip the sign of the imaginary output again
	for (uint n = 0; n < N; ++n)
	{
		cxT& rOutput = cxOutput[n];
#if 0 // Don't divide by N because this is typically used in convolution, and we can just do it there.
		rOutput.r *= +_1_div_N;
		rOutput.i *= -_1_div_N;
#else
		rOutput.i = -rOutput.i;
#endif
	}
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::TransformForward_InPlace_DIF(FixedArray<cxT, N>& cxInOut)
{
	Transform_Main_DIF<0, M - 1, false>(cxInOut);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<M, T, T_Twiddle>::TransformForward_InPlace_DIF(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI)
{
	Transform_Main_DIF<0, M - 1>(fInOutR, fInOutI);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<M, T, T_Twiddle>::ApplyBitReverseAndInterleave(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N * 2>& fOut)
{
	//	Restore the time domain real output as interleaved real and complex. We need to apply bit reversal here as well.
	for (uint n = 0; n < N; n += 4)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);
		const uint nR2 = GetBitReverseIndex(n + 2);
		const uint nR3 = GetBitReverseIndex(n + 3);

		//	Interleave the output while bit reversing.
		T* pSh = fOut + n * 2;

		const T fSh0 = fInR[nR0];
		const T fSh1 = fInI[nR0];
		const T fSh2 = fInR[nR1];
		const T fSh3 = fInI[nR1];
		const T fSh4 = fInR[nR2];
		const T fSh5 = fInI[nR2];
		const T fSh6 = fInR[nR3];
		const T fSh7 = fInI[nR3];

		pSh[0] = fSh0;
		pSh[1] = fSh1;
		pSh[2] = fSh2;
		pSh[3] = fSh3;
		pSh[4] = fSh4;
		pSh[5] = fSh5;
		pSh[6] = fSh6;
		pSh[7] = fSh7;
	}
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::TransformForward_Slow(const FixedArray<T, N>& fInput, FixedArray<cxT, N>& cxOutput)
{
	const T _2Pi = (T)2 * PI_<T>;
	const T _2Pi_div_N = _2Pi / (T)N;

	for (uint m = 0; m < N; ++m)
	{
		const T fm = (T)m;
		cxOutput[m] = 0;
		for (uint n = 0; n < N; ++n)
		{
			const T fAngle = _2Pi_div_N * (T)n * fm;
			const cxT cx(Cos(fAngle), -Sin(fAngle));
			cxOutput[m] += cx * fInput[n];
		}
	}
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::TransformForward_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput)
{
	const T _2Pi = (T)2 * PI_<T>;
	const T _2Pi_div_N = _2Pi / (T)N;

	for (uint m = 0; m < N; ++m)
	{
		const T fm = (T)m;
		cxOutput[m] = 0;
		for (uint n = 0; n < N; ++n)
		{
			const T fAngle = _2Pi_div_N * (T)n * fm;
			const cxT cx(Cos(fAngle), -Sin(fAngle));
			cxOutput[m] += cx * cxInput[n];
		}
	}
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::TransformInverse_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutTime)
{
	const T _2Pi = (T)2 * PI_<T>;
	const T _1_div_N = (T)1 / (T)N;
	const T _2Pi_div_N = _2Pi * _1_div_N;

	{
		T fTime = 0;
		for (uint m = 0; m < N; ++m)
		{
			fTime += cxInput[m].r;
		}
		fOutTime[0] = _1_div_N * fTime;
	}

	for (uint n = 1; n < N; ++n)
	{
		const T fn = (T)n;
		T fTime = 0;
		for (uint m = 0; m < N; ++m)
		{
			const T fm = (T)m;
			const T fAngle = _2Pi_div_N * fn * fm;
			fTime += (cxInput[m].r * Cos(fAngle)) - (cxInput[m].i * Sin(fAngle));
		}

		fOutTime[N - n] = _1_div_N * fTime;
	}
}

template <uint M, typename T, typename T_Twiddle>
void FFT_Base<M, T, T_Twiddle>::TransformInverse_Slow(const FixedArray<cxT, N>& cxInput, FixedArray<cxT, N>& cxOutput)
{
	const T _2Pi = (T)2 * PI_<T>;
	const T _1_div_N = (T)1 / (T)N;
	const T _2Pi_div_N = _2Pi * _1_div_N;

	{
		cxT cxn = 0;
		for (uint m = 0; m < N; ++m)
		{
			cxn += cxInput[m];
		}
		cxOutput[0] = cxn * _1_div_N;
	}

	for (uint n = 1; n < N; ++n)
	{
		const T fn = (T)n;
		cxT cxn = 0;
		for (uint m = 0; m < N; ++m)
		{
			const T fm = (T)m;
			const T fAngle = _2Pi_div_N * fn * fm;
			cxn += cxInput[m] * cxT(Cos(fAngle), Sin(fAngle));
		}

		cxOutput[N - n] = cxn * _1_div_N;
	}
}



#if FFTL_SIMD_F32x4

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::TransformForward(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	Transform_Stage0_BR(fInR, fInI, fOutR, fOutI);

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::TransformForward(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	Transform_Stage0_BR(cxInput, fOutR, fOutI);

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::TransformForward_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // 2nd half of cxInput is assumed to be all zero
{
	Transform_Stage0_BR_1stHalf(cxInput, fOutR, fOutI);

	//	Invoke the main transform function
	Transform_Main_DIT<M - 1, 1>(fOutR, fOutI);
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::TransformInverse(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	//	Swap the real and imaginary parts.
	TransformForward(fInI, fInR, fOutI, fOutR);

#if 0 // Don't divide by N because this is typically used in convolution, and we can just do it there.
	//	Divide everything by N.
	const T _1_div_N = (T)1 / (T)N;
	const f32_4 v1_div_N = { _1_div_N,_1_div_N,_1_div_N,_1_div_N };

	for (uint n = 0; n < N; n += 4)
	{
		T* pReal = &fOutR[n];
		f32_4 vReal = f32_4::LoadA(pReal);
		vReal = (vReal * v1_div_N);
		StoreA(pReal, vReal);

		T* pfI = &fOutI[n];
		f32_4 vImag = f32_4::LoadA(pfI);
		vImag = (vImag * v1_div_N);
		StoreA(pfI, vImag);
	}
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::TransformForward_InPlace_DIF(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	Transform_Main_DIF<0, M - 1>(fOutR, fOutI);
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::TransformInverse_InPlace_DIT(FixedArray<T, N>& fInOutR, FixedArray<T, N>& fInOutI)
{
	//	Invoke the main transform function
	//	Swap the real and imaginary parts.
	Transform_Main_DIT<M - 1, 0>(fInOutI, fInOutR);
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::Transform_Stage0_BR(const FixedArray<T, N>& fInReal, const FixedArray<T, N>& fInImag, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	//	Specialized SIMD case for stage 0 that requires XXZZYYWW shuffling
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Perform the first stage of the transform while copying the input to the output with bit reversal indices.
	for (uint n = 0; n < N; n += 8)
	{
		//	Loop for each 4 butterflies

#if defined(FFTL_AVX2) && 0 // This approach is actually slower. Don't use it.
		FFTL_IF_CONSTEXPR (sizeof(T_BR) == 2)
		{
			//	Shuffle the inputs around so that we can do 4 butterflies at once. Current is even, next is odd.
			const __m128i v16Indices = _mm_load_si128(reinterpret_cast<const __m128i*>(sm_BitReverseIndices + n));
			const __m128i v32Indices_0_3 = _mm_unpacklo_epi16(v16Indices, _mm_setzero_si128());
			const __m128i v32Indices_4_7 = _mm_unpackhi_epi16(v16Indices, _mm_setzero_si128());
			const __m128i v32Indices_Ev = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v32Indices_0_3), _mm_castsi128_ps(v32Indices_4_7), FFTL_MM_SHUFFLE_XYZW(0, 2, 0, 2)));
			const __m128i v32Indices_Od = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v32Indices_0_3), _mm_castsi128_ps(v32Indices_4_7), FFTL_MM_SHUFFLE_XYZW(1, 3, 1, 3)));

			const f32_4 vCurR = _mm_i32gather_ps(fInReal.data(), v32Indices_Ev, 4);
			const f32_4 vCurI = _mm_i32gather_ps(fInImag.data(), v32Indices_Ev, 4);

			const f32_4 vNextR = _mm_i32gather_ps(fInReal.data(), v32Indices_Od, 4);
			const f32_4 vNextI = _mm_i32gather_ps(fInImag.data(), v32Indices_Od, 4);

			//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
			// Also the input is already pre-shuffled.
			Calculate4Butterflies_DIT_Stage0(vCurR, vNextR, vCurI, vNextI, &fOutR[n], &fOutI[n]);
		}
		else
#endif
		{
			const uint nR0 = GetBitReverseIndex(n + 0);
			const uint nR1 = GetBitReverseIndex(n + 1);
			const uint nR2 = GetBitReverseIndex(n + 2);
			const uint nR3 = GetBitReverseIndex(n + 3);
			const uint nR4 = GetBitReverseIndex(n + 4);
			const uint nR5 = GetBitReverseIndex(n + 5);
			const uint nR6 = GetBitReverseIndex(n + 6);
			const uint nR7 = GetBitReverseIndex(n + 7);

			//	Shuffle the inputs around so that we can do 4 butterflies at once. Current is even, next is odd.
			const f32_4 vCurR = V4fSet(fInReal[nR0], fInReal[nR2], fInReal[nR4], fInReal[nR6]);
			const f32_4 vCurI = V4fSet(fInImag[nR0], fInImag[nR2], fInImag[nR4], fInImag[nR6]);

			const f32_4 vNextR = V4fSet(fInReal[nR1], fInReal[nR3], fInReal[nR5], fInReal[nR7]);
			const f32_4 vNextI = V4fSet(fInImag[nR1], fInImag[nR3], fInImag[nR5], fInImag[nR7]);

			//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
			// Also the input is already pre-shuffled.
			Calculate4Butterflies_DIT_Stage0(vCurR, vNextR, vCurI, vNextI, &fOutR[n], &fOutI[n]);
		}
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_StageTimers[0] += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::Transform_Stage0_BR(const FixedArray<cxT, N>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI)
{
	//	Specialized SIMD case for stage 0 that requires XXZZYYWW shuffling
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Perform the first stage of the transform while copying the input to the output with bit reversal indices.
	for (uint n = 0; n < N; n += 8)
	{
		//	Loop for each 4 butterflies

		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);
		const uint nR2 = GetBitReverseIndex(n + 2);
		const uint nR3 = GetBitReverseIndex(n + 3);
		const uint nR4 = GetBitReverseIndex(n + 4);
		const uint nR5 = GetBitReverseIndex(n + 5);
		const uint nR6 = GetBitReverseIndex(n + 6);
		const uint nR7 = GetBitReverseIndex(n + 7);

		//	Shuffle the inputs around so that we can do 4 butterflies at once. Current is even, next is odd.
		const f32_4 vCurR = V4fSet(cxInput[nR0].r, cxInput[nR2].r, cxInput[nR4].r, cxInput[nR6].r);
		const f32_4 vCurI = V4fSet(cxInput[nR0].i, cxInput[nR2].i, cxInput[nR4].i, cxInput[nR6].i);

		const f32_4 vNextR = V4fSet(cxInput[nR1].r, cxInput[nR3].r, cxInput[nR5].r, cxInput[nR7].r);
		const f32_4 vNextI = V4fSet(cxInput[nR1].i, cxInput[nR3].i, cxInput[nR5].i, cxInput[nR7].i);

		//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
		// Also the input is already pre-shuffled.
		Calculate4Butterflies_DIT_Stage0(vCurR, vNextR, vCurI, vNextI, &fOutR[n], &fOutI[n]);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_StageTimers[0] += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::Transform_Stage0_BR_1stHalf(const FixedArray<cxT, N_2>& cxInput, FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // 2nd half of cxInput is assumed to be all zero
{
	//	Specialized SIMD case for stage 0 that requires XXZZYYWW shuffling
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Perform the first stage of the transform while copying the input to the output with bit reversal indices.
	for (uint n = 0; n < N; n += 8)
	{
		//	Loop for each 4 butterflies

		const uint nR0 = GetBitReverseIndex(n + 0);
//		const uint nR1 = GetBitReverseIndex(n + 1);
		const uint nR2 = GetBitReverseIndex(n + 2);
//		const uint nR3 = GetBitReverseIndex(n + 3);
		const uint nR4 = GetBitReverseIndex(n + 4);
//		const uint nR5 = GetBitReverseIndex(n + 5);
		const uint nR6 = GetBitReverseIndex(n + 6);
//		const uint nR7 = GetBitReverseIndex(n + 7);

		//	Shuffle the inputs around so that we can do 4 butterflies at once. Current is even, next is odd.
		const f32_4 vCurR = V4fSet(cxInput[nR0].r, cxInput[nR2].r, cxInput[nR4].r, cxInput[nR6].r);
		const f32_4 vCurI = V4fSet(cxInput[nR0].i, cxInput[nR2].i, cxInput[nR4].i, cxInput[nR6].i);

//		const f32_4 vNextR = V4fSet(cxInput[nR1].r, cxInput[nR3].r, cxInput[nR5].r, cxInput[nR7].r);
//		const f32_4 vNextI = V4fSet(cxInput[nR1].i, cxInput[nR3].i, cxInput[nR5].i, cxInput[nR7].i);
		const f32_4 vNextR = f32_4::Zero();
		const f32_4 vNextI = f32_4::Zero();

		//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
		// Also the input is already pre-shuffled.
		Calculate4Butterflies_DIT_Stage0(vCurR, vNextR, vCurI, vNextI, &fOutR[n], &fOutI[n]);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_StageTimers[0] += timer.GetTicks();
#endif
}

template <uint M>
template <uint STAGE_CURRENT, uint STAGE_BEGIN>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Transform_Main_DIT(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT > STAGE_BEGIN)
	{
		Transform_Main_DIT<STAGE_CURRENT - 1, STAGE_BEGIN>(fOutR, fOutI);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	FFTL_IF_CONSTEXPR (STAGE_CURRENT == 0)
	{
		//	Loop for each 4 butterflies
		for (uint n = 0; n < N; n += 8)
		{
			//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
			// Also the input is already pre-shuffled.
			Calculate4Butterflies_DIT_Stage0(&fOutR[n], &fOutI[n]);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == 1)
	{
		//	Specialized SIMD case for stage 1 that requires XYXYZWZW shuffling
		//	Get the phase angles for the next 2 sub DFT's
		const f32_4 vUr(1,  0, 1,  0);
		const f32_4 vUi(0, -1, 0, -1);
	
		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < N; uButterfly += 8)
		{
			T* pfR = &fOutR[uButterfly];
			T* pfI = &fOutI[uButterfly];
	
			Calculate4Butterflies_DIT_Stage1(vUr, vUi, pfR, pfI);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == 2)
	{
		//	Only 4 wide SIMD for stage 2
		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

		const f32_4 vUr = f32_4::LoadA(twiddlesReal + 0);
		const f32_4 vUi = f32_4::LoadA(twiddlesImag + 0);

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == M - 1)
	{
		//	We can make an exception in the case of the last stage because the inner loop only runs one time for each sub DFT,
		// and our twiddle factors are all contiguous in memory.
		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

#if FFTL_SIMD_F32x8
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT += 8)
		{
			const f32_8 vUr = f32_8::LoadA(twiddlesReal + nSubDFT);
			const f32_8 vUi = f32_8::LoadA(twiddlesImag + nSubDFT);
	
			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly + nStageExp_2;
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#else
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT += 4)
		{
			const f32_4 vUr = f32_4::LoadA(twiddlesReal + nSubDFT);
			const f32_4 vUi = f32_4::LoadA(twiddlesImag + nSubDFT);
	
			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly + nStageExp_2;
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#endif //FFTL_SIMD_F32x8
	}
	else
	{
		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

#if FFTL_SIMD_F32x8
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT += 8)
		{
			const f32_8 vUr = f32_8::LoadA(twiddlesReal + nSubDFT);
			const f32_8 vUi = f32_8::LoadA(twiddlesImag + nSubDFT);
	
			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
			{
				const uint uButterflyNext = uButterfly + nStageExp_2;
				T* pfCurR = &fOutR[uButterfly];
				T* pfCurI = &fOutI[uButterfly];
				T* pfNextR = &fOutR[uButterflyNext];
				T* pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#else
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT += 4)
		{
			const f32_4 vUr = f32_4::LoadA(twiddlesReal + nSubDFT);
			const f32_4 vUi = f32_4::LoadA(twiddlesImag + nSubDFT);
	
			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
			{
				const uint uButterflyNext = uButterfly + nStageExp_2;
				T* pfCurR = &fOutR[uButterfly];
				T* pfCurI = &fOutI[uButterfly];
				T* pfNextR = &fOutR[uButterflyNext];
				T* pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#endif // FFTL_SIMD_F32x8
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_StageTimers[STAGE_CURRENT] += timer.GetTicks();
#endif
}

template <uint M>
template <uint STAGE_CURRENT, uint STAGE_BEGIN>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Transform_Main_DIF(FixedArray<T, N>& fOutR, FixedArray<T, N>& fOutI) // forced inline to eliminate recursion
{
	//	Complete all the first stages first. This is effectively a template for loop.
	FFTL_IF_CONSTEXPR (STAGE_CURRENT < STAGE_BEGIN)
	{
		Transform_Main_DIF<STAGE_CURRENT + 1, STAGE_BEGIN>(fOutR, fOutI);
	}

	constexpr uint nStageExp = 1 << (STAGE_CURRENT + 1);
	constexpr uint nStageExp_2 = nStageExp >> 1;

#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	FFTL_IF_CONSTEXPR (STAGE_CURRENT == 0)
	{
		//	Specialized SIMD case for stage 0.
		//	Loop for each 4 butterflies
		for (uint n = 0; n < N; n += 8)
		{
			//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
			// Also the input is already pre-shuffled.
			Calculate4Butterflies_DIF_Stage0(&fOutR[n], &fOutI[n]);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == 1)
	{
		//	Specialized SIMD case for stage 1 that requires XYXYZWZW shuffling
		//	Get the phase angles for the next 2 sub DFT's
		const f32_4 vUr(1, 0, 1, 0);
		const f32_4 vUi(0, -1, 0, -1);

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < N; uButterfly += 8)
		{
			T* pfR = &fOutR[uButterfly];
			T* pfI = &fOutI[uButterfly];
	
			Calculate4Butterflies_DIF_Stage1(vUr, vUi, pfR, pfI);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == 2)
	{
		//	Stage 2 easier to just use 4 wide.
		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

		const f32_4 vUr = f32_4::LoadA(twiddlesReal + 0);
		const f32_4 vUi = f32_4::LoadA(twiddlesImag + 0);

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < N; uButterfly += nStageExp)
		{
			const uint uButterflyNext = uButterfly + nStageExp_2;
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}
	else FFTL_IF_CONSTEXPR (STAGE_CURRENT == M - 1)
	{
		//	We can make an exception in the case of the last stage because the inner loop only runs one time for each sub DFT,
		// and our twiddle factors are all contiguous in memory.

		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

#if FFTL_SIMD_F32x8
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2 - 8; nSubDFT >= 0; nSubDFT -= 8)
		{
			const f32_8 vUr = f32_8::LoadA(twiddlesReal + nSubDFT);
			const f32_8 vUi = f32_8::LoadA(twiddlesImag + nSubDFT);
	
			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly + nStageExp_2;
	
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#else
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2 - 4; nSubDFT >= 0; nSubDFT -= 4)
		{
			const f32_4 vUr = f32_4::LoadA(twiddlesReal + nSubDFT);
			const f32_4 vUi = f32_4::LoadA(twiddlesImag + nSubDFT);
	
			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly + nStageExp_2;
	
			T* pfCurR = &fOutR[uButterfly];
			T* pfCurI = &fOutI[uButterfly];
			T* pfNextR = &fOutR[uButterflyNext];
			T* pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#endif //FFTL_SIMD_F32x8
	}

	//	Loop down for each subsequent stage, but leave stage 2, 1 and 0 for special loops.
	else
	{
		const auto& twiddlesReal = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxR();
		const auto& twiddlesImag = FFT_Twiddles<STAGE_CURRENT, T_Twiddle>::GetCplxI();

#if FFTL_SIMD_F32x8
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2 - 8; nSubDFT >= 0; nSubDFT -= 8)
		{
			const f32_8 vUr = f32_8::LoadA(twiddlesReal + nSubDFT);
			const f32_8 vUi = f32_8::LoadA(twiddlesImag + nSubDFT);
	
			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
			{
				const uint uButterflyNext = uButterfly + nStageExp_2;
				T* pfCurR = &fOutR[uButterfly];
				T* pfCurI = &fOutI[uButterfly];
				T* pfNextR = &fOutR[uButterflyNext];
				T* pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#else
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2 - 4; nSubDFT >= 0; nSubDFT -= 4)
		{
			const f32_4 vUr = f32_4::LoadA(twiddlesReal + nSubDFT);
			const f32_4 vUi = f32_4::LoadA(twiddlesImag + nSubDFT);
	
			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < N; uButterfly += nStageExp)
			{
				const uint uButterflyNext = uButterfly + nStageExp_2;
				T* pfCurR = &fOutR[uButterfly];
				T* pfCurI = &fOutI[uButterfly];
				T* pfNextR = &fOutR[uButterflyNext];
				T* pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#endif //FFTL_SIMD_F32x8
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	m_StageTimers[STAGE_CURRENT] += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT<M, f32, f32>::ApplyBitReverseAndInterleave(const FixedArray<T, N>& fInR, const FixedArray<T, N>& fInI, FixedArray<T, N * 2>& fOut)
{
	//	Restore the time domain real output as interleaved real and complex. We need to apply bit reversal here as well.
	for (uint n = 0; n < N; n += 4)
	{
		const uint nR0 = GetBitReverseIndex(n + 0);
		const uint nR1 = GetBitReverseIndex(n + 1);
		const uint nR2 = GetBitReverseIndex(n + 2);
		const uint nR3 = GetBitReverseIndex(n + 3);

		//	Interleave the output while bit reversing.
		T* pSh = fOut + n * 2;

		const T fSh0 = fInR[nR0];
		const T fSh1 = fInI[nR0];
		const T fSh2 = fInR[nR1];
		const T fSh3 = fInI[nR1];
		const T fSh4 = fInR[nR2];
		const T fSh5 = fInI[nR2];
		const T fSh6 = fInR[nR3];
		const T fSh7 = fInI[nR3];

#if 1
		const f32_4 vSh0_3(fSh0, fSh1, fSh2, fSh3);
		const f32_4 vSh4_7(fSh4, fSh5, fSh6, fSh7);

		vSh0_3.StoreA(pSh + 0);
		vSh4_7.StoreA(pSh + 4);
#else
		pSh[0] = fSh0;
		pSh[1] = fSh1;
		pSh[2] = fSh2;
		pSh[3] = fSh3;
		pSh[4] = fSh4;
		pSh[5] = fSh5;
		pSh[6] = fSh6;
		pSh[7] = fSh7;
#endif
	}
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Calculate4Butterflies_DIT_Stage1(f32_4_In vUr, f32_4_In vUi, T* pfR, T* pfI)
{
	//	No need to shuffle the input because we've already pre-shuffled
	const f32_4 vCurR = f32_4::LoadA(pfR + 0);
	const f32_4 vNextR = f32_4::LoadA(pfR + 4);

	const f32_4 vCurI = f32_4::LoadA(pfI + 0);
	const f32_4 vNextI = f32_4::LoadA(pfI + 4);

	const f32_4 Wr = SubMul(vNextR * vUr, vNextI, vUi);
	const f32_4 Wi = AddMul(vNextR * vUi, vNextI, vUr);

	const f32_4 vNewCurR = (vCurR + Wr);
	const f32_4 vNewCurI = (vCurI + Wi);
	const f32_4 vNewNextR = (vCurR - Wr);
	const f32_4 vNewNextI = (vCurI - Wi);

	//	Now shuffle them for the subsequent stages, and store
	const f32_4 vCCNNr0 = Permute<0, 1, 4, 5>(vNewCurR, vNewNextR);
	StoreA(pfR + 0, vCCNNr0);
	const f32_4 vCCNNr1 = Permute<2, 3, 6, 7>(vNewCurR, vNewNextR);
	StoreA(pfR + 4, vCCNNr1);

	const f32_4 vCCNNi0 = Permute<0, 1, 4, 5>(vNewCurI, vNewNextI);
	StoreA(pfI + 0, vCCNNi0);
	const f32_4 vCCNNi1 = Permute<2, 3, 6, 7>(vNewCurI, vNewNextI);
	StoreA(pfI + 4, vCCNNi1);
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Calculate4Butterflies_DIF_Stage1(f32_4_In vUr, f32_4_In vUi, T* pfR, T* pfI)
{
	//	For DIF processing, we're running backwards, so there was no pre-shuffling in stage 2.

	const f32_4 vCCNN0r = f32_4::LoadA(pfR + 0);
	const f32_4 vCCNN1r = f32_4::LoadA(pfR + 4);

	const f32_4 vCCNN0i = f32_4::LoadA(pfI + 0);
	const f32_4 vCCNN1i = f32_4::LoadA(pfI + 4);

	const f32_4 vCurrR = Permute<0, 1, 4, 5>(vCCNN0r, vCCNN1r);
	const f32_4 vNextR = Permute<2, 3, 6, 7>(vCCNN0r, vCCNN1r);

	const f32_4 vCurrI = Permute<0, 1, 4, 5>(vCCNN0i, vCCNN1i);
	const f32_4 vNextI = Permute<2, 3, 6, 7>(vCCNN0i, vCCNN1i);

	const f32_4 Wr = vCurrR - vNextR;
	const f32_4 Wi = vCurrI - vNextI;

	const f32_4 vNewCurR = vCurrR + vNextR;
	const f32_4 vNewCurI = vCurrI + vNextI;
	const f32_4 vNewNextR = SubMul(Wr * vUr, Wi, vUi);
	const f32_4 vNewNextI = AddMul(Wr * vUi, Wi, vUr);

	//	Don't pre-shuffle for stage 0. Stage 0 will self-correct.
	StoreA(pfR + 0, vNewCurR);
	StoreA(pfR + 4, vNewNextR);
	StoreA(pfI + 0, vNewCurI);
	StoreA(pfI + 4, vNewNextI);
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Calculate4Butterflies_DIT_Stage0(T* pfR, T* pfI)
{
	//	Shuffle the input around for the first stage so we can properly process 4 at a time.

	const f32_4 vCNCN0r = f32_4::LoadA(pfR + 0);
	const f32_4 vCNCN1r = f32_4::LoadA(pfR + 4);

	const f32_4 vCNCN0i = f32_4::LoadA(pfI + 0);
	const f32_4 vCNCN1i = f32_4::LoadA(pfI + 4);

	const f32_4 vCurrR = Permute<0, 2, 4, 6>(vCNCN0r, vCNCN1r);
	const f32_4 vNextR = Permute<1, 3, 5, 7>(vCNCN0r, vCNCN1r);

	const f32_4 vCurrI = Permute<0, 2, 4, 6>(vCNCN0i, vCNCN1i);
	const f32_4 vNextI = Permute<1, 3, 5, 7>(vCNCN0i, vCNCN1i);

	const f32_4 vNewCurR = vCurrR + vNextR;
	const f32_4 vNewCurI = vCurrI + vNextI;
	const f32_4 vNewNextR = vCurrR - vNextR;
	const f32_4 vNewNextI = vCurrI - vNextI;

	//	Now shuffle them so that stage 1 doesn't have to pre-shuffle on input, and store
	const f32_4 vCCNNr0 = Permute<0, 4, 2, 6>(vNewCurR, vNewNextR);
	const f32_4 vCCNNr1 = Permute<1, 5, 3, 7>(vNewCurR, vNewNextR);
	const f32_4 vCCNNi0 = Permute<0, 4, 2, 6>(vNewCurI, vNewNextI);
	const f32_4 vCCNNi1 = Permute<1, 5, 3, 7>(vNewCurI, vNewNextI);

	StoreA(pfR + 0, vCCNNr0);
	StoreA(pfR + 4, vCCNNr1);
	StoreA(pfI + 0, vCCNNi0);
	StoreA(pfI + 4, vCCNNi1);
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Calculate4Butterflies_DIF_Stage0(T* pfR, T* pfI)
{
	//	For DIF processing, we're running backwards, so stage 1 has processed before us, and we need to correct for its order.

	const f32_4 vs1_0r = f32_4::LoadA(pfR + 0);
	const f32_4 vs1_1r = f32_4::LoadA(pfR + 4);

	const f32_4 vs1_0i = f32_4::LoadA(pfI + 0);
	const f32_4 vs1_1i = f32_4::LoadA(pfI + 4);

	const f32_4 vCurrR = Permute<0, 4, 2, 6>(vs1_0r, vs1_1r);
	const f32_4 vNextR = Permute<1, 5, 3, 7>(vs1_0r, vs1_1r);

	const f32_4 vCurrI = Permute<0, 4, 2, 6>(vs1_0i, vs1_1i);
	const f32_4 vNextI = Permute<1, 5, 3, 7>(vs1_0i, vs1_1i);

	const f32_4 vNewCurR = vCurrR + vNextR;
	const f32_4 vNewCurI = vCurrI + vNextI;
	const f32_4 vNewNextR = vCurrR - vNextR;
	const f32_4 vNewNextI = vCurrI - vNextI;

	//	Now post shuffle them back to the normal (final) order.
	const f32_4 vCNCNr0 = Permute<0, 4, 1, 5>(vNewCurR, vNewNextR);
	const f32_4 vCNCNr1 = Permute<2, 6, 3, 7>(vNewCurR, vNewNextR);
	const f32_4 vCNCNi0 = Permute<0, 4, 1, 5>(vNewCurI, vNewNextI);
	const f32_4 vCNCNi1 = Permute<2, 6, 3, 7>(vNewCurI, vNewNextI);

	StoreA(pfR + 0, vCNCNr0);
	StoreA(pfR + 4, vCNCNr1);
	StoreA(pfI + 0, vCNCNi0);
	StoreA(pfI + 4, vCNCNi1);
}

template <uint M>
FFTL_FORCEINLINE void FFT<M, f32, f32>::Calculate4Butterflies_DIT_Stage0(f32_4_In vCurR, f32_4_In vNextR, f32_4_In vCurI, f32_4_In vNextI, T* pfR, T* pfI)
{
	//	No need to shuffle the input because we've already pre-shuffled
	const f32_4 vNewCurR = vCurR + vNextR;
	const f32_4 vNewCurI = vCurI + vNextI;
	const f32_4 vNewNextR = vCurR - vNextR;
	const f32_4 vNewNextI = vCurI - vNextI;

	//	Now shuffle them so that stage 1 doesn't have to pre-shuffle on input, and store
	const f32_4 vCCNNr0 = Permute<0, 4, 2, 6>(vNewCurR, vNewNextR);
	const f32_4 vCCNNr1 = Permute<1, 5, 3, 7>(vNewCurR, vNewNextR);
	const f32_4 vCCNNi0 = Permute<0, 4, 2, 6>(vNewCurI, vNewNextI);
	const f32_4 vCCNNi1 = Permute<1, 5, 3, 7>(vNewCurI, vNewNextI);

	StoreA(pfR + 0, vCCNNr0);
	StoreA(pfR + 4, vCCNNr1);
	StoreA(pfI + 0, vCCNNi0);
	StoreA(pfI + 4, vCCNNi1);
}

template <uint M>
template <typename V>
FFTL_FORCEINLINE void FFT<M, f32, f32>::CalculateVButterflies_DIT(const V& vUr, const V& vUi, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	const V vCurR = V::LoadA(pfCurR);
	const V vCurI = V::LoadA(pfCurI);
	const V vNextR = V::LoadA(pfNextR);
	const V vNextI = V::LoadA(pfNextI);

#if defined(FFTL_HAS_FMA) && 0 // Doesn't quite work for vUr values that are zero
	vUi = vUi / vUr;
	const V Wr = SubMul(vNextR, vNextI, vUi);
	const V Wi = AddMul(vNextI, vNextR, vUi);

	const V vNewCurR = AddMul(vCurR, Wr, vUr);
	const V vNewCurI = AddMul(vCurI, Wi, vUr);
	const V vNewNextR = SubMul(vCurR, Wr, vUr);
	const V vNewNextI = SubMul(vCurI, Wi, vUr);
#else
	const V Wr = SubMul(vNextR * vUr, vNextI, vUi);
	const V Wi = AddMul(vNextR * vUi, vNextI, vUr);

	const V vNewCurR = vCurR + Wr;
	const V vNewCurI = vCurI + Wi;
	const V vNewNextR = vCurR - Wr;
	const V vNewNextI = vCurI - Wi;
#endif

	StoreA(pfNextR, vNewNextR);
	StoreA(pfNextI, vNewNextI);
	StoreA(pfCurR, vNewCurR);
	StoreA(pfCurI, vNewCurI);
}

template <uint M>
template <typename V>
FFTL_FORCEINLINE void FFT<M, f32, f32>::CalculateVButterflies_DIF(const V& vUr, const V& vUi, T* pfCurR, T* pfCurI, T* pfNextR, T* pfNextI)
{
	const V vCurR = V::LoadA(pfCurR);
	const V vCurI = V::LoadA(pfCurI);
	const V vNextR = V::LoadA(pfNextR);
	const V vNextI = V::LoadA(pfNextI);

	const V Wr = vCurR - vNextR;
	const V Wi = vCurI - vNextI;

	const V vNewCurR = vCurR + vNextR;
	const V vNewCurI = vCurI + vNextI;
	const V vNewNextR = SubMul(Wr * vUr, Wi, vUi);
	const V vNewNextI = AddMul(Wr * vUi, Wi, vUr);

	StoreA(pfNextR, vNewNextR);
	StoreA(pfNextI, vNewNextI);
	StoreA(pfCurR, vNewCurR);
	StoreA(pfCurI, vNewCurI);
}

#endif // FFTL_SIMD_F32x4





template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Real_Base<M, T, T_Twiddle>::TransformForward(const FixedArray<T, N>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI)
{
	const FixedArray<cxT, N_2>& cxInput = *reinterpret_cast<const FixedArray<cxT, N_2>*>(&fTimeIn);

	//	Perform the half size complex FFT
	sm_fft::TransformForward(cxInput, fFreqOutR, fFreqOutI);

#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Special case for 0 index
	{
		const T fDcR = fFreqOutR[0];
		const T fDcI = fFreqOutI[0];
		fFreqOutR[0] = fDcR + fDcI;
		fFreqOutI[0] = fDcR - fDcI; // Sneaky shove of the Nyquist bin into the imag DC bin because it's always 0 anyway.
	}

	const T vHalf = ConvertTo<T>(0.5f);

	for (uint n = 1; n < N_4; n += 1)
	{
		const uint Nmn = N_2 - n;

		const cxNumber<T> twid(ConvertTo<T>(GetTwiddleReal(n)), ConvertTo<T>(GetTwiddleImag(n)));

		const cxNumber<T> fpk(fFreqOutR[n], fFreqOutI[n]);
		const cxNumber<T> fpnk(fFreqOutR[Nmn], -fFreqOutI[Nmn]);

		const cxNumber<T> f1k = fpk + fpnk;
		const cxNumber<T> f2k = fpk - fpnk;
		const cxNumber<T> tw = f2k * twid;

		fFreqOutR[n] = vHalf * (f1k.r + tw.r);
		fFreqOutI[n] = vHalf * (f1k.i + tw.i);
		fFreqOutR[Nmn] = vHalf * (f1k.r - tw.r);
		fFreqOutI[Nmn] = vHalf * (tw.i - f1k.i);
	}

	//	The odd center bin just needs the imaginary part negated.
	fFreqOutI[N_4] = -fFreqOutI[N_4];

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PostProcessTimer += timer.GetTicks();
#endif
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Real_Base<M, T, T_Twiddle>::TransformForward_1stHalf(const FixedArray<T, N_2>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI) // 2nd half of fTimeIn is assumed to be zero
{
	const FixedArray<cxT, N_4>& cxInput = *reinterpret_cast<const FixedArray<cxT, N_4>*>(&fTimeIn);

	//	Perform the half size complex FFT
	sm_fft::TransformForward_1stHalf(cxInput, fFreqOutR, fFreqOutI);

#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Special case for 0 index
	{
		const T fDcR = fFreqOutR[0];
		const T fDcI = fFreqOutI[0];
		fFreqOutR[0] = fDcR + fDcI;
		fFreqOutI[0] = fDcR - fDcI; // Sneaky shove of the Nyquist bin into the imag DC bin because it's always 0 anyway.
	}

	const T vHalf = ConvertTo<T>(0.5);

	for (uint n = 1; n < N_4; n += 1)
	{
		const uint Nmn = N_2 - n;

		const cxNumber<T> twid(ConvertTo<T>(GetTwiddleReal(n)), ConvertTo<T>(GetTwiddleImag(n)));

		const cxNumber<T> fpk(fFreqOutR[n], fFreqOutI[n]);
		const cxNumber<T> fpnk(fFreqOutR[Nmn], -fFreqOutI[Nmn]);

		const cxNumber<T> f1k = fpk + fpnk;
		const cxNumber<T> f2k = fpk - fpnk;
		const cxNumber<T> tw = f2k * twid;

		fFreqOutR[n] = vHalf * (f1k.r + tw.r);
		fFreqOutI[n] = vHalf * (f1k.i + tw.i);
		fFreqOutR[Nmn] = vHalf * (f1k.r - tw.r);
		fFreqOutI[Nmn] = vHalf * (tw.i - f1k.i);
	}

	//	The odd center bin just needs the imaginary part negated.
	fFreqOutI[N_4] = -fFreqOutI[N_4];

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PostProcessTimer += timer.GetTicks();
#endif
}

template <uint M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Real_Base<M, T, T_Twiddle>::TransformInverse(const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	FixedArray<cxT, N_2>& cxTimeOut = *reinterpret_cast<FixedArray<cxT, N_2>*>(fTimeOut + 0);

	const T vInv_N = ConvertTo<T>((T_Twiddle)1.0 / N);

	//	Special case for 0 index
	{
		const f32 fDC = fFreqInR[0] * vInv_N;
		const f32 fNy = fFreqInI[0] * vInv_N;
		cxTimeOut[0].r = fDC + fNy;
		cxTimeOut[0].i = fDC - fNy;
	}

	for (uint n = 1; n < N_4; n += 1)
	{
		const uint Nmn = N_2 - n;

		const cxNumber<T> twid(ConvertTo<T>(GetTwiddleReal(n)), -ConvertTo<T>(GetTwiddleImag(n)));

		const cxNumber<T> fk(fFreqInR[n], fFreqInI[n]);
		const cxNumber<T> fnkc(fFreqInR[Nmn], -fFreqInI[Nmn]);

		const cxNumber<T> fek = fk + fnkc;
		const cxNumber<T> tmp = fk - fnkc;
		const cxNumber<T> fok = tmp * twid;

		const uint nR = sm_fft::GetBitReverseIndex(n);
		const uint NmnR = sm_fft::GetBitReverseIndex(Nmn);

		//	Flip real and imaginary for inverse FFT and apply bit reversal for DIF version.
		cxTimeOut[nR].r = (fek.r + fok.r) * vInv_N;
		cxTimeOut[nR].i = (fek.i + fok.i) * vInv_N;
		cxTimeOut[NmnR].r = (fek.r - fok.r) * vInv_N;
		cxTimeOut[NmnR].i = (fok.i - fek.i) * vInv_N;
	}

	const T vTwo = ConvertTo<T>(2) * vInv_N;
	const uint nR = sm_fft::GetBitReverseIndex(N_4);

	//	The odd center bin just needs to be doubled and the imaginary part negated.
	cxTimeOut[nR].r = fFreqInR[N_4] * +vTwo;
	cxTimeOut[nR].i = fFreqInI[N_4] * -vTwo;

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PreProcessTimer += timer.GetTicks();
#endif

	//	Perform the half size complex inverse FFT
	sm_fft::TransformInverse_InPlace_DIT(cxTimeOut);
}

template <uint M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Real_Base<M, T, T_Twiddle>::TransformInverse_ClobberInput(FixedArray<T, N_2>& fFreqInR, FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut)
{
	//	No input clobbering necessary here because there's no stack allocation.
	TransformInverse(fFreqInR, fFreqInI, fTimeOut);
}







#if FFTL_SIMD_F32x4

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::TransformForward(const FixedArray<T, N>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI)
{
	//	Perform the half size complex FFT
	sm_fft::TransformForward(*reinterpret_cast<const FixedArray<cxT, N_2>*>(&fTimeIn), fFreqOutR, fFreqOutI);
	PostProcessForward(fFreqOutR, fFreqOutI);
}

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::TransformForward_1stHalf(const FixedArray<T, N_2>& fTimeIn, FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI) // 2nd half of fTimeIn is assumed to be all zeros
{
	//	Perform the half size complex FFT
	sm_fft::TransformForward_1stHalf(*reinterpret_cast<const FixedArray<cxT, N_4>*>(&fTimeIn), fFreqOutR, fFreqOutI);
	PostProcessForward(fFreqOutR, fFreqOutI);
}

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::PostProcessForward(FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Special case for 0 index
	{
		const f32 fDcR = fFreqOutR[0];
		const f32 fDcI = fFreqOutI[0];
		fFreqOutR[0] = fDcR + fDcI;
		fFreqOutI[0] = fDcR - fDcI; // Sneaky shove of the Nyquist bin into the imag DC bin because it's always 0 anyway.
	}

	const f32_4 vHalf = ConvertTo<f32_4>(0.5f);

	//	Special case for 3 element loads and stores we only need to do once.
	{
		const uint n = 1;
		const uint Nmn = N_2 - n - 2;

		//	Starting at 1 forces an unaligned load.
		const cxNumber<f32_4> twid(f32_4::LoadU(GetTwiddleRealPtr(n)), f32_4::LoadU(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fpk(f32_4::LoadU(fFreqOutR + n), f32_4::LoadU(fFreqOutI + n));
		const cxNumber<f32_4> fpnk(ZYXX(f32_4::Load3(fFreqOutR + Nmn)), ZYXX(f32_4::Load3(fFreqOutI + Nmn)));

		const cxNumber<f32_4> f1k = cxNumber<f32_4>(fpk.r + fpnk.r, fpk.i - fpnk.i);
		const cxNumber<f32_4> f2k = cxNumber<f32_4>(fpk.r - fpnk.r, fpk.i + fpnk.i);
		const cxNumber<f32_4> tw = f2k * twid;

		//	Store only 3 so we don't pollute the next stage
		(vHalf * (f1k.r + tw.r)).Store3(fFreqOutR + n);
		(vHalf * (f1k.i + tw.i)).Store3(fFreqOutI + n);
		ZYXX(vHalf * (f1k.r - tw.r)).Store3(fFreqOutR + Nmn);
		ZYXX(vHalf * (tw.i - f1k.i)).Store3(fFreqOutI + Nmn);
	}

	//	N-n loading will be unaligned.
	{
		const uint n = 4;
		const uint Nmn = N_2 - n - 3;

		const cxNumber<f32_4> twid(f32_4::LoadA(GetTwiddleRealPtr(n)), f32_4::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fpk(f32_4::LoadA(fFreqOutR + n), f32_4::LoadA(fFreqOutI + n));
		const cxNumber<f32_4> fpnk(Reverse(f32_4::LoadU(fFreqOutR + Nmn)), Reverse(f32_4::LoadU(fFreqOutI + Nmn)));

		const cxNumber<f32_4> f1k = cxNumber<f32_4>(fpk.r + fpnk.r, fpk.i - fpnk.i);
		const cxNumber<f32_4> f2k = cxNumber<f32_4>(fpk.r - fpnk.r, fpk.i + fpnk.i);
		const cxNumber<f32_4> tw = f2k * twid;

		(vHalf * (f1k.r + tw.r)).StoreA(fFreqOutR + n);
		(vHalf * (f1k.i + tw.i)).StoreA(fFreqOutI + n);
		Reverse(vHalf * (f1k.r - tw.r)).StoreU(fFreqOutR + Nmn);
		Reverse(vHalf * (tw.i - f1k.i)).StoreU(fFreqOutI + Nmn);
	}
#if FFTL_SIMD_F32x8
	//	N-n loading will be unaligned.
	for (uint n = 8; n < N_4; n += 8)
	{
		const f32_8 vHalf8 = f32_8::Splat(vHalf);

		const uint Nmn = N_2 - n - 7;

		const cxNumber<f32_8> twid(f32_8::LoadA(GetTwiddleRealPtr(n)), f32_8::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_8> fpk(f32_8::LoadA(fFreqOutR + n), f32_8::LoadA(fFreqOutI + n));
		const cxNumber<f32_8> fpnk(Reverse(f32_8::LoadU(fFreqOutR + Nmn)), Reverse(f32_8::LoadU(fFreqOutI + Nmn)));

		const cxNumber<f32_8> f1k = cxNumber<f32_8>(fpk.r + fpnk.r, fpk.i - fpnk.i);
		const cxNumber<f32_8> f2k = cxNumber<f32_8>(fpk.r - fpnk.r, fpk.i + fpnk.i);
		const cxNumber<f32_8> tw = f2k * twid;

		(vHalf8 * (f1k.r + tw.r)).StoreA(fFreqOutR + n);
		(vHalf8 * (f1k.i + tw.i)).StoreA(fFreqOutI + n);
		Reverse(vHalf8 * (f1k.r - tw.r)).StoreU(fFreqOutR + Nmn);
		Reverse(vHalf8 * (tw.i - f1k.i)).StoreU(fFreqOutI + Nmn);
	}
#else
	//	N-n loading will be unaligned.
	for (uint n = 8; n < N_4; n += 4)
	{
		const uint Nmn = N_2 - n - 3;

		const cxNumber<f32_4> twid(f32_4::LoadA(GetTwiddleRealPtr(n)), f32_4::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fpk(f32_4::LoadA(fFreqOutR + n), f32_4::LoadA(fFreqOutI + n));
		const cxNumber<f32_4> fpnk(Reverse(f32_4::LoadU(fFreqOutR + Nmn)), Reverse(f32_4::LoadU(fFreqOutI + Nmn)));

		const cxNumber<f32_4> f1k = cxNumber<f32_4>(fpk.r + fpnk.r, fpk.i - fpnk.i);
		const cxNumber<f32_4> f2k = cxNumber<f32_4>(fpk.r - fpnk.r, fpk.i + fpnk.i);
		const cxNumber<f32_4> tw = f2k * twid;

		(vHalf * (f1k.r + tw.r)).StoreA(fFreqOutR + n);
		(vHalf * (f1k.i + tw.i)).StoreA(fFreqOutI + n);
		Reverse(vHalf * (f1k.r - tw.r)).StoreU(fFreqOutR + Nmn);
		Reverse(vHalf * (tw.i - f1k.i)).StoreU(fFreqOutI + Nmn);
	}
#endif

	//	The odd center bin just needs the imaginary part negated.
	fFreqOutI[N_4] = -fFreqOutI[N_4];

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PostProcessTimer += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::PreProcessInverse(FixedArray<T, N_2>& fFreqOutR, FixedArray<T, N_2>& fFreqOutI, const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	//	Special case for 0 index
	{
		const f32 fDC = fFreqInR[0];
		const f32 fNy = fFreqInI[0];
		fFreqOutR[0] = fDC + fNy;
		fFreqOutI[0] = fDC - fNy;
	}

	//	Special case for 3 element loads and stores we only need to do once.
	{
		const uint n = 1;
		const uint Nmn = N_2 - n - 2;

		//	Starting at 1 forces an unaligned load.
		const cxNumber<f32_4> twid(f32_4::LoadU(GetTwiddleRealPtr(n)), f32_4::LoadU(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fk(f32_4::LoadU(fFreqInR + n), f32_4::LoadU(fFreqInI + n));
		const cxNumber<f32_4> fnkc(ZYXX(f32_4::Load3(fFreqInR + Nmn)), ZYXX(f32_4::Load3(fFreqInI + Nmn)));

		const cxNumber<f32_4> fek = cxNumber<f32_4>(fk.r + fnkc.r, fk.i - fnkc.i);
		const cxNumber<f32_4> tmp = cxNumber<f32_4>(fk.r - fnkc.r, fk.i + fnkc.i);
		const cxNumber<f32_4> fok = NegMul(tmp, twid);

		//	Store only 3 so we don't pollute the next stage
		(fek.r + fok.r).Store3(fFreqOutR + n);
		(fek.i + fok.i).Store3(fFreqOutI + n);
		ZYXX(fek.r - fok.r).Store3(fFreqOutR + Nmn);
		ZYXX(fok.i - fek.i).Store3(fFreqOutI + Nmn);
	}

	//	N-n loading will be unaligned.
	{
		const uint n = 4;
		const uint Nmn = N_2 - n - 3;

		const cxNumber<f32_4> twid(f32_4::LoadA(GetTwiddleRealPtr(n)), f32_4::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fk(f32_4::LoadA(fFreqInR + n), f32_4::LoadA(fFreqInI + n));
		const cxNumber<f32_4> fnkc(Reverse(f32_4::LoadU(fFreqInR + Nmn)), Reverse(f32_4::LoadU(fFreqInI + Nmn)));

		const cxNumber<f32_4> fek = cxNumber<f32_4>(fk.r + fnkc.r, fk.i - fnkc.i);
		const cxNumber<f32_4> tmp = cxNumber<f32_4>(fk.r - fnkc.r, fk.i + fnkc.i);
		const cxNumber<f32_4> fok = NegMul(tmp, twid);

		(fek.r + fok.r).StoreA(fFreqOutR + n);
		(fek.i + fok.i).StoreA(fFreqOutI + n);
		Reverse(fek.r - fok.r).StoreU(fFreqOutR + Nmn);
		Reverse(fok.i - fek.i).StoreU(fFreqOutI + Nmn);
	}

#if FFTL_SIMD_F32x8
	//	N-n loading will be unaligned.
	for (uint n = 8; n < N_4; n += 8)
	{
		const uint Nmn = N_2 - n - 7;

		const cxNumber<f32_8> twid(f32_8::LoadA(GetTwiddleRealPtr(n)), f32_8::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_8> fk(f32_8::LoadA(fFreqInR + n), f32_8::LoadA(fFreqInI + n));
		const cxNumber<f32_8> fnkc(Reverse(f32_8::LoadU(fFreqInR + Nmn)), Reverse(f32_8::LoadU(fFreqInI + Nmn)));

		const cxNumber<f32_8> fek = cxNumber<f32_8>(fk.r + fnkc.r, fk.i - fnkc.i);
		const cxNumber<f32_8> tmp = cxNumber<f32_8>(fk.r - fnkc.r, fk.i + fnkc.i);
		const cxNumber<f32_8> fok = NegMul(tmp, twid);

		(fek.r + fok.r).StoreA(fFreqOutR + n);
		(fek.i + fok.i).StoreA(fFreqOutI + n);
		Reverse(fek.r - fok.r).StoreU(fFreqOutR + Nmn);
		Reverse(fok.i - fek.i).StoreU(fFreqOutI + Nmn);
	}
#else
	//	N-n loading will be unaligned.
	for (uint n = 8; n < N_4; n += 4)
	{
		const uint Nmn = N_2 - n - 3;

		const cxNumber<f32_4> twid(f32_4::LoadA(GetTwiddleRealPtr(n)), f32_4::LoadA(GetTwiddleImagPtr(n)));

		const cxNumber<f32_4> fk(f32_4::LoadA(fFreqInR + n), f32_4::LoadA(fFreqInI + n));
		const cxNumber<f32_4> fnkc(Reverse(f32_4::LoadU(fFreqInR + Nmn)), Reverse(f32_4::LoadU(fFreqInI + Nmn)));

		const cxNumber<f32_4> fek = cxNumber<f32_4>(fk.r + fnkc.r, fk.i - fnkc.i);
		const cxNumber<f32_4> tmp = cxNumber<f32_4>(fk.r - fnkc.r, fk.i + fnkc.i);
		const cxNumber<f32_4> fok = NegMul(tmp, twid);

		(fek.r + fok.r).StoreA(fFreqOutR + n);
		(fek.i + fok.i).StoreA(fFreqOutI + n);
		Reverse(fek.r - fok.r).StoreU(fFreqOutR + Nmn);
		Reverse(fok.i - fek.i).StoreU(fFreqOutI + Nmn);
	}
#endif

	//	The odd center bin just needs to be doubled and the imaginary part negated.
	fFreqOutR[N_4] = fFreqInR[N_4] * +2.f;
	fFreqOutI[N_4] = fFreqInI[N_4] * -2.f;

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PreProcessTimer += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::TransformInverse(const FixedArray<T, N_2>& fFreqInR, const FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	FixedArray<T, N_2>& fFftInR = *reinterpret_cast<FixedArray<T, N_2>*>(fTimeOut + 0);
	FixedArray<T, N_2>& fFftInI = *reinterpret_cast<FixedArray<T, N_2>*>(fTimeOut + N_2);

	PreProcessInverse(fFftInR, fFftInI, fFreqInR, fFreqInI);

	//	Perform the half size complex inverse FFT
	FixedArray_Aligned32<T, N_2> fTempR;
	FixedArray_Aligned32<T, N_2> fTempI;
	sm_fft::TransformInverse(fFftInR, fFftInI, fTempR, fTempI);

#if FFTL_STAGE_TIMERS
	timer.Start();
#endif

	const f32_4 vInv_N = ConvertTo<f32_4>(1.f / N);

	//	Restore the time domain real output as interleaved real and complex.
	for (uint n = 0; n < N_2; n += 4)
	{
		const f32_4 vInA = f32_4::LoadA(fTempR + n) * vInv_N;
		const f32_4 vInB = f32_4::LoadA(fTempI + n) * vInv_N;
		const f32_4 vShA = MergeXY(vInA, vInB); // Evens
		const f32_4 vShB = MergeZW(vInA, vInB); // Odds
		vShA.StoreA(fTimeOut + n * 2 + 0);
		vShB.StoreA(fTimeOut + n * 2 + 4);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PostProcessTimer += timer.GetTicks();
#endif
}

template <uint M>
FFTL_COND_INLINE void FFT_Real<M, f32, f32>::TransformInverse_ClobberInput(FixedArray<T, N_2>& fFreqInR, FixedArray<T, N_2>& fFreqInI, FixedArray<T, N>& fTimeOut)
{
#if FFTL_STAGE_TIMERS
	Timer timer;
	timer.Start();
#endif

	FixedArray<T, N_2>& fFftInR = fFreqInR;
	FixedArray<T, N_2>& fFftInI = fFreqInI;

	PreProcessInverse(fFftInR, fFftInI, fFreqInR, fFreqInI);

	//	Perform the half size complex inverse FFT
	sm_fft::TransformForward_InPlace_DIF(fFftInI, fFftInR); // Reverse real and imaginary for inverse FFT

#if FFTL_STAGE_TIMERS
	timer.Start();
#endif

	const f32_4 vInv_N = ConvertTo<f32_4>(1.f / N);

	//	Restore the time domain real output as interleaved real and complex. We need to apply bit reversal here as well.
	for (uint n = 0; n < N_2; n += 4)
	{
		const uint nR0 = sm_fft::GetBitReverseIndex(n + 0);
		const uint nR1 = sm_fft::GetBitReverseIndex(n + 1);
		const uint nR2 = sm_fft::GetBitReverseIndex(n + 2);
		const uint nR3 = sm_fft::GetBitReverseIndex(n + 3);

		//	Interleave the output while bit reversing.
		const f32_4 vShA(fFftInR[nR0], fFftInI[nR0], fFftInR[nR1], fFftInI[nR1]);
		const f32_4 vShB(fFftInR[nR2], fFftInI[nR2], fFftInR[nR3], fFftInI[nR3]);

		(vShA * vInv_N).StoreA(fTimeOut + n * 2 + 0);
		(vShB * vInv_N).StoreA(fTimeOut + n * 2 + 4);
	}

#if FFTL_STAGE_TIMERS
	timer.Stop();
	sm_fft::m_PostProcessTimer += timer.GetTicks();
#endif
}
#endif // FFTL_SIMD_F32x4



template <uint M>
void FFT_RealV<M>::TransformForward(const f32* fTimeIn, f32* fFreqOutR, f32* fFreqOutI) const
{
	FFT_Real<M, f32>::TransformForward(*reinterpret_cast<const FixedArray<f32, N>*>(fTimeIn), *reinterpret_cast<FixedArray<f32, N_2>*>(fFreqOutR), *reinterpret_cast<FixedArray<f32, N_2>*>(fFreqOutI));
}
template <uint M>
void FFT_RealV<M>::TransformInverse(const f32* fFreqInR, const f32* fFreqInI, f32* fTimeOut) const
{
	FFT_Real<M, f32>::TransformInverse(*reinterpret_cast<const FixedArray<f32, N_2>*>(fFreqInR), *reinterpret_cast<const FixedArray<f32, N_2>*>(fFreqInI), *reinterpret_cast<FixedArray<f32, N>*>(fTimeOut));
}
template <uint M>
void FFT_RealV<M>::TransformInverse_ClobberInput(f32* fFreqInR, f32* fFreqInI, f32* fTimeOut) const
{
	FFT_Real<M, f32>::TransformInverse_ClobberInput(*reinterpret_cast<FixedArray<f32, N_2>*>(fFreqInR), *reinterpret_cast<FixedArray<f32, N_2>*>(fFreqInI), *reinterpret_cast<FixedArray<f32, N>*>(fTimeOut));
}







template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::Convolver()
{
	MemZero(m_AccumulationBuffer);
	MemZero(m_PrevTail);
	MemZero(m_PrevTail);
	MemZero(m_tempBufferA);
	MemZero(m_tempBufferB);
	MemZero(m_inputSignal_FD);
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::Convolve(FixedArray_Aligned32<T, N>& fInOutput, const Kernel* pKernelArray_FD, size_t kernelArraySize)
{
	ConvolveInitial_FirstStage(fInOutput, pKernelArray_FD, kernelArraySize);
	ConvolveInitial_LastStage(fInOutput);
	ConvolveResumePartial(pKernelArray_FD, kernelArraySize, 1, Max(kernelArraySize, m_LeftoverKernelCount));
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveInitial_FirstStage(const FixedArray_Aligned32<T, N>& fInput, const Kernel* pKernelArray_FD, size_t kernelArraySize)
{
	m_bConvolutionFrameComplete = false;
	m_LeftoverKernelCount = Max(m_LeftoverKernelCount, kernelArraySize);

	if (kernelArraySize > 0)
	{
		FFTL_ASSERT(pKernelArray_FD != nullptr);

		{
			//	FFT Timer
#if defined(FFTL_ENABLE_PROFILING)
			FFTL_PROFILE_TIMERSCOPE(timer, &m_timerFftForward);
#endif
			//	Convert the time domain input to freq domain
			sm_fft::TransformForward_1stHalf(fInput, m_inputSignal_FD.r(), m_inputSignal_FD.i());
		}

		//	Convolve the first segment, perform IFFT, and write to the output
		const Kernel& kernel = pKernelArray_FD[0];
		Kernel& curBuffer = m_AccumulationBuffer[0];

		{
			//	Convolution Timer
#if defined(FFTL_ENABLE_PROFILING)
			FFTL_PROFILE_TIMERSCOPE_PAUSE(timer, &m_timerConvolution);
#endif
			//	Frequency domain convolution
			ConvolveFD(m_tempBufferA, m_inputSignal_FD, kernel, curBuffer);
		}

		m_bInputSignalHasData = true;
	}
	else if (m_LeftoverKernelCount > 0)
	{
		//	If nothing to convolve anymore, fill in the previous convolution segments
		//	No convolution is necessary here because the input is effectively zero
		m_tempBufferA = m_AccumulationBuffer[0];
		m_bInputSignalHasData = false;
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveInitial_LastStage(FixedArray_Aligned32<T, N>& fOutput)
{
	if (m_LeftoverKernelCount > 0)
	{
		{
			//	FFT Timer
#if defined(FFTL_ENABLE_PROFILING)
			FFTL_PROFILE_TIMERSCOPE(timer, &m_timerFftInverse);
#endif
			//	Convert the new frequency domain signal back to the time domain
			sm_fft::TransformInverse_ClobberInput(m_tempBufferA.r(), m_tempBufferA.i(), m_tempBufferB.t);
		}

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		AddArrays(fOutput, m_PrevTail, m_tempBufferB.r());

		//	Store the 2nd half of the IFFT buffer (reverb tail)
		MemCopy(m_PrevTail, m_tempBufferB.i());
	}
	else
	{
		MemZero(fOutput);
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveResumePartial(const Kernel* pKernelArray_FD, size_t kernelArraySize, size_t startKernelIndex, size_t endKernelIndex)
{
	auto ConvolveLoopLambda = [this](const Kernel* pKernelArray_FD, size_t startKernelIndex, size_t endKernelIndex)
	{
		//	Use min here just in case we start processing a different array than we did in ConvolveInitial
//		m_LeftoverKernelCount = Min(m_LeftoverKernelCount, kernelArraySize);

		//	Perform short convolutions on the remaining kernels, accumulating everything as necessary
		for (size_t k = Max(startKernelIndex, 1u); k < endKernelIndex; ++k)
		{
			const Kernel& kernel = pKernelArray_FD[k];
			Kernel& curBuffer = m_AccumulationBuffer[k];
			Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

			//	Frequency domain convolution
#if defined(FFTL_ENABLE_PROFILING)
			FFTL_PROFILE_TIMERSCOPE_PAUSE(timer, &m_timerConvolution);
#endif
			ConvolveFD(prvBuffer, m_inputSignal_FD, kernel, curBuffer);
		}
	};

	auto TailConvolveLambda = [this](size_t startKernelIndex, size_t endKernelIndex)
	{
		//	Finish up the previous decay if we've switched to a shorter kernel, and we've completed convolution on the kernel
		for (size_t k = Max(startKernelIndex, 1u); k < Min(endKernelIndex, m_LeftoverKernelCount); ++k)
		{
			Kernel& curBuffer = m_AccumulationBuffer[k];
			Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

			prvBuffer = curBuffer;
		}

		if (endKernelIndex >= m_LeftoverKernelCount)
		{
			if (m_LeftoverKernelCount > 1)
			{
				MemZero(m_AccumulationBuffer[m_LeftoverKernelCount - 1]);
				--m_LeftoverKernelCount;
			}

			m_bConvolutionFrameComplete = true;

#if defined(FFTL_ENABLE_PROFILING)
			m_timerConvolution.Accum();
#endif
		}
	};

	//	If this is 0, it means we've convolved everything we need to since ConvolveInitial
	if (m_bConvolutionFrameComplete || m_LeftoverKernelCount == 0)
		return;

	FFTL_ASSERT_MSG(startKernelIndex > 0 || pKernelArray_FD == nullptr, "Start kernel index of 1 or greater assumed, as ConvolveInitial would have completed buffer 0");
	FFTL_ASSERT_MSG(startKernelIndex < kernelArraySize || pKernelArray_FD == nullptr, "Cannot convolve past the array length");
	FFTL_ASSERT_MSG(endKernelIndex <= kernelArraySize, "endKernelIndex cannot be greater than kernelArraySize");

	if (m_bInputSignalHasData)
	{
		ConvolveLoopLambda(pKernelArray_FD, startKernelIndex, endKernelIndex);
		
		if (endKernelIndex >= kernelArraySize)
		{
			if (kernelArraySize < m_LeftoverKernelCount)
				TailConvolveLambda(endKernelIndex, m_LeftoverKernelCount);
			else
				m_bConvolutionFrameComplete = true;
		}
	}
	else
	{
		//	If we haven't processed data at the initial convolution, then we should just process the leftover kernels
		TailConvolveLambda(startKernelIndex, endKernelIndex);
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::Convolve(FixedArray_Aligned32<T, N>& fInOutput, const Kernel* pKernelArrayA_FD, size_t kernelArraySizeA, T fGainA, const Kernel* pKernelArrayB_FD, size_t kernelArraySizeB, T fGainB)
{
	const size_t maxNewKernelCount = Max(kernelArraySizeA, kernelArraySizeB);
	const size_t minNewKernelCount = Min(kernelArraySizeA, kernelArraySizeB);

	m_LeftoverKernelCount = Max(m_LeftoverKernelCount, maxNewKernelCount);

	bool bReturn = false;

	if (minNewKernelCount > 0)
	{
		FFTL_ASSERT(pKernelArrayA_FD != nullptr);
		FFTL_ASSERT(pKernelArrayB_FD != nullptr);

		//	Convert the time domain input to freq domain
		sm_fft::TransformForward_1stHalf(fInOutput, m_inputSignal_FD.r(), m_inputSignal_FD.i());

		//	Convolve the first segment, perform IFFT, and write to the output
		const Kernel& kernelA = pKernelArrayA_FD[0];
		const Kernel& kernelB = pKernelArrayB_FD[0];
		Kernel& curBuffer = m_AccumulationBuffer[0];

		//	Frequency domain convolution
		ConvolveFD(m_tempBufferA, m_inputSignal_FD, kernelA, kernelB, curBuffer, fGainA, fGainB);

		//	Convert the new frequency domain signal back to the time domain
		sm_fft::TransformInverse_ClobberInput(m_tempBufferA.r(), m_tempBufferA.i(), m_tempBufferB.t);

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		AddArrays(fInOutput, m_PrevTail, m_tempBufferB.r());

		//	Store the 2nd half of the IFFT buffer (reverb tail)
		MemCopy(m_PrevTail, m_tempBufferB.i());
	}
	else if (kernelArraySizeA > 0)
	{
		FFTL_ASSERT(pKernelArrayA_FD != nullptr);

		//	Convert the time domain input to freq domain
		sm_fft::TransformForward_1stHalf(fInOutput, m_inputSignal_FD.r(), m_inputSignal_FD.i());

		//	Convolve the first segment, perform IFFT, and write to the output
		const Kernel& kernelA = pKernelArrayA_FD[0];
		Kernel& curBuffer = m_AccumulationBuffer[0];

		//	Frequency domain convolution
		ConvolveFD(m_tempBufferA, m_inputSignal_FD, kernelA, curBuffer, fGainA);

		//	Convert the new frequency domain signal back to the time domain
		sm_fft::TransformInverse_ClobberInput(m_tempBufferA.r(), m_tempBufferA.i(), m_tempBufferB.t);

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		AddArrays(fInOutput, m_PrevTail, m_tempBufferB.r());

		//	Store the 2nd half of the IFFT buffer (reverb tail)
		MemCopy(m_PrevTail, m_tempBufferB.i());
	}
	else if (kernelArraySizeB > 0)
	{
		FFTL_ASSERT(pKernelArrayB_FD != nullptr);

		//	Convert the time domain input to freq domain
		sm_fft::TransformForward_1stHalf(fInOutput, m_inputSignal_FD.r(), m_inputSignal_FD.i());

		//	Convolve the first segment, perform IFFT, and write to the output
		const Kernel& kernelB = pKernelArrayB_FD[0];
		Kernel& curBuffer = m_AccumulationBuffer[0];

		//	Frequency domain convolution
		ConvolveFD(m_tempBufferA, m_inputSignal_FD, kernelB, curBuffer, fGainB);

		//	Convert the new frequency domain signal back to the time domain
		sm_fft::TransformInverse_ClobberInput(m_tempBufferA.r(), m_tempBufferA.i(), m_tempBufferB.t);

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		AddArrays(fInOutput, m_PrevTail, m_tempBufferB.r());

		//	Store the 2nd half of the IFFT buffer (reverb tail)
		MemCopy(m_PrevTail, m_tempBufferB.i());
	}
	else if (m_LeftoverKernelCount > 0)
	{
		//	If nothing to convolve anymore, fill in the previous convolution segments
		//	No convolution is necessary here because the input is effectively zero
		m_tempBufferA = m_AccumulationBuffer[0];

		//	Convert the new frequency domain signal back to the time domain
		sm_fft::TransformInverse_ClobberInput(m_tempBufferA.r(), m_tempBufferA.i(), m_tempBufferB.t);

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		AddArrays(fInOutput, m_PrevTail, m_tempBufferB.r());

		//	Store the 2nd half of the IFFT buffer (reverb tail)
		MemCopy(m_PrevTail, m_tempBufferB.i());

		//	Indicate that the output buffer was filled
		bReturn = true;
	}
	else
	{
		MemZero(fInOutput);
	}

	//	Perform short convolutions on the remaining kernels, accumulating everything as necessary
	for (size_t k = 1; k < minNewKernelCount; ++k)
	{
		const Kernel& kernelA = pKernelArrayA_FD[k];
		const Kernel& kernelB = pKernelArrayB_FD[k];
		Kernel& curBuffer = m_AccumulationBuffer[k];
		Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

		//	Frequency domain convolution
		ConvolveFD(prvBuffer, m_inputSignal_FD, kernelA, kernelB, curBuffer, fGainA, fGainB);
	}

	if (kernelArraySizeA > kernelArraySizeB)
	{
		for (size_t k = Max(minNewKernelCount, 1u); k < maxNewKernelCount; ++k)
		{
			const Kernel& kernelA = pKernelArrayA_FD[k];
			Kernel& curBuffer = m_AccumulationBuffer[k];
			Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

			//	Frequency domain convolution
			ConvolveFD(prvBuffer, m_inputSignal_FD, kernelA, curBuffer, fGainA);
		}
	}
	else
	{
		for (size_t k = Max(minNewKernelCount, 1u); k < maxNewKernelCount; ++k)
		{
			const Kernel& kernelB = pKernelArrayB_FD[k];
			Kernel& curBuffer = m_AccumulationBuffer[k];
			Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

			//	Frequency domain convolution
			ConvolveFD(prvBuffer, m_inputSignal_FD, kernelB, curBuffer, fGainB);
		}
	}

	//	Finish up the previous decay if we've switched to a shorter kernel
	for (size_t k = Max(maxNewKernelCount, 1u); k < m_LeftoverKernelCount; ++k)
	{
		Kernel& curBuffer = m_AccumulationBuffer[k];
		Kernel& prvBuffer = m_AccumulationBuffer[k - 1];

		prvBuffer = curBuffer;
		MemZero(curBuffer);
	}

	if (m_LeftoverKernelCount > maxNewKernelCount)
		--m_LeftoverKernelCount;
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY)
{
	//	Cache dc and Nyquist bins
	const T dc = inX.r()[0] * inY.r()[0];
	const T nq = inX.i()[0] * inY.i()[0];

	//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
	FFTL_IF_CONSTEXPR (std::is_same<T, f32>::value)
	{
		for (uint n = 0; n < N; n += 8)
		{
			const f32_8 xR = f32_8::LoadA(inX.r() + n);
			const f32_8 xI = f32_8::LoadA(inX.i() + n);
			const f32_8 yR = f32_8::LoadA(inY.r() + n);
			const f32_8 yI = f32_8::LoadA(inY.i() + n);

			const f32_8 rR = AddMul(xI * yI, xR, yR);
			const f32_8 rI = AddMul(xI * yR, xR, yI);

//			const f32_8 rR = (xR * yR - xI * yI);
//			const f32_8 rI = (xR * yI + xI * yR);
			rR.StoreA(output.r() + n);
			rI.StoreA(output.i() + n);
		}

		output.r()[0] = dc;
		output.i()[0] = nq;
	}
	else
	{
		output.r()[0] = dc;
		output.i()[0] = nq;

		for (uint n = 1; n < N; n += 1)
		{
			const T& xR = inX.r()[n];
			const T& xI = inX.i()[n];
			const T& yR = inY.r()[n];
			const T& yI = inY.i()[n];

			const T rR = AddMul(xI * yI, xR, yR);
			const T rI = AddMul(xI * yR, xR, yI);

//			const T rR = xR * yR - xI * yI;
//			const T rI = xR * yI + xI * yR;
			output.r()[n] = rR;
			output.i()[n] = rI;
		}
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inW)
{
	//	Cache dc and Nyquist bins
	const T dc = AddMul(inW.r()[0], inX.r()[0], inY.r()[0]);
	const T nq = AddMul(inW.i()[0], inX.i()[0], inY.i()[0]);

	//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
	FFTL_IF_CONSTEXPR (std::is_same<T, f32>::value)
	{
		for (uint n = 0; n < N; n += 8)
		{
			const f32_8 xR = f32_8::LoadA(inX.r() + n);
			const f32_8 xI = f32_8::LoadA(inX.i() + n);
			const f32_8 yR = f32_8::LoadA(inY.r() + n);
			const f32_8 yI = f32_8::LoadA(inY.i() + n);
			const f32_8 wR = f32_8::LoadA(inW.r() + n);
			const f32_8 wI = f32_8::LoadA(inW.i() + n);

			const f32_8 rR = AddMul(SubMul(wR, xI, yI), xR, yR);
			const f32_8 rI = AddMul(AddMul(wI, xI, yR), xR, yI);

//			const f32_8 rR = (xR * yR - xI * yI + wR);
//			const f32_8 rI = (xR * yI + xI * yR + wI);
			rR.StoreA(output.r() + n);
			rI.StoreA(output.i() + n);
		}

		output.r()[0] = dc;
		output.i()[0] = nq;
	}
	else
	{
		output.r()[0] = dc;
		output.i()[0] = nq;

		for (uint n = 1; n < N; n += 1)
		{
			const T& xR = inX.r()[n];
			const T& xI = inX.i()[n];
			const T& yR = inY.r()[n];
			const T& yI = inY.i()[n];
			const T& wR = inW.r()[n];
			const T& wI = inW.i()[n];

			const T rR = AddMul(SubMul(wR, xI, yI), xR, yR);
			const T rI = AddMul(AddMul(wI, xI, yR), xR, yI);

//			const T rR = xR * yR - xI * yI + wR;
//			const T rI = xR * yI + xI * yR + wI;
			output.r()[n] = rR;
			output.i()[n] = rI;
		}
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inW, T fGainY)
{
	//	Cache dc and Nyquist bins
	const T dc = AddMul(inW.r()[0], inX.r()[0], inY.r()[0] * fGainY);
	const T nq = AddMul(inW.i()[0], inX.i()[0], inY.i()[0] * fGainY);

	//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
	FFTL_IF_CONSTEXPR (std::is_same<T, f32>::value)
	{
		const f32_8 vGainY = f32_8::Splat(fGainY);

		for (uint n = 0; n < N; n += 8)
		{
			const f32_8 xR = f32_8::LoadA(inX.r() + n);
			const f32_8 xI = f32_8::LoadA(inX.i() + n);
			const f32_8 yR = f32_8::LoadA(inY.r() + n);
			const f32_8 yI = f32_8::LoadA(inY.i() + n);
			const f32_8 wR = f32_8::LoadA(inW.r() + n);
			const f32_8 wI = f32_8::LoadA(inW.i() + n);

			const f32_8 aR = yR * vGainY;
			const f32_8 aI = yI * vGainY;

			const f32_8 rR = AddMul(SubMul(wR, xI, aI), xR, aR);
			const f32_8 rI = AddMul(AddMul(wI, xI, aR), xR, aI);

			rR.StoreA(output.r() + n);
			rI.StoreA(output.i() + n);
		}

		output.r()[0] = dc;
		output.i()[0] = nq;
	}
	else
	{
		output.r()[0] = dc;
		output.i()[0] = nq;

		for (uint n = 1; n < N; n += 1)
		{
			const T& xR = inX.r()[n];
			const T& xI = inX.i()[n];
			const T& yR = inY.r()[n];
			const T& yI = inY.i()[n];
			const T& wR = inW.r()[n];
			const T& wI = inW.i()[n];

			const T aR = yR * fGainY;
			const T aI = yI * fGainY;

			const T rR = AddMul(SubMul(wR, xI, aI), xR, aR);
			const T rI = AddMul(AddMul(wI, xI, aR), xR, aI);

			output.r()[n] = rR;
			output.i()[n] = rI;
		}
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::ConvolveFD(Kernel& output, const Kernel& inX, const Kernel& inY, const Kernel& inZ, const Kernel& inW, T fGainY, T fGainZ)
{
	//	Cache dc and Nyquist bins
	const T dc = AddMul(inW.r()[0], inX.r()[0], AddMul(inY.r()[0] * fGainY, inZ.r()[0], fGainZ));
	const T nq = AddMul(inW.i()[0], inX.i()[0], AddMul(inY.i()[0] * fGainY, inZ.i()[0], fGainZ));

	//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
	FFTL_IF_CONSTEXPR (std::is_same<T, f32>::value)
	{
		const f32_8 vGainY = f32_8::Splat(fGainY);
		const f32_8 vGainZ = f32_8::Splat(fGainZ);

		for (uint n = 0; n < N; n += 8)
		{
			const f32_8 xR = f32_8::LoadA(inX.r() + n);
			const f32_8 xI = f32_8::LoadA(inX.i() + n);
			const f32_8 yR = f32_8::LoadA(inY.r() + n);
			const f32_8 yI = f32_8::LoadA(inY.i() + n);
			const f32_8 zR = f32_8::LoadA(inZ.r() + n);
			const f32_8 zI = f32_8::LoadA(inZ.i() + n);
			const f32_8 wR = f32_8::LoadA(inW.r() + n);
			const f32_8 wI = f32_8::LoadA(inW.i() + n);

			const f32_8 aR = AddMul(yR * vGainY, zR, vGainZ);
			const f32_8 aI = AddMul(yI * vGainY, zI, vGainZ);

			const f32_8 rR = AddMul(SubMul(wR, xI, aI), xR, aR);
			const f32_8 rI = AddMul(AddMul(wI, xI, aR), xR, aI);

			rR.StoreA(output.r() + n);
			rI.StoreA(output.i() + n);
		}

		output.r()[0] = dc;
		output.i()[0] = nq;
	}
	else
	{
		output.r()[0] = dc;
		output.i()[0] = nq;

		for (uint n = 1; n < N; n += 1)
		{
			const T& xR = inX.r()[n];
			const T& xI = inX.i()[n];
			const T& yR = inY.r()[n];
			const T& yI = inY.i()[n];
			const T& zR = inZ.r()[n];
			const T& zI = inZ.i()[n];
			const T& wR = inW.r()[n];
			const T& wI = inW.i()[n];

			const T aR = AddMul(yR * fGainY, zR, fGainZ);
			const T aI = AddMul(yI * fGainY, zI, fGainZ);

			const T rR = AddMul(SubMul(wR, xI, aI), xR, aR);
			const T rI = AddMul(AddMul(wI, xI, aR), xR, aI);

			output.r()[n] = rR;
			output.i()[n] = rI;
		}
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::AddArrays(FixedArray_Aligned32<T, N>& output, const FixedArray_Aligned32<T, N>& inA, const FixedArray_Aligned32<T, N>& inB)
{
	//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
	FFTL_IF_CONSTEXPR (std::is_same<T, f32>::value)
	{
		for (uint n = 0; n < N; n += 8)
		{
			const f32_8 a = f32_8::LoadA(inA + n);
			const f32_8 b = f32_8::LoadA(inB + n);
			const f32_8 val = a + b;
			val.StoreA(output + n);
		}
	}
	else
	{
		for (uint n = 0; n < N; n += 1)
		{
			const T val = inA[n] + inB[n];
			output[n] = val;
		}
	}
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
uint Convolver<M, T_MAX_KERNELS, T, T_Twiddle>::InitKernel(Kernel* pKernelOutput_FD, const T* pKernelInput_TD, size_t kernelLength)
{
	//	Determine the number of kernels we need
	const uint kernelCount = safestatic_cast<uint>(AlignForward<N>(kernelLength) / N);

	FFTL_ASSERT(kernelCount > 0);
	FFTL_ASSERT(kernelCount <= T_MAX_KERNELS);

	size_t samplesRemaining = kernelLength;

	MemZero(pKernelOutput_FD, kernelCount);

	for (uint i = 0; i < kernelCount - 1; ++i)
	{
		Kernel& kernel = pKernelOutput_FD[i];

		//	Convert to frequency domain, and store
		sm_fft::TransformForward_1stHalf(*reinterpret_cast<const FixedArray<T, N>*>(pKernelInput_TD + i * N), kernel.r(), kernel.i());

		samplesRemaining -= N;
	}

	{
		FixedArray_Aligned32<T, N> temp;
		MemZero(temp);
		MemCopy(temp.data(), pKernelInput_TD + N * (kernelCount - 1), samplesRemaining);

		Kernel& kernel = pKernelOutput_FD[kernelCount - 1];

		//	Convert to frequency domain, and store
		sm_fft::TransformForward_1stHalf(temp, kernel.r(), kernel.i());
	}

	return kernelCount;
}



template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
uint ConvolverV<M, T_MAX_KERNELS, T, T_Twiddle>::InitKernel(T* pKernelOutput_FD, const T* pKernelInput_TD, size_t kernelLength) const
{
	return BaseConvolver::InitKernel(reinterpret_cast<typename BaseConvolver::Kernel*>(pKernelOutput_FD), pKernelInput_TD, kernelLength);
}

template <uint M, size_t T_MAX_KERNELS, typename T, typename T_Twiddle>
void ConvolverV<M, T_MAX_KERNELS, T, T_Twiddle>::Convolve(T* fInOutput, const T* pKernelArray_FD, size_t kernelArraySize)
{
	return BaseConvolver::Convolve(*reinterpret_cast<FixedArray_Aligned32<T, BaseConvolver::N>*>(fInOutput), reinterpret_cast<const typename BaseConvolver::Kernel*>(pKernelArray_FD), kernelArraySize);
}





template <typename T, uint T_N, uint T_KERNEL_LENGTH>
void Convolver_Slow<T, T_N, T_KERNEL_LENGTH>::Convolve(const FixedArray<T, T_N>& fInput, FixedArray<T, T_N>& fOutput)
{
	memmove(&m_AccumulationBuffer, &m_AccumulationBuffer[T_N], sizeof(T)*T_KERNEL_LENGTH);
	MemZero(&m_AccumulationBuffer[T_KERNEL_LENGTH], T_N);

	for (uint n = 0; n < T_N; ++n)
	{
		const T fInThis = fInput[n];
		for (uint m = 0; m < T_KERNEL_LENGTH; ++m)
		{
			m_AccumulationBuffer[n + m] += fInThis * m_Kernel[m];
		}
	}

	MemCopy(fOutput, m_AccumulationBuffer);
}



} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // _FFTL_FFT_INL
