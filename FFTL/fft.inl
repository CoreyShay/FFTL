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

#ifndef _FFTL_FFT_INL
#define _FFTL_FFT_INL

#include "defs.h"
#include "ComplexNumber.h"
#include "Math.h"

#include <string.h> // memcpy

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




template <uint _M, typename T, typename T_Twiddle>
FFT_Base<_M, T, T_Twiddle>::FFT_Base()
{
	//	Precomputations
	this->ComputeBitReversalIndices();
	this->ComputeTwiddleFactors();

	//	Debug
	this->PrintSetupInfo();

#if FFTL_STAGE_TIMERS
	memset(&m_StageTimers[0], 0, sizeof(m_StageTimers));
	m_PreProcessTimer.QuadPart = 0;
	m_PostProcessTimer.QuadPart = 0;
#endif
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::ComputeBitReversalIndices()
{
	for (T_BR i = 0; i < _N; ++i)
	{
		T_BR uReverseIndex = i;
		for (T_BR uRightIndex = 0, uLeftIndex = _M-1; uRightIndex < uLeftIndex; ++uRightIndex, --uLeftIndex)
		{
			const uint uShiftAmount = uLeftIndex - uRightIndex;
			uReverseIndex &= ~( (1<<uRightIndex) | (1<<uLeftIndex) );

			T_BR bitRight = (1<<uRightIndex) & i;
			T_BR bitLeft = (1<<uLeftIndex) & i;
			bitRight <<= uShiftAmount;
			bitLeft >>= uShiftAmount;
			uReverseIndex |= bitLeft | bitRight;
		}

		m_BitReverseIndices[i] = uReverseIndex;
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::ComputeTwiddleFactors()
{
	FFTL_StaticAssert(_M >= 2);
	FFTL_StaticAssert(_M <= 16);

	m_TwiddleFactorsR[_N/2] = 1;
	m_TwiddleFactorsI[_N/2] = 0;
	for (uint n = 1; n < _N/2; ++n)
	{
		//  Values for Sin(pi) and Cos(pi/2) come out to be not exactly zero, so we can preempt that here
		if (n==_N/4)
		{
			m_TwiddleFactorsR[n+_N/2] = 0;
			m_TwiddleFactorsI[n+_N/2] = -1;
		}
		else
		{
			const double fAngle = -2 * pi_64 * (double)n / (double)_N;
			m_TwiddleFactorsR[n+_N/2] = (T_Twiddle)Cos(fAngle);
			m_TwiddleFactorsI[n+_N/2] = (T_Twiddle)Sin(fAngle);
		}
	}

	//	The 0 index twiddles won't be used because we will actually start at 1, but why not fill them in with something?
	m_TwiddleFactorsR[0] = 1;
	m_TwiddleFactorsI[0] = 0;

	uint uTwiddleToIndex = 1;
	//	Loop for each stage
	for (uint nStage=0, nStageExp=2, uTwiddleStep=_N/2; nStage < _M-1; ++nStage, nStageExp<<=1, uTwiddleStep>>=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; ++nSubDFT, ++uTwiddleToIndex)
		{
			const uint uTwiddleFromIndex = nSubDFT*uTwiddleStep + _N/2;
			m_TwiddleFactorsR[uTwiddleToIndex] = m_TwiddleFactorsR[uTwiddleFromIndex];
			m_TwiddleFactorsI[uTwiddleToIndex] = m_TwiddleFactorsI[uTwiddleFromIndex];
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::ApplyWindow(FixedArray<T,_N>& fInOut, const WindowCoefficients& coeff) const
{
	for (uint n = 0; n < _N; ++n)
		fInOut[n] *= coeff.m_C[n];
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::ApplyWindow(FixedArray<cxT,_N>& cxInOut, const WindowCoefficients& coeff) const
{
	for (uint n = 0; n < _N; ++n)
		cxInOut[n] *= coeff.m_C[n];
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_DIT(const cxT& cxU, cxT* __restrict pcxCurr, cxT* __restrict pcxNext)
{
	const cxT W = *pcxNext * cxU;
	*pcxNext = *pcxCurr - W;
	*pcxCurr += W;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_Unity(cxT* __restrict pcxCurr, cxT* __restrict pcxNext)
{
	const cxT W = *pcxNext;
	*pcxNext = *pcxCurr - W;
	*pcxCurr += W;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_DIF(const cxT& cxU, cxT* __restrict pcxCurr, cxT* __restrict pcxNext)
{
	const cxT W = *pcxCurr - *pcxNext;
	*pcxCurr += *pcxNext;
	*pcxNext = W * cxU;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_DIT(const T& fUReal, const T& fUImag, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	const T Wr = *pfNextR*fUReal - *pfNextI*fUImag;
	const T Wi = *pfNextR*fUImag + *pfNextI*fUReal;

	*pfNextR = *pfCurR - Wr;
	*pfNextI = *pfCurI - Wi;

	*pfCurR += Wr;
	*pfCurI += Wi;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_Unity(T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	const T Wr = *pfNextR;
	const T Wi = *pfNextI;

	*pfNextR = *pfCurR - Wr;
	*pfNextI = *pfCurI - Wi;

	*pfCurR += Wr;
	*pfCurI += Wi;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_Unity(const T& fCurR, const T& fCurI, const T& fNextR, const T& fNextI, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	*pfCurR = fCurR + fNextR;
	*pfCurI = fCurI + fNextI;

	*pfNextR = fCurR - fNextR;
	*pfNextI = fCurI - fNextI;
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_FORCEINLINE void FFT_Base<_M, T, T_Twiddle>::CalculateButterfly_DIF(const T& fUReal, const T& fUImag, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	const T Wr = *pfCurR - *pfNextR;
	const T Wi = *pfCurI - *pfNextI;

	*pfCurR += *pfNextR;
	*pfCurI += *pfNextI;

	*pfNextR = Wr*fUReal - Wi*fUImag;
	*pfNextI = Wr*fUImag + Wi*fUReal;
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::PrintSetupInfo() const
{
#if FFTL_PRINTSETUP
	const T_Twiddle _2Pi = (T_Twiddle)2 * (T_Twiddle)pi_64;
	const T_Twiddle _1_div_N = (T_Twiddle)1 / (T_Twiddle)_N;
	const T_Twiddle _2Pi_div_N = _2Pi * _1_div_N;
	const uint M = _M;
	const uint N = _N;

	FFTL_Printf("N = %u\n", N);
	FFTL_Printf("N_2 = %u\n", _N_div_2);
	FFTL_Printf("M = %u\n", M);
	FFTL_Printf("twoPi = %f\n", _2Pi);
	FFTL_Printf("one_N = %f\n", _1_div_N);
	FFTL_Printf("twoPiOne_N = %f\n", _2Pi_div_N);

#if 0
	FFTL_Printf("M twiddles:\n");
	for (uint i = 0; i < _M; ++i)
	{
		FFTL_Printf("[%f, %f]; ", m_TwiddleFactors[i].r, m_TwiddleFactors[i].i);
	}

	FFTL_Printf("\nN twiddles:\n");
	//	Loop for each stage
	for (uint nStage=0, nStageExp=2; nStage < _M; ++nStage, nStageExp<<=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	Lookup the twiddle factor for phase angle
		const cxT& cxS = m_TwiddleFactors[nStage];
		cxT cxU(1, 0);

		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; ++nSubDFT)
		{
			FFTL_Printf("[%f, %f]; ", cxU.r, cxU.i);
			cxU *= cxS;
		}
		FFTL_Printf("\n\n");
	}
#endif
#endif // FFTL_PRINTSETUP
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::PrintTimerInfo(uint iterationCount) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laFreq;
	QueryPerformanceFrequency(&laFreq);
	const double fInvUS = 1000000.0 / laFreq.QuadPart;

	if (m_PreProcessTimer.QuadPart != 0)
	{
		FFTL_Printf("\tPreProcess: %f us\n", (double)m_PreProcessTimer.QuadPart * fInvUS / iterationCount);
		m_PreProcessTimer.QuadPart = 0;
	}

	for (int m = 0; m < _M; ++m)
	{
		if (m_StageTimers[m].QuadPart != 0)
		{
			FFTL_Printf("\tStage %i: %f us\n", m, (double)m_StageTimers[m].QuadPart * fInvUS / iterationCount);
			m_StageTimers[m].QuadPart = 0;
		}
	}

	if (m_PostProcessTimer.QuadPart != 0)
	{
		FFTL_Printf("\tPostProcess: %f us\n", (double)m_PostProcessTimer.QuadPart * fInvUS / iterationCount);
		m_PostProcessTimer.QuadPart = 0;
	}
#else
	(void)iterationCount;
#endif
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::WindowCoefficients::Compute(enFftWindowType windowType, uint uWidth)
{
	//	Zero out anything that might be lurking
	memset(m_C.data, 0, sizeof(m_C));

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
			const uint uWidth_div_2 = uWidth>>1;
			const T_Twiddle fWidth_div_2 = (T_Twiddle)uWidth_div_2;
			for (uint n = 0; n < uWidth_div_2; ++n)
				m_C[n] = m_C[uWidth-n-1] = (T_Twiddle)n/(fWidth_div_2-1);
		}
		break;
	case kWindowHanning:
		{
			for (uint n = 0; n < uWidth; ++n)
				m_C[n] = T_Twiddle(0.50 - 0.50*Cos(2.0*pi_64*n/(uWidth-1)) );
		}
		break;
	case kWindowHamming:
		{
			for (uint n = 0; n < uWidth; ++n)
				m_C[n] = T_Twiddle(0.54 - 0.46*Cos(2.0*pi_64*n/(uWidth-1)) );
		}
		break;
	case kWindowBlackman:
		{
			for (uint n = 0; n < uWidth; ++n)
				m_C[n] = T_Twiddle(0.42 - 0.50*Cos(2.0*pi_64*n/(uWidth-1)) + 0.08*Cos(4.0*pi_64*n/(uWidth-1)) );
		}
		break;
	case kWindowVorbis:
		{
			for (uint n = 0; n < uWidth; ++n)
				m_C[n] = T_Twiddle( Sin(0.5*pi_64 * Square(Sin(pi_64/(uWidth)*(n+0.5))) ) );
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
FFTL_FORCEINLINE f32_4 LoadTo<f32_4, float>(const float* a)
{
	return f32_4::Splat4(a);
}
template<>
FFTL_FORCEINLINE f32_8 LoadTo<f32_8, float>(const float* a)
{
	return f32_8::Splat8(a);
}

template<typename T_TO, typename T_FROM>
FFTL_FORCEINLINE T_TO ConvertTo(const T_FROM& a)
{
	return static_cast<T_TO>(a);
}
template<>
FFTL_FORCEINLINE f32_4 ConvertTo<f32_4, float>(const float& a)
{
	return f32_4::Splat4(&a);
}
template<>
FFTL_FORCEINLINE f32_8 ConvertTo<f32_8, float>(const float& a)
{
	return f32_8::Splat8(&a);
}







template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::Transform_Main_DIT(FixedArray<cxT,_N>& cxOutput) const
{
	uint uTwiddleIndex = 1;

	//	Loop for each stage
	for (uint nStage=0, nStageExp=2; nStage < _M; ++nStage, nStageExp<<=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	The first iteration of every stage will have a 1,0 twiddle factor, so we can save a few cycles
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;
			//cxT* __restrict pcxCurr = &cxOutput[uButterfly];
			//cxT* __restrict pcxNext = &cxOutput[uButterflyNext];
			//CalculateButterfly_DIT(pcxCurr, pcxNext);

			T* __restrict pfCurR = &cxOutput[uButterfly].r;
			T* __restrict pfCurI = &cxOutput[uButterfly].i;
			T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
			T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
			this->CalculateButterfly_DIT(pfCurR, pfCurI, pfNextR, pfNextI);
		}

		//	Loop for each sub DFT
		++uTwiddleIndex;
		for (uint nSubDFT = 1; nSubDFT < nStageExp_2; ++nSubDFT, ++uTwiddleIndex)
		{
			const T uR = LoadTo<T>(this->GetTwiddleRealPtr(uTwiddleIndex));
			const T uI = LoadTo<T>(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each butterfly
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				//cxT* __restrict pcxCurr = &cxOutput[uButterfly];
				//cxT* __restrict pcxNext = &cxOutput[uButterflyNext];
				//CalculateButterfly_DIT(cxU, pcxCurr, pcxNext);

				T* __restrict pfCurR = &cxOutput[uButterfly].r;
				T* __restrict pfCurI = &cxOutput[uButterfly].i;
				T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
				T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
				this->CalculateButterfly_DIT(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformInverse_InPlace_DIT(FixedArray<cxT,_N>& cxOutput) const
{
	uint uTwiddleIndex = 1;

	//	Loop for each stage
	for (uint nStage=0, nStageExp=2; nStage < _M; ++nStage, nStageExp<<=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	The first iteration of every stage will have a 1,0 twiddle factor, so we can save a few cycles
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;

			T* __restrict pfCurR = &cxOutput[uButterfly].r;
			T* __restrict pfCurI = &cxOutput[uButterfly].i;
			T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
			T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
			this->CalculateButterfly_DIT(pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
		}

		//	Loop for each sub DFT
		++uTwiddleIndex;
		for (uint nSubDFT = 1; nSubDFT < nStageExp_2; ++nSubDFT, ++uTwiddleIndex)
		{
			const T uR = LoadTo<T>(this->GetTwiddleRealPtr(uTwiddleIndex));
			const T uI = LoadTo<T>(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each butterfly
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;

				T* __restrict pfCurR = &cxOutput[uButterfly].r;
				T* __restrict pfCurI = &cxOutput[uButterfly].i;
				T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
				T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
				this->CalculateButterfly_DIT(uR, uI, pfCurI, pfCurR, pfNextI, pfNextR); // reverse real and imaginary inputs
			}
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::Transform_Main_DIF(FixedArray<cxT,_N>& cxOutput) const
{
	uint uTwiddleIndex = _N - 1;

	//	Loop for each stage
	for (int nStage=_M-1, nStageExp=_N; nStage >= 0; --nStage, nStageExp>>=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-1; nSubDFT >= 1; --nSubDFT, --uTwiddleIndex)
		{
			const T uR = LoadTo<T>(this->GetTwiddleRealPtr(uTwiddleIndex));
			const T uI = LoadTo<T>(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each butterfly
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;

				T* __restrict pfCurR = &cxOutput[uButterfly].r;
				T* __restrict pfCurI = &cxOutput[uButterfly].i;
				T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
				T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
				this->CalculateButterfly_DIF(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}

		//	The first iteration of every stage (or in this case, the last, going backwards) will have a 1,0 twiddle factor, so we can save a few cycles
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;

			T* __restrict pfCurR = &cxOutput[uButterfly].r;
			T* __restrict pfCurI = &cxOutput[uButterfly].i;
			T* __restrict pfNextR = &cxOutput[uButterflyNext].r;
			T* __restrict pfNextI = &cxOutput[uButterflyNext].i;
			this->CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
		}

		--uTwiddleIndex;
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::Transform_Main_DIT(FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI, bool bSkipStage0) const
{
	uint uTwiddleIndex		=	bSkipStage0 ? 2 : 1;
	uint nStage				=	bSkipStage0 ? 1 : 0;
	uint nStageExp			=	bSkipStage0 ? 4 : 2;

	//	Loop for each stage
	for (; nStage < _M; ++nStage, nStageExp<<=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	The first iteration of every stage will have a 1,0 twiddle factor, so we can save a few cycles
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;
			T* __restrict pfCurR = fOutR + uButterfly;
			T* __restrict pfCurI = fOutI + uButterfly;
			T* __restrict pfNextR = fOutR + uButterflyNext;
			T* __restrict pfNextI = fOutI + uButterflyNext;
			this->CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
		}

		//	Loop for each sub DFT
		++uTwiddleIndex;
		for (uint nSubDFT = 1; nSubDFT < nStageExp_2; ++nSubDFT, ++uTwiddleIndex)
		{
			const T uR = LoadTo<T>(this->GetTwiddleRealPtr(uTwiddleIndex));
			const T uI = LoadTo<T>(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each butterfly
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = fOutR + uButterfly;
				T* __restrict pfCurI = fOutI + uButterfly;
				T* __restrict pfNextR = fOutR + uButterflyNext;
				T* __restrict pfNextI = fOutI + uButterflyNext;
				this->CalculateButterfly_DIT(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::Transform_Main_DIF(FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	uint uTwiddleIndex = _N - 1;

	//	Loop for each stage
	for (int nStage=_M-1, nStageExp=_N; nStage >= 0; --nStage, nStageExp>>=1)
	{
		const uint nStageExp_2 = nStageExp>>1;

		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-1; nSubDFT >= 1; --nSubDFT, --uTwiddleIndex)
		{
			const T uR = LoadTo<T>(this->GetTwiddleRealPtr(uTwiddleIndex));
			const T uI = LoadTo<T>(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each butterfly
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;

				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				this->CalculateButterfly_DIF(uR, uI, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}

		//	The first iteration of every stage (or in this case, the last, going backwards) will have a 1,0 twiddle factor, so we can save a few cycles
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;

			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			this->CalculateButterfly_Unity(pfCurR, pfCurI, pfNextR, pfNextI);
		}

		--uTwiddleIndex;
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformForward(const FixedArray<cxT,_N>& cxInput, FixedArray<cxT,_N>& cxOutput) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < _N; n += 2)
	{
		const uint nR0 = m_BitReverseIndices[n+0];
		const uint nR1 = m_BitReverseIndices[n+1];

		T* __restrict pfCurR = &cxOutput[n].r;
		T* __restrict pfCurI = &cxOutput[n].i;
		T* __restrict pfNextR = &cxOutput[n+1].r;
		T* __restrict pfNextI = &cxOutput[n+1].i;
		this->CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, cxInput[nR1].r, cxInput[nR1].i, pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	m_PreProcessTimer += laEnd - laStart;
#endif

	Transform_Main_DIT(cxOutput);
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformForward(const FixedArray<T,_N>& fInR, const FixedArray<T,_N>& fInI, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < _N; n += 2)
	{
		const uint nR0 = m_BitReverseIndices[n+0];
		const uint nR1 = m_BitReverseIndices[n+1];

		T* __restrict pfCurR = fOutR + n;
		T* __restrict pfCurI = fOutI + n;
		T* __restrict pfNextR = fOutR + n+1;
		T* __restrict pfNextI = fOutI + n+1;
		this->CalculateButterfly_Unity(fInR[nR0], fInI[nR0], fInR[nR1], fInI[nR1], pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	m_PreProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif

	//	Invoke the main transform function
	Transform_Main_DIT(fOutR, fOutI);
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformForward(const FixedArray<cxT,_N>& cxInput, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Copy the input to the output with the bit reversal indices, simultaneously completing the first stage of _M stages.
	for (uint n = 0; n < _N; n += 2)
	{
		const uint nR0 = m_BitReverseIndices[n+0];
		const uint nR1 = m_BitReverseIndices[n+1];

		T* __restrict pfCurR = fOutR + n;
		T* __restrict pfCurI = fOutI + n;
		T* __restrict pfNextR = fOutR + n+1;
		T* __restrict pfNextI = fOutI + n+1;
		this->CalculateButterfly_Unity(cxInput[nR0].r, cxInput[nR0].i, cxInput[nR1].r, cxInput[nR1].i, pfCurR, pfCurI, pfNextR, pfNextI);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	m_PreProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif

	//	Invoke the main transform function
	Transform_Main_DIT(fOutR, fOutI, true);
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformForwardApplyWindow(const FixedArray<cxT,_N>& cxInput, FixedArray<cxT,_N>& cxOutput, const WindowCoefficients& coeff) const
{
	//	Copy the input to the output with the bit reversal indices, and apply the window function
	for (uint n = 0; n < _N; ++n)
	{
		const uint nR = m_BitReverseIndices[n];
		cxOutput[n] = cxInput[nR] * coeff.m_C[nR];
	}

	//	Invoke the main transform function
	Transform_Main_DIT(cxOutput);
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Base<_M, T, T_Twiddle>::TransformInverse(const FixedArray<cxT,_N>& cxInput, FixedArray<cxT,_N>& cxOutput, bool bApplyBitReverse) const
{
	const T _1_div_N = ConvertTo<T>(1 / (T_Twiddle)_N);

	if (bApplyBitReverse)
	{
		//	Copy the input to the output with the bit reversal indices, and flip the imaginary component sign
		for (uint n = 0; n < _N; ++n)
		{
			const uint nR = this->m_BitReverseIndices[n];
			const cxT& rInput = cxInput[nR];
			cxOutput[n].Set(rInput.r, -rInput.i);
		}
	}
	else
	{
		for (uint n = 0; n < _N; ++n)
		{
			const cxT& rInput = cxInput[n];
			cxOutput[n].Set(rInput.r, -rInput.i);
		}
	}

	//	Invoke the main transform function
	Transform_Main_DIT(cxOutput);

	//	Divide everything by N and flip the sign of the imaginary output again
	for (uint n = 0; n < _N; ++n)
	{
		cxT& rOutput = cxOutput[n];
		rOutput.r *= +_1_div_N;
		rOutput.i *= -_1_div_N;
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::TransformForward_Slow(const FixedArray<T,_N>& fInput, FixedArray<cxT,_N>& cxOutput) const
{
	const T _2Pi = (T)2 * (T)pi_64;
	const T _2Pi_div_N = _2Pi / (T)_N;

	for (uint m = 0; m < _N; ++m)
	{
		const T fm = (T)m;
		cxOutput[m] = 0;
		for (uint n = 0; n < _N; ++n)
		{
			const T fAngle = _2Pi_div_N * (T)n * fm;
			const cxT cx(Cos(fAngle), -Sin(fAngle));
			cxOutput[m] += cx * fInput[n];
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::TransformForward_Slow(const FixedArray<cxT,_N>& cxInput, FixedArray<cxT,_N>& cxOutput) const
{
	const T _2Pi = (T)2 * (T)pi_64;
	const T _2Pi_div_N = _2Pi / (T)_N;

	for (uint m = 0; m < _N; ++m)
	{
		const T fm = (T)m;
		cxOutput[m] = 0;
		for (uint n = 0; n < _N; ++n)
		{
			const T fAngle = _2Pi_div_N * (T)n * fm;
			const cxT cx(Cos(fAngle), -Sin(fAngle));
			cxOutput[m] += cx * cxInput[n];
		}
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::TransformInverse_Slow(const FixedArray<cxT,_N>& cxInput, FixedArray<T,_N>& fOutTime) const
{
	const T _2Pi = (T)2 * (T)pi_64;
	const T _1_div_N = (T)1 / (T)_N;
	const T _2Pi_div_N = _2Pi * _1_div_N;

	{
		T fTime = 0;
		for (uint m = 0; m < _N; ++m)
		{
			fTime += cxInput[m].r;
		}
		fOutTime[0] = _1_div_N * fTime;
	}

	for (uint n = 1; n < _N; ++n)
	{
		const T fn = (T)n;
		T fTime = 0;
		for (uint m = 0; m < _N; ++m)
		{
			const T fm = (T)m;
			const T fAngle = _2Pi_div_N*fn*fm;
			fTime += (cxInput[m].r * Cos(fAngle)) - (cxInput[m].i * Sin(fAngle));
		}
		fOutTime[_N-n] = _1_div_N * fTime;
	}
}

template <uint _M, typename T, typename T_Twiddle>
void FFT_Base<_M, T, T_Twiddle>::TransformInverse_Slow(const FixedArray<cxT,_N>& cxInput, FixedArray<cxT,_N>& cxOutput) const
{
	const T _2Pi = (T)2 * (T)pi_64;
	const T _1_div_N = (T)1 / (T)_N;
	const T _2Pi_div_N = _2Pi * _1_div_N;

	{
		cxT cxn = 0;
		for (uint m = 0; m < _N; ++m)
		{
			cxn += cxInput[m];
		}
		cxOutput[0] = cxn * _1_div_N;
	}

	for (uint n = 1; n < _N; ++n)
	{
		const T fn = (T)n;
		cxT cxn = 0;
		for (uint m = 0; m < _N; ++m)
		{
			const T fm = (T)m;
			const T fAngle = _2Pi_div_N*fn*fm;
			cxn += cxInput[m] * cxT(Cos(fAngle), Sin(fAngle));
		}
		cxOutput[_N-n] = cxn * _1_div_N;
	}
}














#if FFTL_SIMD4
template <uint _M>
FFT<_M, float, float>::FFT()
{
	//	4 component SIMD FFT won't work unless we can process 8 elements or more at a time
	FFTL_StaticAssert(_N>=8);
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::TransformForward(const FixedArray<T,_N>& fInR, const FixedArray<T,_N>& fInI, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	Transform_Stage0_BR(fInR, fInI, fOutR, fOutI);

	//	Invoke the main transform function
	Transform_Main_DIT(fOutR, fOutI);
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::TransformForward(const FixedArray<cxT,_N>& cxInput, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	Transform_Stage0_BR(cxInput, fOutR, fOutI);

	//	Invoke the main transform function
	Transform_Main_DIT(fOutR, fOutI);
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::TransformInverse(const FixedArray<T,_N>& fInR, const FixedArray<T,_N>& fInI, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	//	Swap the real and imaginary parts.
	TransformForward(fInI, fInR, fOutI, fOutR);

#if 0 // Don't divide by N because this is typically used in convolution, and we can just do it there.
	//	Divide everything by N.
	const T _1_div_N = (T)1 / (T)_N;
	const f32_4 v1_div_N = { _1_div_N,_1_div_N,_1_div_N,_1_div_N };

	for (uint n = 0; n < _N; n+=4)
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

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::TransformForward_InPlace_DIF(FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
#endif

	uint uTwiddleIndex = _N - (FFTL_SIMD8 ? 8 : 4);

	//	We can make an exception in the case of the last stage because the inner loop only runs one time for each sub DFT,
	// and our twiddle factors are all contiguous in memory.
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif

		const uint nStageExp=_N;
		const uint nStageExp_2 = nStageExp>>1;

#if FFTL_SIMD8
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-8; nSubDFT >= 0; nSubDFT-=8, uTwiddleIndex-=8)
		{
			const f32_8 vUr = f32_8::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_8 vUi = f32_8::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly+nStageExp_2;

			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#else
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-4; nSubDFT >= 0; nSubDFT-=4, uTwiddleIndex-=4)
		{
			const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly+nStageExp_2;

			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#endif //FFTL_SIMD8
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[_M-1].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}

	//	Loop down for each subsequent stage, but leave stage 2, 1 and 0 for special loops.
	for (int nStage=_M-2, nStageExp=_N/2; nStage >= 3; --nStage, nStageExp>>=1)
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif
		const uint nStageExp_2 = nStageExp>>1;

#if FFTL_SIMD8
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-8; nSubDFT >= 0; nSubDFT-=8, uTwiddleIndex-=8)
		{
			const f32_8 vUr = f32_8::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_8 vUi = f32_8::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#else
		//	Loop for each sub DFT
		for (int nSubDFT = nStageExp_2-4; nSubDFT >= 0; nSubDFT-=4, uTwiddleIndex-=4)
		{
			const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#endif //FFTL_SIMD8
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[nStage].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}

	//	Stage 2 easier to just use 4 wide.
	{
		const uint nStageExp = 8;
		const uint nStageExp_2 = 4;

		const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(4));
		const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(4));

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
		{
			const uint uButterflyNext = uButterfly+nStageExp_2;
			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIF(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
	}

	//	Specialized SIMD case for stage 1 that requires XYXYZWZW shuffling
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif

		//	Get the phase angles for the next 2 sub DFT's
		const f32_4 vUr(1, 0, 1, 0);
		const f32_4 vUi(0, -1, 0, -1);

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=8)
		{
			T* __restrict pfR = &fOutR[uButterfly];
			T* __restrict pfI = &fOutI[uButterfly];

			Calculate4Butterflies_DIF_Stage1(vUr, vUi, pfR, pfI);
		}
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[1].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}

	//	Specialized SIMD case for stage 0.
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif
		//	Loop for each 4 butterflies
		for (uint n = 0; n < _N; n+=8)
		{
			//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
			// Also the input is already pre-shuffled.
			Calculate4Butterflies_DIF_Stage0(&fOutR[n], &fOutI[n]);
		}

#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[0].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::TransformInverse_InPlace_DIT(FixedArray<T,_N>& fInOutR, FixedArray<T,_N>& fInOutI) const
{
	//	Swap the real and imaginary parts.
	Transform_Stage0(fInOutI, fInOutR);

	//	Invoke the main transform function
	Transform_Main_DIT(fInOutI, fInOutR);
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::Transform_Stage0(FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	//	Assume the input has already been bit-reversed, or it doesn't need to be.
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Loop for each 4 butterflies
	for (uint n = 0; n < _N; n+=8)
	{
		//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
		// Also the input is already pre-shuffled.
		Calculate4Butterflies_DIT_Stage0(&fOutR[n], &fOutI[n]);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	m_StageTimers[0].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::Transform_Stage0_BR(const FixedArray<T,_N>& fInReal, const FixedArray<T,_N>& fInImag, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	//	Specialized SIMD case for stage 0 that requires XXZZYYWW shuffling
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Perform the first stage of the transform while copying the input to the output with bit reversal indices.
	for (uint n = 0; n < _N; n+=8)
	{
		//	Loop for each 4 butterflies

		const uint nR0 = m_BitReverseIndices[n+0];
		const uint nR1 = m_BitReverseIndices[n+1];
		const uint nR2 = m_BitReverseIndices[n+2];
		const uint nR3 = m_BitReverseIndices[n+3];
		const uint nR4 = m_BitReverseIndices[n+4];
		const uint nR5 = m_BitReverseIndices[n+5];
		const uint nR6 = m_BitReverseIndices[n+6];
		const uint nR7 = m_BitReverseIndices[n+7];

		//	Shuffle the inputs around so that we can do 4 butterflies at once. Current is even, next is odd.
		const f32_4 vCurR = V4fSet(fInReal[nR0], fInReal[nR2], fInReal[nR4], fInReal[nR6]);
		const f32_4 vCurI = V4fSet(fInImag[nR0], fInImag[nR2], fInImag[nR4], fInImag[nR6]);

		const f32_4 vNextR = V4fSet(fInReal[nR1], fInReal[nR3], fInReal[nR5], fInReal[nR7]);
		const f32_4 vNextI = V4fSet(fInImag[nR1], fInImag[nR3], fInImag[nR5], fInImag[nR7]);

		//	Twiddle factor isn't needed here because it's multiplying by 1 (this calculation requires only adding and subtracting)
		// Also the input is already pre-shuffled.
		Calculate4Butterflies_DIT_Stage0(vCurR, vNextR, vCurI, vNextI, &fOutR[n], &fOutI[n]);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	m_StageTimers[0].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}

template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::Transform_Stage0_BR(const FixedArray<cxT,_N>& cxInput, FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
	//	Specialized SIMD case for stage 0 that requires XXZZYYWW shuffling
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Perform the first stage of the transform while copying the input to the output with bit reversal indices.
	for (uint n = 0; n < _N; n+=8)
	{
		//	Loop for each 4 butterflies

		const uint nR0 = m_BitReverseIndices[n+0];
		const uint nR1 = m_BitReverseIndices[n+1];
		const uint nR2 = m_BitReverseIndices[n+2];
		const uint nR3 = m_BitReverseIndices[n+3];
		const uint nR4 = m_BitReverseIndices[n+4];
		const uint nR5 = m_BitReverseIndices[n+5];
		const uint nR6 = m_BitReverseIndices[n+6];
		const uint nR7 = m_BitReverseIndices[n+7];

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
	QueryPerformanceCounter(&laEnd);
	m_StageTimers[0].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}


template <uint _M>
FFTL_COND_INLINE void FFT<_M, float, float>::Transform_Main_DIT(FixedArray<T,_N>& fOutR, FixedArray<T,_N>& fOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
#endif

	//	Specialized SIMD case for stage 1 that requires XYXYZWZW shuffling
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif

		//	Get the phase angles for the next 2 sub DFT's
		const f32_4 vUr(1, 0, 1, 0);
		const f32_4 vUi(0, -1, 0, -1);

		//	Loop for each 4 butterflies
		for (uint uButterfly = 0; uButterfly < _N; uButterfly+=8)
		{
			T* __restrict pfR = &fOutR[uButterfly];
			T* __restrict pfI = &fOutI[uButterfly];

			Calculate4Butterflies_DIT_Stage1(vUr, vUi, pfR, pfI);
		}
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[1].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}


	//	Only 4 wide SIMD for stage 2
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif

		const uint nStageExp = 8;
		const uint nStageExp_2 = 4;

		//	Loop for each sub DFT
//		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT+=4, uTwiddleIndex+=4)
		{
			const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(4));
			const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(4));

			//	Loop for each 4 butterflies
			for (uint uButterfly = 0; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[2].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}

	uint uTwiddleIndex = 8;

	//	Loop for each stage except for the last
	for (uint nStage=3, nStageExp=16; nStage < _M-1; ++nStage, nStageExp<<=1)
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif

		const uint nStageExp_2 = nStageExp>>1;
#if FFTL_SIMD8
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT+=8, uTwiddleIndex+=8)
		{
			const f32_8 vUr = f32_8::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_8 vUi = f32_8::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#else
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT+=4, uTwiddleIndex+=4)
		{
			const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			//	Loop for each 4 butterflies
			for (uint uButterfly = nSubDFT; uButterfly < _N; uButterfly+=nStageExp)
			{
				const uint uButterflyNext = uButterfly+nStageExp_2;
				T* __restrict pfCurR = &fOutR[uButterfly];
				T* __restrict pfCurI = &fOutI[uButterfly];
				T* __restrict pfNextR = &fOutR[uButterflyNext];
				T* __restrict pfNextI = &fOutI[uButterflyNext];
				CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
			}
		}
#endif // FFTL_SIMD8
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[nStage].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}

	//	We can make an exception in the case of the last stage because the inner loop only runs one time for each sub DFT,
	// and our twiddle factors are all contiguous in memory.
	{
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laStart);
#endif
	
		const uint nStageExp=_N;
		const uint nStageExp_2 = nStageExp>>1;

#if FFTL_SIMD8
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT+=8, uTwiddleIndex+=8)
		{
			const f32_8 vUr = f32_8::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_8 vUi = f32_8::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly+nStageExp_2;
			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#else
		//	Loop for each sub DFT
		for (uint nSubDFT = 0; nSubDFT < nStageExp_2; nSubDFT+=4, uTwiddleIndex+=4)
		{
			const f32_4 vUr = f32_4::LoadA(this->GetTwiddleRealPtr(uTwiddleIndex));
			const f32_4 vUi = f32_4::LoadA(this->GetTwiddleImagPtr(uTwiddleIndex));

			const uint uButterfly = nSubDFT;
			const uint uButterflyNext = uButterfly+nStageExp_2;
			T* __restrict pfCurR = &fOutR[uButterfly];
			T* __restrict pfCurI = &fOutI[uButterfly];
			T* __restrict pfNextR = &fOutR[uButterflyNext];
			T* __restrict pfNextI = &fOutI[uButterflyNext];
			CalculateVButterflies_DIT(vUr, vUi, pfCurR, pfCurI, pfNextR, pfNextI);
		}
#endif //FFTL_SIMD8
#if FFTL_STAGE_TIMERS
		QueryPerformanceCounter(&laEnd);
		m_StageTimers[_M-1].QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
	}
}

template <uint _M>
FFTL_FORCEINLINE void FFT<_M, float, float>::Calculate4Butterflies_DIT_Stage1(f32_4Arg vUr, f32_4Arg vUi, T* __restrict pfR, T* __restrict pfI)
{
	//	No need to shuffle the input because we've already pre-shuffled
	const f32_4 vCurR = f32_4::LoadA(pfR+0);
	const f32_4 vNextR = f32_4::LoadA(pfR+4);

	const f32_4 vCurI = f32_4::LoadA(pfI+0);
	const f32_4 vNextI = f32_4::LoadA(pfI+4);

	const f32_4 Wr = ( (vNextR*vUr) - (vNextI*vUi) );
	const f32_4 Wi = ( (vNextR*vUi) + (vNextI*vUr) );

	const f32_4 vNewNextR = (vCurR - Wr);
	const f32_4 vNewNextI = (vCurI - Wi);
	const f32_4 vNewCurR = (vCurR + Wr);
	const f32_4 vNewCurI = (vCurI + Wi);

	//	Now shuffle them for the subsequent stages, and store
	const f32_4 vCCNNr0 = Permute<sh_X1,sh_Y1,sh_X2,sh_Y2>(vNewCurR, vNewNextR);
	StoreA(pfR+0, vCCNNr0);
	const f32_4 vCCNNr1 = Permute<sh_Z1,sh_W1,sh_Z2,sh_W2>(vNewCurR, vNewNextR);
	StoreA(pfR+4, vCCNNr1);

	const f32_4 vCCNNi0 = Permute<sh_X1,sh_Y1,sh_X2,sh_Y2>(vNewCurI, vNewNextI);
	StoreA(pfI+0, vCCNNi0);				
	const f32_4 vCCNNi1 = Permute<sh_Z1,sh_W1,sh_Z2,sh_W2>(vNewCurI, vNewNextI);
	StoreA(pfI+4, vCCNNi1);
}

template <uint _M>
FFTL_FORCEINLINE void FFT<_M, float, float>::Calculate4Butterflies_DIF_Stage1(f32_4Arg vUr, f32_4Arg vUi, T* __restrict pfR, T* __restrict pfI)
{
	//	For DIF processing, we're running backwards, so there was no pre-shuffling in stage 2.

	const f32_4 vCCNN0r = f32_4::LoadA(pfR+0);
	const f32_4 vCCNN1r = f32_4::LoadA(pfR+4);

	const f32_4 vCCNN0i = f32_4::LoadA(pfI+0);
	const f32_4 vCCNN1i = f32_4::LoadA(pfI+4);

	const f32_4 vCurR =  Permute<sh_X1,sh_Y1,sh_X2,sh_Y2>(vCCNN0r, vCCNN1r);
	const f32_4 vNextR = Permute<sh_Z1,sh_W1,sh_Z2,sh_W2>(vCCNN0r, vCCNN1r);

	const f32_4 vCurI =  Permute<sh_X1,sh_Y1,sh_X2,sh_Y2>(vCCNN0i, vCCNN1i);
	const f32_4 vNextI = Permute<sh_Z1,sh_W1,sh_Z2,sh_W2>(vCCNN0i, vCCNN1i);

	const f32_4 Wr = ( vCurR - vNextR );
	const f32_4 Wi = ( vCurI - vNextI );

	const f32_4 vNewCurR = (vCurR + vNextR);
	const f32_4 vNewCurI = (vCurI + vNextI);
	const f32_4 vNewNextR = ( ( Wr * vUr) - ( Wi * vUi) );
	const f32_4 vNewNextI = ( ( Wr * vUi) + ( Wi * vUr) );

	//	Don't pre-shuffle for stage 0. Stage 0 will self-correct.
	StoreA(pfR+0, vNewCurR);
	StoreA(pfR+4, vNewNextR);
	StoreA(pfI+0, vNewCurI);
	StoreA(pfI+4, vNewNextI);
}

template <uint _M>
FFTL_FORCEINLINE void FFT<_M, float, float>::Calculate4Butterflies_DIT_Stage0(T* __restrict pfR, T* __restrict pfI)
{
	//	Shuffle the input around for the first stage so we can properly process 4 at a time.

	const f32_4 vCNCN0r = f32_4::LoadA(pfR+0);
	const f32_4 vCNCN1r = f32_4::LoadA(pfR+4);

	const f32_4 vCNCN0i = f32_4::LoadA(pfI+0);
	const f32_4 vCNCN1i = f32_4::LoadA(pfI+4);

	const f32_4 vCurR =  Permute<sh_X1,sh_Z1,sh_X2,sh_Z2>(vCNCN0r, vCNCN1r);
	const f32_4 vNextR = Permute<sh_Y1,sh_W1,sh_Y2,sh_W2>(vCNCN0r, vCNCN1r);

	const f32_4 vCurI =  Permute<sh_X1,sh_Z1,sh_X2,sh_Z2>(vCNCN0i, vCNCN1i);
	const f32_4 vNextI = Permute<sh_Y1,sh_W1,sh_Y2,sh_W2>(vCNCN0i, vCNCN1i);

	const f32_4 vNewNextR = (vCurR - vNextR);
	const f32_4 vNewNextI = (vCurI - vNextI);
	const f32_4 vNewCurR = (vCurR + vNextR);
	const f32_4 vNewCurI = (vCurI + vNextI);

	//	Now shuffle them so that stage 1 doesn't have to pre-shuffle on input, and store
	const f32_4 vCCNNr0 = Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vNewCurR, vNewNextR);
	const f32_4 vCCNNr1 = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vNewCurR, vNewNextR);
	const f32_4 vCCNNi0 = Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vNewCurI, vNewNextI);
	const f32_4 vCCNNi1 = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vNewCurI, vNewNextI);
	
	StoreA(pfR+0, vCCNNr0);
	StoreA(pfR+4, vCCNNr1);
	StoreA(pfI+0, vCCNNi0);
	StoreA(pfI+4, vCCNNi1);
}

template <uint _M>
FFTL_FORCEINLINE void FFT<_M, float, float>::Calculate4Butterflies_DIF_Stage0(T* __restrict pfR, T* __restrict pfI)
{
	//	For DIF processing, we're running backwards, so stage 1 has processed before us, and we need to correct for its order.

	const f32_4 vs1_0r = f32_4::LoadA(pfR+0);
	const f32_4 vs1_1r = f32_4::LoadA(pfR+4);

	const f32_4 vs1_0i = f32_4::LoadA(pfI+0);
	const f32_4 vs1_1i = f32_4::LoadA(pfI+4);

	const f32_4 vCurR =  Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vs1_0r, vs1_1r);
	const f32_4 vNextR = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vs1_0r, vs1_1r);

	const f32_4 vCurI =  Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vs1_0i, vs1_1i);
	const f32_4 vNextI = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vs1_0i, vs1_1i);

	const f32_4 vNewNextR = (vCurR - vNextR);
	const f32_4 vNewNextI = (vCurI - vNextI);
	const f32_4 vNewCurR = (vCurR + vNextR);
	const f32_4 vNewCurI = (vCurI + vNextI);

	//	Now post shuffle them back to the normal (final) order.
	const f32_4 vCNCNr0 = Permute<sh_X1,sh_X2,sh_Y1,sh_Y2>(vNewCurR, vNewNextR);
	const f32_4 vCNCNr1 = Permute<sh_Z1,sh_Z2,sh_W1,sh_W2>(vNewCurR, vNewNextR);
	const f32_4 vCNCNi0 = Permute<sh_X1,sh_X2,sh_Y1,sh_Y2>(vNewCurI, vNewNextI);
	const f32_4 vCNCNi1 = Permute<sh_Z1,sh_Z2,sh_W1,sh_W2>(vNewCurI, vNewNextI);

	StoreA(pfR+0, vCNCNr0);
	StoreA(pfR+4, vCNCNr1);
	StoreA(pfI+0, vCNCNi0);
	StoreA(pfI+4, vCNCNi1);
}

template <uint _M>
FFTL_FORCEINLINE void FFT<_M, float, float>::Calculate4Butterflies_DIT_Stage0(f32_4Arg vCurR, f32_4Arg vNextR, f32_4Arg vCurI, f32_4Arg vNextI, T* __restrict pfR, T* __restrict pfI)
{
	//	No need to shuffle the input because we've already pre-shuffled
	const f32_4 vNewNextR = (vCurR - vNextR);
	const f32_4 vNewNextI = (vCurI - vNextI);
	const f32_4 vNewCurR = (vCurR + vNextR);
	const f32_4 vNewCurI = (vCurI + vNextI);

	//	Now shuffle them so that stage 1 doesn't have to pre-shuffle on input, and store
	const f32_4 vCCNNr0 = Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vNewCurR, vNewNextR);
	const f32_4 vCCNNr1 = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vNewCurR, vNewNextR);
	const f32_4 vCCNNi0 = Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(vNewCurI, vNewNextI);
	const f32_4 vCCNNi1 = Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(vNewCurI, vNewNextI);

	StoreA(pfR+0, vCCNNr0);
	StoreA(pfR+4, vCCNNr1);
	StoreA(pfI+0, vCCNNi0);
	StoreA(pfI+4, vCCNNi1);
}

template <uint _M>
template <typename V>
FFTL_FORCEINLINE void FFT<_M, float, float>::CalculateVButterflies_DIT(const V& vUr, const V& vUi, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	const V vCurR = V::LoadA(pfCurR);
	const V vCurI = V::LoadA(pfCurI);
	const V vNextR = V::LoadA(pfNextR);
	const V vNextI = V::LoadA(pfNextI);

	const V Wr = ( (vNextR*vUr) - (vNextI*vUi) );
	const V Wi = ( (vNextR*vUi) + (vNextI*vUr) );

	const V vNewNextR = (vCurR - Wr);
	const V vNewNextI = (vCurI - Wi);
	const V vNewCurR = (vCurR + Wr);
	const V vNewCurI = (vCurI + Wi);

	StoreA(pfNextR, vNewNextR);
	StoreA(pfNextI, vNewNextI);
	StoreA(pfCurR, vNewCurR);
	StoreA(pfCurI, vNewCurI);
}

template <uint _M>
template <typename V>
FFTL_FORCEINLINE void FFT<_M, float, float>::CalculateVButterflies_DIF(const V& vUr, const V& vUi, T* __restrict pfCurR, T* __restrict pfCurI, T* __restrict pfNextR, T* __restrict pfNextI)
{
	const V vCurR = V::LoadA(pfCurR);
	const V vCurI = V::LoadA(pfCurI);
	const V vNextR = V::LoadA(pfNextR);
	const V vNextI = V::LoadA(pfNextI);

	const V Wr = ( vCurR - vNextR );
	const V Wi = ( vCurI - vNextI );

	const V vNewCurR = (vCurR + vNextR);
	const V vNewCurI = (vCurI + vNextI);
	const V vNewNextR = ( ( Wr * vUr) - ( Wi * vUi) );
	const V vNewNextI = ( ( Wr * vUi) + ( Wi * vUr) );
	
	StoreA(pfNextR, vNewNextR);
	StoreA(pfNextI, vNewNextI);
	StoreA(pfCurR, vNewCurR);
	StoreA(pfCurI, vNewCurI);
}
#endif





template <uint _M, typename T, typename T_Twiddle>
FFT_Real_Base<_M, T, T_Twiddle>::FFT_Real_Base()
{
	for (uint n = 0; n < _N_4; ++n)
	{
		const double fAngle = 0.5 * pi_64 * (double)n / (double)_N_4;
		m_PostTwiddlesR[n] = (float)-Sin(fAngle);
		m_PostTwiddlesI[n] = (float)-Cos(fAngle);
	}
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Real_Base<_M, T, T_Twiddle>::TransformForward(const FixedArray<T,_N>& fTimeIn, FixedArray<T,_N_2>& fFreqOutR, FixedArray<T,_N_2>& fFreqOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
#endif

	const FixedArray<cxT,_N_2>& cxInput = *reinterpret_cast<const FixedArray<cxT,_N_2>*>(&fTimeIn);

	//	Perform the half size complex FFT
	this->m_fft.TransformForward(cxInput, fFreqOutR, fFreqOutI);

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laStart);
#endif

	//	Special case for 0 index
	{
		const T fDcR = fFreqOutR[0];
		const T fDcI = fFreqOutI[0];
		fFreqOutR[0] = fDcR + fDcI;
		fFreqOutI[0] = fDcR - fDcI; // Sneaky shove of the Nyquist bin into the imag DC bin because it's always 0 anyway.
	}

	const T vHalf = ConvertTo<T>(0.5f);

	for (uint n = 1; n < _N_4; n += 1)
	{
		const uint Nmn = _N_2-n;

		const cxNumber<T> twid( ConvertTo<T>(this->GetTwiddleReal(n)), ConvertTo<T>(this->GetTwiddleImag(n)) );

		const cxNumber<T> fpk( fFreqOutR[n], fFreqOutI[n] );
		const cxNumber<T> fpnk( fFreqOutR[Nmn], -fFreqOutI[Nmn] );

		const cxNumber<T> f1k = fpk + fpnk;
		const cxNumber<T> f2k = fpk - fpnk;
		const cxNumber<T> tw = f2k * twid;

		fFreqOutR[n] = (vHalf * (f1k.r + tw.r));
		fFreqOutI[n] = (vHalf * (f1k.i + tw.i));
		fFreqOutR[Nmn] = (vHalf * (f1k.r - tw.r));
		fFreqOutI[Nmn] = (vHalf * (tw.i - f1k.i));
	}

	//	The odd center bin just needs the imaginary part negated.
	fFreqOutI[_N_4] = -fFreqOutI[_N_4];

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	this->m_fft.m_PostProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}

template <uint _M, typename T, typename T_Twiddle>
FFTL_COND_INLINE void FFT_Real_Base<_M, T, T_Twiddle>::TransformInverse(const FixedArray<T,_N_2>& fFreqInR, const FixedArray<T,_N_2>& fFreqInI, FixedArray<T,_N>& fTimeOut) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	FixedArray<cxT,_N_2>& cxTimeOut = *reinterpret_cast<FixedArray<cxT,_N_2>*>(fTimeOut+0);

	const T vInv_N = ConvertTo<T>((T_Twiddle)1.0/_N);

	//	Special case for 0 index
	{
		const float fDC = fFreqInR[0] * vInv_N;
		const float fNy = fFreqInI[0] * vInv_N;
		cxTimeOut[0].i = fDC + fNy;
		cxTimeOut[0].r = fDC - fNy;
	}

	for (uint n = 1; n < _N_4; n += 1)
	{
		const uint Nmn = _N_2-n;

		const cxNumber<T> twid( ConvertTo<T>(this->GetTwiddleReal(n)), -ConvertTo<T>(this->GetTwiddleImag(n)) );

		const cxNumber<T> fk( fFreqInR[n], fFreqInI[n] );
		const cxNumber<T> fnkc( fFreqInR[Nmn], -fFreqInI[Nmn] );

		const cxNumber<T> fek = fk + fnkc;
		const cxNumber<T> tmp = fk - fnkc;
		const cxNumber<T> fok = tmp * twid;

		const uint nR =		this->m_fft.GetBitReverseIndex(n);
		const uint NmnR =	this->m_fft.GetBitReverseIndex(Nmn);

		//	Flip real and imaginary for inverse FFT and apply bit reversal for DIF version.
		cxTimeOut[nR].i = (fek.r + fok.r) * vInv_N;
		cxTimeOut[nR].r = (fek.i + fok.i) * vInv_N;
		cxTimeOut[NmnR].i = (fek.r - fok.r) * vInv_N;
		cxTimeOut[NmnR].r = (fok.i - fek.i) * vInv_N;
	}

	const T vTwo = ConvertTo<T>(2) * vInv_N;
	const uint nR =	this->m_fft.GetBitReverseIndex(_N_4);

	//	The odd center bin just needs to be doubled and the imaginary part negated.
	cxTimeOut[nR].i = fFreqInR[_N_4] * +vTwo;
	cxTimeOut[nR].r = fFreqInI[_N_4] * -vTwo;

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	this->m_fft.m_PreProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif

	//	Perform the half size complex inverse FFT
	this->m_fft.TransformForward_InPlace_DIF(cxTimeOut);
}







#if FFTL_SIMD4
template <uint _M>
FFTL_COND_INLINE void FFT_Real<_M, float, float>::TransformForward(const FixedArray<T,_N>& fTimeIn, FixedArray<T,_N_2>& fFreqOutR, FixedArray<T,_N_2>& fFreqOutI) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	//	Perform the half size complex FFT
	this->m_fft.TransformForward(*reinterpret_cast<const FixedArray<cxT,_N_2>*>(&fTimeIn), fFreqOutR, fFreqOutI);

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laStart);
#endif

	//	Special case for 0 index
	{
		const float fDcR = fFreqOutR[0];
		const float fDcI = fFreqOutI[0];
		fFreqOutR[0] = fDcR + fDcI;
		fFreqOutI[0] = fDcR - fDcI; // Sneaky shove of the Nyquist bin into the imag DC bin because it's always 0 anyway.
	}

	const f32_4 vHalf = ConvertTo<f32_4>(0.5f);

	//	Special case for 3 element loads and stores we only need to do once.
	{
		const uint n = 1;
		const uint Nmn = _N_2-n-2;

		//	Starting at 1 forces an unaligned load.
		const cxNumber<f32_4> twid( f32_4::LoadU(this->GetTwiddleRealPtr(n)), f32_4::LoadU(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_4> fpk( f32_4::LoadU(fFreqOutR+n), f32_4::LoadU(fFreqOutI+n) );
		const cxNumber<f32_4> fpnk( ZYXX( f32_4::Load3(fFreqOutR+Nmn) ), -ZYXX( f32_4::Load3(fFreqOutI+Nmn) ) );

		const cxNumber<f32_4> f1k = fpk + fpnk;
		const cxNumber<f32_4> f2k = fpk - fpnk;
		const cxNumber<f32_4> tw = f2k * twid;

		//	Store only 3 so we don't pollute the next stage
		(vHalf * (f1k.r + tw.r)).Store3(fFreqOutR+n);
		(vHalf * (f1k.i + tw.i)).Store3(fFreqOutI+n);
		ZYXX(vHalf * (f1k.r - tw.r)).Store3(fFreqOutR+Nmn);
		ZYXX(vHalf * (tw.i - f1k.i)).Store3(fFreqOutI+Nmn);
	}

	//	N-n loading will be unaligned.
	{
		const uint n = 4;
		const uint Nmn = _N_2-n-3;

		const cxNumber<f32_4> twid( f32_4::LoadA(this->GetTwiddleRealPtr(n)), f32_4::LoadA(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_4> fpk( f32_4::LoadA(fFreqOutR+n), f32_4::LoadA(fFreqOutI+n) );
		const cxNumber<f32_4> fpnk( Reverse( f32_4::LoadU(fFreqOutR+Nmn) ), -Reverse( f32_4::LoadU(fFreqOutI+Nmn) ) );

		const cxNumber<f32_4> f1k = fpk + fpnk;
		const cxNumber<f32_4> f2k = fpk - fpnk;
		const cxNumber<f32_4> tw = f2k * twid;

		(vHalf * (f1k.r + tw.r)).StoreA(fFreqOutR+n);
		(vHalf * (f1k.i + tw.i)).StoreA(fFreqOutI+n);
		Reverse(vHalf * (f1k.r - tw.r)).StoreU(fFreqOutR+Nmn);
		Reverse(vHalf * (tw.i - f1k.i)).StoreU(fFreqOutI+Nmn);
	}
#if FFTL_SIMD8
	//	N-n loading will be unaligned.
	for (uint n = 8; n < _N_4; n += 8)
	{
		const f32_8 vHalf8 = f32_8::Splat(vHalf);

		const uint Nmn = _N_2-n-7;

		const cxNumber<f32_8> twid( f32_8::LoadA(this->GetTwiddleRealPtr(n)), f32_8::LoadA(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_8> fpk( f32_8::LoadA(fFreqOutR+n), f32_8::LoadA(fFreqOutI+n) );
		const cxNumber<f32_8> fpnk( Reverse( f32_8::LoadU(fFreqOutR+Nmn) ), -Reverse( f32_8::LoadU(fFreqOutI+Nmn) ) );

		const cxNumber<f32_8> f1k = fpk + fpnk;
		const cxNumber<f32_8> f2k = fpk - fpnk;
		const cxNumber<f32_8> tw = f2k * twid;

		(vHalf8 * (f1k.r + tw.r)).StoreA(fFreqOutR+n);
		(vHalf8 * (f1k.i + tw.i)).StoreA(fFreqOutI+n);
		Reverse(vHalf8 * (f1k.r - tw.r)).StoreU(fFreqOutR+Nmn);
		Reverse(vHalf8 * (tw.i - f1k.i)).StoreU(fFreqOutI+Nmn);
	}
#else
	//	N-n loading will be unaligned.
	for (uint n = 8; n < _N_4; n += 4)
	{
		const uint Nmn = _N_2-n-3;

		const cxNumber<f32_4> twid( f32_4::LoadA(this->GetTwiddleRealPtr(n)), f32_4::LoadA(this->GetTwiddleImagPtr(n)) );
		
		const cxNumber<f32_4> fpk( f32_4::LoadA(fFreqOutR+n), f32_4::LoadA(fFreqOutI+n) );
		const cxNumber<f32_4> fpnk( Reverse( f32_4::LoadU(fFreqOutR+Nmn) ), -Reverse( f32_4::LoadU(fFreqOutI+Nmn) ) );
		
		const cxNumber<f32_4> f1k = fpk + fpnk;
		const cxNumber<f32_4> f2k = fpk - fpnk;
		const cxNumber<f32_4> tw = f2k * twid;

		(vHalf * (f1k.r + tw.r)).StoreA(fFreqOutR+n);
		(vHalf * (f1k.i + tw.i)).StoreA(fFreqOutI+n);
		Reverse(vHalf * (f1k.r - tw.r)).StoreU(fFreqOutR+Nmn);
		Reverse(vHalf * (tw.i - f1k.i)).StoreU(fFreqOutI+Nmn);
	}
#endif

	//	The odd center bin just needs the imaginary part negated.
	fFreqOutI[_N_4] = -fFreqOutI[_N_4];

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	this->m_fft.m_PostProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}

template <uint _M>
FFTL_COND_INLINE void FFT_Real<_M, float, float>::TransformInverse(const FixedArray<T,_N_2>& fFreqInR, const FixedArray<T,_N_2>& fFreqInI, FixedArray<T,_N>& fTimeOut) const
{
#if FFTL_STAGE_TIMERS
	LARGE_INTEGER laStart, laEnd;
	QueryPerformanceCounter(&laStart);
#endif

	FixedArray<T,_N_2>& fTimeOutR = *reinterpret_cast<FixedArray<T,_N_2>*>(fTimeOut+0);
	FixedArray<T,_N_2>& fTimeOutI = *reinterpret_cast<FixedArray<T,_N_2>*>(fTimeOut+_N_2);

	//	Special case for 0 index
	{
		const float fDC = fFreqInR[0];
		const float fNy = fFreqInI[0];
		fTimeOutR[0] = fDC + fNy;
		fTimeOutI[0] = fDC - fNy;
	}

	//	Special case for 3 element loads and stores we only need to do once.
	{
		const uint n = 1;
		const uint Nmn = _N_2-n-2;

		//	Starting at 1 forces an unaligned load.
		const cxNumber<f32_4> twid( f32_4::LoadU(this->GetTwiddleRealPtr(n)), -f32_4::LoadU(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_4> fk( f32_4::LoadU(fFreqInR+n), f32_4::LoadU(fFreqInI+n) );
		const cxNumber<f32_4> fnkc( ZYXX( f32_4::Load3(fFreqInR+Nmn) ), -ZYXX( f32_4::Load3(fFreqInI+Nmn) ) );

		const cxNumber<f32_4> fek = fk + fnkc;
		const cxNumber<f32_4> tmp = fk - fnkc;
		const cxNumber<f32_4> fok = tmp * twid;

		//	Store only 3 so we don't pollute the next stage
		(fek.r + fok.r).Store3(fTimeOutR+n);
		(fek.i + fok.i).Store3(fTimeOutI+n);
		ZYXX(fek.r - fok.r).Store3(fTimeOutR+Nmn);
		ZYXX(fok.i - fek.i).Store3(fTimeOutI+Nmn);
	}

	//	N-n loading will be unaligned.
	{
		const uint n = 4;
		const uint Nmn = _N_2-n-3;

		const cxNumber<f32_4> twid( f32_4::LoadA(this->GetTwiddleRealPtr(n)), -f32_4::LoadA(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_4> fk( f32_4::LoadA(fFreqInR+n), f32_4::LoadA(fFreqInI+n) );
		const cxNumber<f32_4> fnkc( Reverse( f32_4::LoadU(fFreqInR+Nmn) ), -Reverse( f32_4::LoadU(fFreqInI+Nmn) ) );

		const cxNumber<f32_4> fek = fk + fnkc;
		const cxNumber<f32_4> tmp = fk - fnkc;
		const cxNumber<f32_4> fok = tmp * twid;

		(fek.r + fok.r).StoreA(fTimeOutR+n);
		(fek.i + fok.i).StoreA(fTimeOutI+n);
		Reverse(fek.r - fok.r).StoreU(fTimeOutR+Nmn);
		Reverse(fok.i - fek.i).StoreU(fTimeOutI+Nmn);
	}

#if FFTL_SIMD8
	//	N-n loading will be unaligned.
	for (uint n = 8; n < _N_4; n += 8)
	{
		const uint Nmn = _N_2-n-7;

		const cxNumber<f32_8> twid( f32_8::LoadA(this->GetTwiddleRealPtr(n)), -f32_8::LoadA(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_8> fk( f32_8::LoadA(fFreqInR+n), f32_8::LoadA(fFreqInI+n) );
		const cxNumber<f32_8> fnkc( Reverse( f32_8::LoadU(fFreqInR+Nmn) ), -Reverse( f32_8::LoadU(fFreqInI+Nmn) ) );

		const cxNumber<f32_8> fek = fk + fnkc;
		const cxNumber<f32_8> tmp = fk - fnkc;
		const cxNumber<f32_8> fok = tmp * twid;

		(fek.r + fok.r).StoreA(fTimeOutR+n);
		(fek.i + fok.i).StoreA(fTimeOutI+n);
		Reverse(fek.r - fok.r).StoreU(fTimeOutR+Nmn);
		Reverse(fok.i - fek.i).StoreU(fTimeOutI+Nmn);
	}
#else
	//	N-n loading will be unaligned.
	for (uint n = 8; n < _N_4; n += 4)
	{
		const uint Nmn = _N_2-n-3;

		const cxNumber<f32_4> twid( f32_4::LoadA(this->GetTwiddleRealPtr(n)), -f32_4::LoadA(this->GetTwiddleImagPtr(n)) );

		const cxNumber<f32_4> fk( f32_4::LoadA(fFreqInR+n), f32_4::LoadA(fFreqInI+n) );
		const cxNumber<f32_4> fnkc( Reverse( f32_4::LoadU(fFreqInR+Nmn) ), -Reverse( f32_4::LoadU(fFreqInI+Nmn) ) );

		const cxNumber<f32_4> fek = fk + fnkc;
		const cxNumber<f32_4> tmp = fk - fnkc;
		const cxNumber<f32_4> fok = tmp * twid;

		(fek.r + fok.r).StoreA(fTimeOutR+n);
		(fek.i + fok.i).StoreA(fTimeOutI+n);
		Reverse(fek.r - fok.r).StoreU(fTimeOutR+Nmn);
		Reverse(fok.i - fek.i).StoreU(fTimeOutI+Nmn);
	}
#endif

	//	The odd center bin just needs to be doubled and the imaginary part negated.
	fTimeOutR[_N_4] = fFreqInR[_N_4] * +2.f;
	fTimeOutI[_N_4] = fFreqInI[_N_4] * -2.f;

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	this->m_fft.m_PreProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif

	//	Perform the half size complex inverse FFT
	FixedArray_Aligned32<T,_N_2> fTempR;
	FixedArray_Aligned32<T,_N_2> fTempI;
	this->m_fft.TransformInverse(fTimeOutR, fTimeOutI, fTempR, fTempI);

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laStart);
#endif

	const f32_4 vInv_N = ConvertTo<f32_4>(1.f/_N);

	//	Restore the time domain real output as interleaved real and complex.
	for (uint n = 0; n < _N_2; n += 4)
	{
		const f32_4 vInA = f32_4::LoadA(fTempR+n) * vInv_N;
		const f32_4 vInB = f32_4::LoadA(fTempI+n) * vInv_N;
		const f32_4 vShA = MergeXY(vInA, vInB); // Evens
		const f32_4 vShB = MergeZW(vInA, vInB); // Odds
		vShA.StoreA(fTimeOut+n*2+0);
		vShB.StoreA(fTimeOut+n*2+4);
	}

#if FFTL_STAGE_TIMERS
	QueryPerformanceCounter(&laEnd);
	this->m_fft.m_PostProcessTimer.QuadPart += laEnd.QuadPart - laStart.QuadPart;
#endif
}
#endif








template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle> const FFT<_M, T, T_Twiddle> Convolver<_M, T_MAX_KERNELS, T,T_Twiddle>::sm_FFT;

template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle>
Convolver<_M, T_MAX_KERNELS, T, T_Twiddle>::Convolver()
	: m_KernelCount(0)
{
	memset(&m_AccumulationBuffer, 0, sizeof(m_AccumulationBuffer));
}

template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle>
Convolver<_M, T_MAX_KERNELS, T, T_Twiddle>::~Convolver()
{
	Destroy();
}

template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<_M, T_MAX_KERNELS, T, T_Twiddle>::InitKernel(const void* pKernelInput, size_t sizeBytes)
{
	const byte* byteInput = static_cast<const byte*>(pKernelInput);
	Destroy();

	const uint fileLength = (uint)sizeBytes;
	const uint sampleCount = fileLength / sizeof(T);

	//	Determine the number of kernels we need
	const uint kernelCount = AlignForward<_N>(sampleCount) / _N;

	memset(&m_KernelArray_FD, 0, sizeof(m_KernelArray_FD));
	m_KernelCount = kernelCount;

	const size_t segmentSize = sizeof(Kernel)/2;
	for (uint i = 0; i < kernelCount; ++i, sizeBytes-=segmentSize)
	{
		FFTL_ASSERT(sizeBytes > 0);
		Kernel& kernel = m_KernelArray_FD[i];
		memcpy(&kernel, byteInput + i*segmentSize, Min(segmentSize, sizeBytes));

		//	Convert to frequency domain, and store
		sm_FFT.TransformForward_InPlace_DIF(kernel.AsComplex());
	}
}

template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<_M, T_MAX_KERNELS, T, T_Twiddle>::Convolve(FixedArray<T,_N>& fInOutput)
{
	Kernel inputXFormed;
	Kernel workBuffer;

	const T invN = ConvertTo<T>(1 / (T_Twiddle)_N);

	memcpy(&inputXFormed.AsScalar(), &fInOutput, sizeof(inputXFormed)/2);
	memset(&inputXFormed.AsScalar()[_N], 0, sizeof(inputXFormed)/2);

	//	Convert the time domain input to freq domain
	sm_FFT.TransformForward_InPlace_DIF(inputXFormed.AsComplex());

	//	Perform short convolution on the first kernel only, while pushing to the small output buffer
	{
		Kernel& kernel = m_KernelArray_FD[0];
		T* pAccumBuffer = m_AccumulationBuffer+_N*0;

		//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
		for (uint n=0; n<_N; ++n)
		{
			const cxT& x = inputXFormed.AsComplex()[n];
			const cxT& y = kernel.AsComplex()[n];
			cxT& out = workBuffer.AsComplex()[n];

			out.r = x.r*y.r - x.i*y.i;
			out.i = x.r*y.i + x.i*y.r;
		}

		//	Convert the new frequency domain signal back to the time domain
		sm_FFT.TransformInverse_InPlace(workBuffer.AsComplex());

		//	Write to the output and accumulation buffer, while adding the overlap segment and fill it back in
		for (uint n=0; n<_N; ++n)
		{
			const T val = pAccumBuffer[n] = pAccumBuffer[_N+n] + workBuffer.AsScalar()[n];
			pAccumBuffer[n] = val;
			fInOutput[n] = val * invN;
		}

		for (uint n=_N; n<_2N; ++n)
		{
			pAccumBuffer[n] = workBuffer.AsScalar()[n];
		}
	}

	//	Perform short convolutions on the remaining kernels, accumulating everything as necessary
	for (uint k = 1; k < m_KernelCount; ++k)
	{
		Kernel& kernel = m_KernelArray_FD[k];
		T* pAccumBuffer = m_AccumulationBuffer+_N*k;

		//	Perform the convolution in the frequency domain, which corresponds to a complex multiplication by the kernel
		for (uint n=0; n<_N; ++n)
		{
			const cxT& x = inputXFormed.AsComplex()[n];
			const cxT& y = kernel.AsComplex()[n];
			cxT& out = workBuffer.AsComplex()[n];

			out.r = x.r*y.r - x.i*y.i;
			out.i = x.r*y.i + x.i*y.r;
		}

		//	Convert the new frequency domain signal back to the time domain
		sm_FFT.TransformInverse_InPlace_DIT(workBuffer.AsComplex());

		//	Write to the accumulation float buffer, while adding the overlap segment and fill it back in
		for (uint n=0; n<_N; ++n)
		{
			pAccumBuffer[n] += pAccumBuffer[_N+n] + workBuffer.AsScalar()[n];
		}
		for (uint n=_N; n<_2N; ++n)
		{
			pAccumBuffer[n] = workBuffer.AsScalar()[n];
		}
	}
}

template <uint _M, uint T_MAX_KERNELS, typename T, typename T_Twiddle>
void Convolver<_M, T_MAX_KERNELS, T, T_Twiddle>::Destroy()
{
}








template <typename T, uint _N, uint T_KERNEL_LENGTH>
void Convolver_Slow<T,_N,T_KERNEL_LENGTH>::Convolve(const FixedArray<T,_N>& fInput, FixedArray<T,_N>& fOutput)
{
	memmove(&m_AccumulationBuffer, &m_AccumulationBuffer[_N], sizeof(T)*T_KERNEL_LENGTH);
	memset(&m_AccumulationBuffer[T_KERNEL_LENGTH], 0, sizeof(T)*_N);

	for (uint n = 0; n < _N; ++n)
	{
		const T fInThis = fInput[n];
		for (uint m = 0; m < T_KERNEL_LENGTH; ++m)
		{
			m_AccumulationBuffer[n+m] += fInThis * m_Kernel[m];
		}
	}

	memcpy(&fOutput, &m_AccumulationBuffer, sizeof(fOutput));
}



} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // _FFTL_FFT_INL
