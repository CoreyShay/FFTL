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

//#include "pch.h"
#include "FFTL/fft.h"
#include "kiss_fft130/kiss_fftr.h"

//#include <wtypes.h>
//#include <Windows.h>
//#include <profileapi.h>
//#include <winnt.h>
//#include <conio.h>
#include <stdio.h>

//#include <intrin.h>


#if 0

namespace FFTL
{


/**********************************************************/
/* fft.c                                                  */
/* (c) Douglas L. Jones                                   */
/* University of Illinois at Urbana-Champaign             */
/* January 19, 1992                                       */
/*                                                        */
/*   fft: in-place radix-2 DIT DFT of a complex input     */
/*                                                        */
/*   input:                                               */
/* n: length of FFT: must be a power of two               */
/* m: n = 2**m                                            */
/*   input/output                                         */
/* x: double array of length n with real part of data     */
/* y: double array of length n with imag part of data     */
/*                                                        */
/*   Permission to copy and use this program is granted   */
/*   under a Creative Commons "Attribution" license       */
/*   http://creativecommons.org/licenses/by/1.0/          */
/**********************************************************/
#if 0
void fft(uint32 n, uint32 m, float x[], float y[])
{
	uint32 i,j,k,n1,n2;
	float c,s,e,a,t1,t2;        


	j = 0; /* bit-reverse */
	n2 = n/2;
	for (i=1; i < n - 1; i++)
	{
		n1 = n2;
		while ( j >= n1 )
		{
			j = j - n1;
			n1 = n1/2;
		}
		j = j + n1;

		if (i < j)
		{
			t1 = x[i];
			x[i] = x[j];
			x[j] = t1;
			t1 = y[i];
			y[i] = y[j];
			y[j] = t1;
		}
	}


	n1 = 0; /* FFT */
	n2 = 1;

	for (i=0; i < m; i++)
	{
		n1 = n2;
		n2 = n2 + n2;
		e = -6.283185307179586f/n2;
		a = 0.0;

		for (j=0; j < n1; j++)
		{
			c = cos(a);
			s = sin(a);
			a = a + e;

			for (k=j; k < n; k=k+n2)
			{
				t1 = c*x[k+n1] - s*y[k+n1];
				t2 = s*x[k+n1] + c*y[k+n1];
				x[k+n1] = x[k] - t1;
				y[k+n1] = y[k] - t2;
				x[k] = x[k] + t1;
				y[k] = y[k] + t2;
			}
		}
	}
}
#endif


const uint32 _M = 11;
const uint32 _N = 1<<_M;

typedef float fltType;


inline void sincos(float x, float* s, float* c)
{
	*s = sin(x);
	*c = cos(x);
}

inline unsigned revbin_update(unsigned r, unsigned n)
{
	for (unsigned m=n>>1; (!((r^=m)&m)); m>>=1);
	return r;
}

static void revbin_permute(cxNumber<float>* a, uint n)
	// a[0..n-1] input,result
{
	if (n<=2)
		return;
	uint r = 0; // the reversed 0
	for (uint x = 1; x < n; ++x)
	{
		r = revbin_update(r, n); // inline me
		if (r>x)
			Swap(a[x], a[r]);
	}
}

FixedArray_Aligned32<cxNumber<float>, _N > twid4;

static const uint RX = 4; // == r
static const uint LX = 2; // == log(r)/log(p) == log_2(r)
static void dit4l_fft(cxNumber<float> *f, uint ldn, int is)
	// decimation in time radix 4 fft
	// ldn == log_2(n)
{
	uint twiddleIndex = 0;

	float s2pi = ( is>0 ? 2*pi_32 : -2*pi_32 );
	const uint n = (1<<ldn);
	revbin_permute(f, n);
	uint ldm = (ldn&1); // == (log(n)/log(p)) % LX
	if ( ldm!=0 ) // n is not a power of 4, need a radix 2 step
	{
		for (uint r=0; r<n; r+=2)
		{
			cxNumber<float> a0 = f[r];
			cxNumber<float> a1 = f[r+1];
			f[r] = a0 + a1;
			f[r+1] = a0 - a1;
		}
	}
	ldm += LX;
	for ( ; ldm<=ldn ; ldm+=LX)
	{
		uint m = (1<<ldm);
		uint m4 = (m>>LX);
		float ph0 = s2pi/m;
		for (uint j=0; j<m4; j++)
		{
			float phi = j*ph0;
			float c, s, c2, s2, c3, s3;
			sincos(phi, &s, &c);
			sincos(2*phi, &s2, &c2);
			sincos(3*phi, &s3, &c3);

			twid4[twiddleIndex++].Set(c, s);
			twid4[twiddleIndex++].Set(c2, s2);
			twid4[twiddleIndex++].Set(c3, s3);

			cxNumber<float> e = cxNumber<float>(c,s);
			cxNumber<float> e2 = cxNumber<float>(c2,s2);
			cxNumber<float> e3 = cxNumber<float>(c3,s3);
			for (uint r=0, i0=j+r; r<n; r+=m, i0+=m)
			{
				uint i1 = i0 + m4;
				uint i2 = i1 + m4;
				uint i3 = i2 + m4;
				cxNumber<float> a0 = f[i0];
				cxNumber<float> a1 = f[i2]; // (!)
				cxNumber<float> a2 = f[i1]; // (!)
				cxNumber<float> a3 = f[i3];
				a1 *= e;
				a2 *= e2;
				a3 *= e3;
				cxNumber<float> t0 = (a0+a2) + (a1+a3);
				cxNumber<float> t2 = (a0+a2) - (a1+a3);
				cxNumber<float> t1 = (a0-a2) + cxNumber<float>(0,(float)is) * (a1-a3);
				cxNumber<float> t3 = (a0-a2) - cxNumber<float>(0,(float)is) * (a1-a3);
				f[i0] = t0;
				f[i1] = t1;
				f[i2] = t2;
				f[i3] = t3;
			}
		}
	}
}


FixedArray_Aligned32<fltType, _N> fInput1;
FixedArray_Aligned32<fltType, _N> fInput2;
FixedArray_Aligned32<cxNumber<fltType>, _N> cxInput;
FixedArray_Aligned32<cxNumber<fltType>, _N> cxOutput;
FixedArray_Aligned32<fltType, _N> fOutput1;
FixedArray_Aligned32<fltType, _N> fOutput2;
FixedArray_Aligned32<fltType, _N> fTimeOutput1;
FixedArray_Aligned32<fltType, _N> fTimeOutput2;
FixedArray_Aligned32<fltType, _N> fTestOutput1;
FixedArray_Aligned32<fltType, _N> fTestOutput2;

FixedArray_Aligned32<cxNumber<f32_4>, _N> cxInterleaved4;
FixedArray_Aligned32<cxNumber<f32_8>, _N> cxInterleaved8;

FFTL::Convolver<_M, 2, float, float> m_Convolver;
FFTL::Convolver_Slow<float, _N, _N> m_Convolver_Slow;


void verifyFFT()
{
	memset(&fInput1, 0, sizeof(fInput1));
	memset(&fInput2, 0, sizeof(fInput2));
	memset(&cxOutput, 0, sizeof(cxOutput));
	memset(&fOutput1, 0, sizeof(fOutput1));
	memset(&fOutput2, 0, sizeof(fOutput2));
	memset(&fTimeOutput1, 0, sizeof(fTimeOutput1));
	memset(&fTimeOutput2, 0, sizeof(fTimeOutput2));
	memset(&fTestOutput1, 0, sizeof(fTestOutput1));
	memset(&fTestOutput2, 0, sizeof(fTestOutput2));
	memset(&cxInterleaved4, 0, sizeof(cxInterleaved4));
	memset(&cxInterleaved8, 0, sizeof(cxInterleaved8));

#if 0
	fInput1[0] = 1.f;

#elif 0
	for (uint n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n]  = Cos(fltType(2*pi_64) * (3 * fn/_N));
	}
#elif 0
	for (uint n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n]  = fn;
	}
#else
	for (uint n = 0; n < _N; ++n)
	{
		const float f = (float(rand() % 32768) / 16384.f) - 1.f;
		fInput1[n]  = f;
	}
#endif

	memcpy(fTimeOutput1.data, fInput1.data, sizeof(fInput1));

	for (uint n = 0; n < _N; ++n)
	{
		cxInput[n].Set(fInput1[n], 0);
	}

	FFT<_M, float> fftScalar;
	FFT<_M, float, float> fftSimd;

	dit4l_fft(cxInput.data, _M, 1);

	fftScalar.TransformForward_InPlace_DIF(fTimeOutput1, fTimeOutput2);
	fftScalar.TransformInverse_InPlace_DIT(fTimeOutput1, fTimeOutput2);

	//	fftScalar.TransformForward(fInput1, fInput2, fOutput1, fOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT(abs(fDiff) <= 0.000001f);
	}

	memcpy(fTimeOutput1.data, fInput1.data, sizeof(fInput1));
	memset(&fTimeOutput2, 0, sizeof(fTimeOutput2));

	fftSimd.TransformForward_InPlace_DIF(fTimeOutput1, fTimeOutput2);
	fftSimd.TransformInverse_InPlace_DIT(fTimeOutput1, fTimeOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT(abs(fDiff) <= 0.000001f);
	}

	fftSimd.TransformForward(fInput1, fInput2, fOutput1, fOutput2);
//	fftSimd.TransformInverse(fOutput1, fOutput2, fTimeOutput1, fTimeOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT(abs(fDiff) <= 0.000001f);
	}
}

void verifyRealFFT()
{
	kiss_fft_cpx kissOut[_N/2+1];

	memset(&fInput1, 0, sizeof(fInput1));
	memset(&fInput2, 0, sizeof(fInput2));
#if 0
	for (uint n = 0; n < _N; ++n)
	{
		const float fn = (float)n;
		fInput1[n]  = Cos((2*pi_32) * (6 * fn/_N));
	}
#else
	for (uint n = 0; n < _N; ++n)
	{
		const float f = (float(rand() % 32768) / 16384.f) - 1.f;
		fInput1[n]  = f;
	}
#endif

	FixedArray_Aligned32<fltType, _N/2> fHalfOutR;
	FixedArray_Aligned32<fltType, _N/2> fHalfOutI;

	{
		FFT_Real<_M, float> fftSimdReal;
		kiss_fftr_cfg cfgF = kiss_fftr_alloc( _N, false, 0, 0 );
		//	kiss_fftr_cfg cfgI = kiss_fftr_alloc( _N, true, 0, 0 );

		fftSimdReal.TransformForward(fInput1, fHalfOutR, fHalfOutI);
		kiss_fftr(cfgF, fInput1.data, kissOut);

		{
			const float fDiffR = fHalfOutR[0] - kissOut[0].r;
			const float fDiffI = fHalfOutI[0] - kissOut[_N/2].r;
			FFTL_ASSERT(fDiffR == 0 && fDiffI == 0);
		}
		for (uint n = 1; n < _N/2; ++n)
		{
			const float fDiffR = fHalfOutR[n] - kissOut[n].r;
			const float fDiffI = fHalfOutI[n] - kissOut[n].i;
			FFTL_ASSERT(fDiffR <= 0.00001f && fDiffI <= 0.00001f);
		}

		fftSimdReal.TransformInverse(fHalfOutR, fHalfOutI, fOutput1);
		//	kiss_fftri(cfgI, kissOut, fOutput2.data);

		for (uint n = 0; n < _N; ++n)
		{
			const float fDiff = fOutput1[n] - fInput1[n];
			FFTL_ASSERT(fDiff <= 0.00001f);
		}
	}

	{
		FixedArray_Aligned32<f32_8, _N> vInput;
		FixedArray_Aligned32<f32_8, _N/2> vHalfOutR;
		FixedArray_Aligned32<f32_8, _N/2> vHalfOutI;

		for (uint n = 0; n < _N; ++n)
		{
			vInput[n] = ConvertTo<f32_8>(fInput1[n]);
		}

		//memset(&vInput, 0, sizeof(vInput));
		//(__m256&)vInput[0] = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);

		FFT_Real<_M, f32_8, float> fftSingleReal;

		fftSingleReal.TransformForward(vInput, vHalfOutR, vHalfOutI);

		for (uint n = 0; n < _N/2; ++n)
		{
//			FFTL_ASSERT(vHalfOutR[n].GetNative().m256_f32[0] == fHalfOutR[n]);
//			FFTL_ASSERT(vHalfOutI[n].GetNative().m256_f32[0] == fHalfOutI[n]);
			//FFTL_ASSERT(vHalfOutR[n] == fHalfOutR[n]);
		}
	}
}
#if 1
void verifyConvolution()
{
	memset(&fInput1, 0, sizeof(fInput1));

#if 1
	//	fInput1[_N-1] = 1.f;
	fInput1[0] = 1.f;
	//	fInput1[1] = 1.f;
	//	fInput1[4] = 0.3f;
	//	fInput1[10] = -0.7f;
#elif 0
	for (uint n = 0; n < _N; ++n)
	{
		const float f = float(rand() % 32768) / 32768.f;
		fInput1[n] = f;
	}
#elif 1
	for (uint n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n*2]  = Cos(fltType(2*pi_64) * (0 * fn/_N));
	}
#else
	for (uint n = 0; n < _N; ++n)
	{
		fInput1[n] = float(n&1);
	}
#endif

	m_Convolver_Slow.SetKernel(fInput1);
	m_Convolver.InitKernel(&fInput1, sizeof(fInput1));

	for (uint k = 0; k < 1000; ++k)
	{
#if 0
		for (uint n = 0; n < _N; ++n)
		{
			const float f = float(rand() % 32768) / 32768.f;
			fInput1[n] = f;
		}
#elif 1
		memset(&fInput1, 0, sizeof(fInput1));
		fInput1[1] = 1;
#elif 0
		for (uint n = 0; n < _N; ++n)
		{
			const fltType fn = (fltType)n;
			fInput1[n]  = Sin(fltType(2*pi_64) * (4 * fn/_N));
		}
#elif 0
		for (uint n = 0; n < _N; ++n)
		{
			fInput1[n] = float(n&1);
		}
#endif

		m_Convolver_Slow.Convolve(fInput1, fOutput1);
		//		m_Convolver.Convolve(fInput1, fOutput2);

		for (uint n = 0; n < _N; ++n)
		{
			const float fDiff = fOutput2[n] - fOutput1[n];
			FFTL_ASSERT(abs(fDiff) <= 0.00001f);
		}
	}
}
#endif

void perfTest()
{
	memset(&fInput1, 0, sizeof(fInput1));
	memset(&cxOutput, 0, sizeof(cxOutput));
	memset(&fOutput1, 0, sizeof(fOutput1));
	memset(&fOutput2, 0, sizeof(fOutput2));
	memset(&fInput2, 0, sizeof(fInput2));
	memset(&cxInterleaved4, 0, sizeof(cxInterleaved4));
	memset(&cxInterleaved8, 0, sizeof(cxInterleaved8));

	LARGE_INTEGER laFreq, laCounter, laLastTime, laTimeDiff;
	QueryPerformanceFrequency(&laFreq);

	const double fInvUS = 1000000.0 / laFreq.QuadPart;

	FFTL_Printf("\n\n\n");

#if 0
	for (uint32 n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n]  = sin(fltType(2*pi_64) * (10 * fn/_N));
//		fInput1[n] += cos(fltType(2*pi_64) * (345.5354f * fn/_N));
	}
#endif


	const int loopCount = 4096;


	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, f32_8, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformForward_InPlace_DIF(cxInterleaved8);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Forward FFT 8 ch AVX size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}

	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, f32_4, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformForward_InPlace_DIF(cxInterleaved4);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Forward FFT 4 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}


	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, float, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformForward(fInput1, fInput2, fTimeOutput1, fTimeOutput2);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Forward FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}

	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, float, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformInverse(fInput1, fInput2, fTimeOutput1, fTimeOutput2);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Inverse FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}

	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, float, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformForward_InPlace_DIF(fInput1, fInput2);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Forward in-place FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}

	laTimeDiff.QuadPart = 0;
	{
		FFT<_M, float, float> fft;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformInverse_InPlace_DIT(fInput1, fInput2);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Complex Inverse in-place FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}


	{
		FFT_Real<_M, float> fft;
		FixedArray_Aligned32<fltType, _N/2> fHalfOut1;
		FixedArray_Aligned32<fltType, _N/2> fHalfOut2;
	
		laTimeDiff.QuadPart = 0;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformForward(fInput1, fHalfOut1, fHalfOut2);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Real Forward FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);

		laTimeDiff.QuadPart = 0;
		for (int i = 0; i < loopCount; ++i)
		{
			QueryPerformanceCounter(&laLastTime);
			fft.TransformInverse(fHalfOut1, fHalfOut2, fTimeOutput1);
			QueryPerformanceCounter(&laCounter);
			laTimeDiff.QuadPart += laCounter.QuadPart - laLastTime.QuadPart;
		}
		FFTL_Printf("Real Inverse FFT 1 ch SSE size %u: %f us\n", _N, (laTimeDiff.QuadPart * fInvUS) / loopCount);
		fft.PrintTimerInfo(loopCount);
	}

//	_getch();
}

void RunTests()
{
	FFTL::verifyFFT();
	FFTL::verifyRealFFT();
	FFTL::perfTest();
}

} // namespace FFTL
using namespace FFTL;

int main()
{
	const FFTL::f32_4 a(0,1,2,3);
	const FFTL::f32_4 b(4,5,6,7);
	const FFTL::f32_4 vCCNNr0 = FFTL::Permute<sh_X1,sh_X2,sh_Z1,sh_Z2>(a, b);
	const FFTL::f32_4 vCCNNr1 = FFTL::Permute<sh_Y1,sh_Y2,sh_W1,sh_W2>(a, b);


	FFTL::RunTests();

	return 0;
}
#endif


int main()
{
	printf("hello world\n");
	return 0;
}