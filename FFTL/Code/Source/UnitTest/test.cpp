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

//#include "pch.h"

#define COMPARE_WITH_FFTW 0

#include "../Core/defs.h"
#include "../Core/Math/FFT.h"
#include "../Core/Containers/ListAtomic.h"
#include "../Core/Containers/MemPoolFixedBlock.h"
#include "../Core/Platform/CpuInfo.h"
#include "../Core/Platform/Log.h"
#include "../Core/Platform/Thread.h"
#include "../Core/Platform/Timer.h"
#include "../Core/Math/Vector.h"
#include "../Core/Platform/Atomic.h"
#include "kiss_fft130/kiss_fftr.h"

#if defined(_MSC_VER)
#	pragma warning(disable : 4324) // structure was padded due to alignment specifier
#elif defined(FFTL_PLATFORM_ANDROID)
#	include <android/native_activity.h>
#elif defined(FFTL_PLATFORM_PLAYSTATION)
size_t sceLibcHeapSize = 1000*1024*1024;
unsigned int sceLibcHeapExtendedAlloc = 1;
#endif

#if COMPARE_WITH_FFTW
#	include "fftw/fftw3.h"
#endif

#include <iostream>
#include <stdio.h>
#include <string>
#include "rlutil.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

//#include <intrin.h>


#if 1

namespace FFTL
{

/**********************************************************/
/* fft::c                                                  */
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
void fft(u32 n, u32 m, float x[], float y[])
{
	u32 i,j,k,n1,n2;
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


constexpr u32 _M = 9;
constexpr u32 _N = 1 << _M;

typedef float fltType;


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

//static constexpr uint RX = 4; // == r
static constexpr uint LX = 2; // == log(r)/log(p) == log_2(r)
static void dit4l_fft(cxNumber<float> *f, uint ldn, int is)
	// decimation in time radix 4 fft
	// ldn == log_2(n)
{
	uint twiddleIndex = 0;

	float s2pi = ( is>0 ? 2*PI_32 : -2*PI_32 );
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
			SinCos(phi, s, c);
			SinCos(2*phi, s2, c2);
			SinCos(3*phi, s3, c3);

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


int CvtSd2ToWav(const wchar_t* szPath);


FixedArray_Aligned32<fltType, _N> fInput1;
FixedArray_Aligned32<fltType, _N> fInput2;
FixedArray_Aligned32<cxNumber<fltType>, _N> cxIn;
FixedArray_Aligned32<cxNumber<fltType>, _N> cxOut;
FixedArray_Aligned32<fltType, _N> fOutput1;
FixedArray_Aligned32<fltType, _N> fOutput2;
FixedArray_Aligned32<fltType, _N> fTimeOutput1;
FixedArray_Aligned32<fltType, _N> fTimeOutput2;
FixedArray_Aligned32<fltType, _N> fTestOutput1;
FixedArray_Aligned32<fltType, _N> fTestOutput2;

FixedArray_Aligned32<cxNumber<f32_4>, _N> cxInterleaved4;
FixedArray_Aligned64<cxNumber<f32_8>, _N> cxInterleaved8;

FFTL::Convolver<_M, 4, float, float> m_Convolver;
FFTL::FixedArray<FFTL::Convolver<_M, 4, float, float>::Kernel, 4> m_KernelFD;
FFTL::Convolver_Slow<float, _N, _N * 4> m_Convolver_Slow;

void convolutionTest()
{
	MemZero(fInput1);
	MemZero(fInput2);
	MemZero(cxOut);
	MemZero(fOutput1);
	MemZero(fOutput2);
	MemZero(fTimeOutput1);
	MemZero(fTimeOutput2);
	MemZero(fTestOutput1);
	MemZero(fTestOutput2);
	MemZero(cxInterleaved4);
	MemZero(cxInterleaved8);

	typedef FFT<_M, float> fftScalar;

	FixedArray_Aligned32<fltType, _N*2> input111;
	MemZero(input111);

	for (uint n = 0; n < _N; ++n)
	{
#if 1
		input111[n] = (float(rand() % 32768) / 16384.f) - 1.f;
#else
		input111[n] = -100;
#endif
	}

	for (uint n = 0; n < _N; ++n)
	{
		fInput1[n] = input111[2*n+0];
		fInput2[n] = input111[2*n+1];
	}

//	fInput1[0] = 0;
//	fInput2[0] = 1;
//	fInput1[1] = 2;
//	fInput2[1] = 3;

	fftScalar::TransformForward(fInput1, fInput2, fOutput1, fOutput2);
	FixedArray_Aligned32<cxNumber<fltType>, _N> G;
	for (uint i = 0; i < _N; ++i)
	{
		G[i].r = fOutput1[i];
		G[i].i = fOutput2[i];
	}

	FixedArray_Aligned32<cxNumber<fltType>, _N*2> h;
	MemZero(h);
//	h[2].r = 1;
	h[_N-1].r = -1;
	FixedArray_Aligned32<cxNumber<fltType>, _N*2> H;
	FixedArray_Aligned32<fltType, _N*2> H_re;
	FixedArray_Aligned32<fltType, _N*2> H_im;

	typedef FFT<_M+1, float> fftScalar2N;
	fftScalar2N::TransformForward(h, H_re, H_im);
	for (uint i = 0; i < 2*_N; ++i)
	{
		H[i].r = H_re[i];
		H[i].i = H_im[i];
	}

	FixedArray_Aligned32<cxNumber<fltType>, _N> K1;
	FixedArray_Aligned32<cxNumber<fltType>, _N> K2;

//	K1[0] = (H[0]-H[0].Conj()) * 0.5f;
//	K2[0] = (H[0] * (1.f - fSin) + H[_N-n].Conj() * (1.f + fSin)) * 0.5f;

	for (uint n = 0; n < _N; ++n)
	{
		const float fSin = Sin(PI_32*n/_N);

		K1[n] = (H[n] - H[_N-n].Conj()).Conj() * 0.5f * (Cos(PI_32*n/_N) / _N);
		K2[n] = (H[n] * (1.f + fSin) + H[_N-n].Conj() * (1.f - fSin)).Conj() * (0.5f / _N);
	}

	FixedArray_Aligned32<cxNumber<fltType>, _N> Gh;
	const cxNumber<fltType> ii(0, 1);
	Gh[0] = G[0].Conj() * ii * K1[0] + G[0] * K2[0];
	for (uint n = 1; n < _N; ++n)
	{
		Gh[n] = G[n].Conj() * ii * K1[n] + G[_N-n] * K2[n];
	}

	FixedArray_Aligned32<fltType, _N> Ghr;
	FixedArray_Aligned32<fltType, _N> Ghi;

	for (uint n = 0; n < _N; ++n)
	{
		Ghr[n] = Gh[n].r;
		Ghi[n] = Gh[n].i;
	}

	fftScalar::TransformForward(Ghr, Ghi, fTimeOutput1, fTimeOutput2);
	FixedArray_Aligned32<fltType, _N*2> outFinal;
	for (uint n = 0; n < _N; ++n)
	{
		outFinal[n * 2 + 0] = fTimeOutput1[n];
		outFinal[n * 2 + 1] = fTimeOutput2[n];
	}


	//for (uint i = 0; i < _N; ++i)
	//{
	//	cxNumber<float> c0(fOutput1[i], fOutput1[i]);
	//	c0 *= c0;
	//	fOutput1[i] = c0.r;
	//	fOutput2[i] = c0.i;
	//}

	//fftScalar::TransformInverse(fOutput1, fOutput2, fTimeOutput1, fTimeOutput2);

}

void verifyFFT()
{
	MemZero(fInput1);
	MemZero(fInput2);
	MemZero(cxOut);
	MemZero(fOutput1);
	MemZero(fOutput2);
	MemZero(fTimeOutput1);
	MemZero(fTimeOutput2);
	MemZero(fTestOutput1);
	MemZero(fTestOutput2);
	MemZero(cxInterleaved4);
	MemZero(cxInterleaved8);

#if 0
	fInput1[0] = 1.f;

#elif 0
	for (uint n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n]  = Cos(fltType(2*PI_64) * (3 * fn/_N));
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

	MemCopy(fTimeOutput1, fInput1);

	for (uint n = 0; n < _N; ++n)
	{
		cxIn[n].Set(fInput1[n], 0);
	}

	typedef FFT<_M, float> fftScalar;
	typedef FFT<_M, float, float> fftSimd;

	dit4l_fft(cxIn.data(), _M, 1);

	fftScalar::TransformForward_InPlace_DIF(fTimeOutput1, fTimeOutput2);
	fftScalar::TransformInverse_InPlace_DIT(fTimeOutput1, fTimeOutput2);

	//	fftScalar.TransformForward(fInput1, fInput2, fOutput1, fOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 0.000001f);
	}

	MemCopy(fTimeOutput1, fInput1);
	MemZero(fTimeOutput2);

	fftSimd::TransformForward_InPlace_DIF(fTimeOutput1, fTimeOutput2);
	fftSimd::TransformInverse_InPlace_DIT(fTimeOutput1, fTimeOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 0.000001f);
	}

	fftSimd::TransformForward(fInput1, fInput2, fOutput1, fOutput2);
//	fftSimd::TransformInverse(fOutput1, fOutput2, fTimeOutput1, fTimeOutput2);

	for (uint n = 0; n < _N; ++n)
	{
		const float fDiff = fInput1[n] - (fTimeOutput1[n] / _N);
		FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 0.000001f);
	}

	FFTL_LOG_MSG("verifyFFT: PASS\n");
}

void verifyRealFFT()
{
	auto fFullOut = std::make_unique< FixedArray_Aligned32<fltType, _N> >();
	auto& fHalfOutR = *reinterpret_cast<FixedArray_Aligned32<fltType, _N / 2>*>(fFullOut.get());
	auto& fHalfOutI = *reinterpret_cast<FixedArray_Aligned32<fltType, _N / 2>*>(fFullOut->data() + _N / 2);
	auto pKissOut = std::make_unique< FixedArray_Aligned32<kiss_fft_cpx, _N / 2 + 8> >();
	auto& kissOut = *pKissOut;

	MemZero(fInput1);
	MemZero(fInput2);
#if 0
	for (uint n = 0; n < _N; ++n)
	{
		const float fn = (float)n;
		fInput1[n]  = Cos((2*PI_32) * (6 * fn/_N));
	}
#else
	for (uint n = 0; n < _N; ++n)
	{
		const float f = (float(rand() % 32768) / 16384.f) - 1.f;
		fInput1[n]  = f;
	}
#endif

#if COMPARE_WITH_FFTW
	{
		fftwf_plan planForward = fftwf_plan_r2r_1d(_N, fInput1.data(), fFullOut.data(), FFTW_R2HC, FFTW_ESTIMATE | FFTW_PATIENT);
		fftwf_execute(planForward); /* repeat as needed */
		fftwf_destroy_plan(planForward);
	}
#endif

	{
		typedef FFT_Real<_M, float> fftSimdReal;
		kiss_fftr_cfg cfgF = kiss_fftr_alloc(_N, false, 0, 0);
//		kiss_fftr_cfg cfgI = kiss_fftr_alloc(_N, true, 0, 0);

		fftSimdReal::TransformForward(fInput1, fHalfOutR, fHalfOutI);
		kiss_fftr(cfgF, fInput1.data(), kissOut.data());

		{
			const float fDiffR = fHalfOutR[0] - kissOut[0].r;
			const float fDiffI = fHalfOutI[0] - kissOut[_N / 2].r;
			FFTL_ASSERT_ALWAYS(fDiffR == 0 && fDiffI == 0);
		}
		for (uint n = 1; n < _N / 2; ++n)
		{
			const float fDiffR = fHalfOutR[n] - kissOut[n].r;
			const float fDiffI = fHalfOutI[n] - kissOut[n].i;
			FFTL_ASSERT_ALWAYS(Abs(fDiffR) <= 1 / 16384.f && Abs(fDiffI) <= 1 / 16384.f);
		}

		fftSimdReal::TransformInverse(fHalfOutR, fHalfOutI, fOutput1);
//		kiss_fftri(cfgI, kissOut, fOutput2.data());

		for (uint n = 0; n < _N; ++n)
		{
			const float fDiff = fOutput1[n] - fInput1[n];
			FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 1 / 16384.f);
		}
	}

	{
		auto vInput = std::make_unique< FixedArray_Aligned32<f32_8, _N> >();
		auto vHalfOutR = std::make_unique< FixedArray_Aligned32<f32_8, _N / 2> >();
		auto vHalfOutI = std::make_unique< FixedArray_Aligned32<f32_8, _N / 2> >();

		for (uint n = 0; n < _N; ++n)
		{
			(*vInput)[n] = ConvertTo<f32_8>(fInput1[n]);
		}

		//MemZero(vInput);
		//(__m256&)vInput[0] = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);

		typedef FFT_Real<_M, f32_8, float> fftSingleReal;

		fftSingleReal::TransformForward(*vInput, *vHalfOutR, *vHalfOutI);

		for (uint n = 0; n < _N / 2; ++n)
		{
//			FFTL_ASSERT_ALWAYS(vHalfOutR[n].GetNative().m256_f32[0] == fHalfOutR[n]);
//			FFTL_ASSERT_ALWAYS(vHalfOutI[n].GetNative().m256_f32[0] == fHalfOutI[n]);
			//FFTL_ASSERT_ALWAYS(vHalfOutR[n] == fHalfOutR[n]);
		}
	}

	FFTL_LOG_MSG("verifyRealFFT: PASS\n");
}
#if 1
void verifyConvolution()
{
	FixedArray_Aligned32<fltType, _N*4> fKernel;

	MemZero(fInput1);
	MemZero(fInput2);
	MemZero(fOutput1);
	MemZero(fOutput2);
	MemZero(fKernel);

#if 1
	fKernel[_N*2-1] = 0.8f;
	fKernel[_N*4-1] = -0.6f;
	fKernel[0] = 1.f;
//	fKernel[1] = 1.f;
//	fKernel[4] = 0.3f;
	fKernel[10] = -0.7f;
#elif 0
	for (uint n = 0; n < _N; ++n)
	{
		const float f = float(rand() % 32768) / 32768.f;
		fKernel[n] = f;
	}
#elif 1
	for (uint n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fKernel[n*2]  = Cos(fltType(2*pi_64) * (0 * fn/_N));
	}
#else
	for (uint n = 0; n < _N; ++n)
	{
		fKernel[n] = float(n&1);
	}
#endif

	fKernel[0] += 0.0f;

	m_Convolver_Slow.SetKernel(fKernel);
	m_Convolver.InitKernel(m_KernelFD.data(), fKernel.data(), fKernel.size());

//	for (uint k = 0; k < 1000; ++k)
	{
#if 0
		for (uint n = 0; n < _N; ++n)
		{
			const float f = float(rand() % 32768) / 32768.f;
			fInput2[n] = f;
		}
#elif 1
		MemZero(fInput2);
		fInput2[1] = 1;
#elif 0
		for (uint n = 0; n < _N; ++n)
		{
			const fltType fn = (fltType)n;
			fInput2[n]  = Sin(fltType(2*PI_64) * (4 * fn/_N));
		}
#elif 0
		for (uint n = 0; n < _N; ++n)
		{
			fInput2[n] = float(n&1);
		}
#endif

		m_Convolver_Slow.Convolve(fOutput1, fInput2);
		m_Convolver.Convolve(fOutput2, fInput2, m_KernelFD.data(), m_KernelFD.size());

		for (uint n = 0; n < _N; ++n)
		{
			const float fDiff = fOutput2[n] - fOutput1[n];
			FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 0.005f);
		}

		m_Convolver_Slow.Convolve(fOutput1, fInput2);
		m_Convolver.Convolve(fOutput2, fInput2, m_KernelFD.data(), m_KernelFD.size());

		for (uint n = 0; n < _N; ++n)
		{
			const float fDiff = fOutput2[n] - fOutput1[n];
			FFTL_ASSERT_ALWAYS(Abs(fDiff) <= 0.005f);
		}
	}

	FFTL_LOG_MSG("verifyConvolution: PASS\n");
}
#endif

void perfTest()
{
	MemZero(fInput1);
	MemZero(cxOut);
	MemZero(fOutput1);
	MemZero(fOutput2);
	MemZero(fInput2);
	MemZero(cxInterleaved4);
	MemZero(cxInterleaved8);

	FFTL_LOG_MSG("\n\n\n");

#if 0
	for (u32 n = 0; n < _N; ++n)
	{
		const fltType fn = (fltType)n;
		fInput1[n]  = sin(fltType(2*PI_64) * (10 * fn/_N));
//		fInput1[n] += cos(fltType(2*PI_64) * (345.5354f * fn/_N));
	}
#endif


	const int loopCount = 4096;
	Timer timer;


	{
		typedef FFT<_M, f32_8, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformForward_InPlace_DIF(cxInterleaved8);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Forward FFT 8 ch AVX size %u: %f us\n", _N, timer.GetMicroseconds() / loopCount);
	}

	{
		typedef FFT<_M, f32_4, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformForward_InPlace_DIF(cxInterleaved4);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Forward FFT 4 ch SSE size %u: %f us\n", _N, timer.GetMicroseconds() / loopCount);
	}


	{
		typedef FFT<_M, float, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformForward(fInput1, fInput2, fTimeOutput1, fTimeOutput2);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Forward FFT 1 ch SIMD%u size %u: %f us\n", FFTL_SIMD_F32_WIDTH, _N, timer.GetMicroseconds() / loopCount);
	}

	{
		typedef FFT<_M, float, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformInverse(fInput1, fInput2, fTimeOutput1, fTimeOutput2);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Inverse FFT 1 ch SIMD%u size %u: %f us\n", FFTL_SIMD_F32_WIDTH, _N, timer.GetMicroseconds() / loopCount);
	}

	{
		typedef FFT<_M, float, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformForward_InPlace_DIF(fInput1, fInput2);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Forward in-place FFT 1 ch SIMD%u size %u: %f us\n", FFTL_SIMD_F32_WIDTH, _N, timer.GetMicroseconds() / loopCount);
	}

	{
		typedef FFT<_M, float, float> fft;
		timer.Reset();
		timer.Start();
		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformInverse_InPlace_DIT(fInput1, fInput2);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Complex Inverse in-place FFT 1 ch SIMD%u size %u: %f us\n", FFTL_SIMD_F32_WIDTH, _N, timer.GetMicroseconds() / loopCount);
	}


	{
		typedef FFT_Real<_M, float> fft;
		FixedArray_Aligned32<fltType, _N / 2> fHalfOut1;
		FixedArray_Aligned32<fltType, _N / 2> fHalfOut2;
	
		timer.Reset();
		timer.Start();

		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformForward(fInput1, fHalfOut1, fHalfOut2);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Real Forward FFT 1 ch SSE size %u: %f us\n", _N, timer.GetMicroseconds() / loopCount);

		timer.Reset();
		timer.Start();

		for (int i = 0; i < loopCount; ++i)
		{
			fft::TransformInverse(fHalfOut1, fHalfOut2, fTimeOutput1);
		}
		timer.PauseAccum();
		FFTL_LOG_MSG("Real Inverse FFT 1 ch SSE size %u: %f us\n", _N, timer.GetMicroseconds() / loopCount);

#if COMPARE_WITH_FFTW
		FixedArray_Aligned32<fltType, _N+1> fHalfOutFull;
		{
			fftwf_plan planForward = fftwf_plan_r2r_1d(_N, fInput1.data(), fHalfOutFull.data(), FFTW_R2HC, FFTW_EXHAUSTIVE);

			timer.Reset();
			timer.Start();

			for (int i = 0; i < loopCount; ++i)
			{
				fftwf_execute(planForward); /* repeat as needed */
			}
			timer.PauseAccum();
			fftwf_destroy_plan(planForward);
			FFTL_LOG_MSG("FFTW size %u: %f us\n", _N, timer.GetMicroseconds() / loopCount);
		}
#endif
	}

//	_getch();
}

struct ListTester
{
	FFTL_Define_ListAtomicNode(public, ListTester, m_ListNode);

	u64 m_nUpdateCount = 0;
	u64 m_nAddCount = 0;
	u64 m_nRemCount = 0;

	template <typename T_Functor>
	void Update(T_Functor func)
	{
		AtomicIncrement(&m_nUpdateCount);
		func();
	}
};
void LinkedListThreadSafetyTest()
{
	constexpr uint COUNT = 128;
	constexpr uint ADD_TARGET = COUNT * 3 / 4;
	constexpr uint REM_TARGET = COUNT / 2;

	u64 totalUpdateForCount = 0;
	u64 totalUpdateRevCount = 0;
	u64 nodeCount = 0;
	u64 totalAddCount = 0;
	u64 totalRemCount = 0;

	FixedArray<ListTester, COUNT> testArray;
	FFTL_Define_ListAtomic(ListTester, m_ListNode, testList);
	FixedArray<u8, COUNT> allocated{ false };

	bool terminateAllThreads = false;

	auto funcUpdateFor = [&]()
	{
		while (!terminateAllThreads)
		{
			for (auto&& tester : testList)
			{
				tester.Update( [&]() { AtomicIncrement( &totalUpdateForCount ); } );
			}
		}
	};
	auto funcUpdateRev = [&]()
	{
		while (!terminateAllThreads)
		{
			for (auto&& tester : FFTL::reverse(testList))
			{
				tester.Update( [&]() { AtomicIncrement( &totalUpdateRevCount ); } );
			}
		}
	};

	auto funcAdd = [&]()
	{
		uint targetIndex = 0;
		while (!terminateAllThreads)
		{
			if (nodeCount < ADD_TARGET)
			{
				if (AtomicCompareExchangeStrong<u8>(&allocated[targetIndex], 1, 0) == 0 )
				{
					ListTester* obj = &testArray[targetIndex];
					FFTL_ASSERT_ALWAYS(!obj->m_ListNode.is_linked());
					AtomicIncrement(&obj->m_nAddCount);
					testList.push_back(obj);
					FFTL_ASSERT_ALWAYS(AtomicIncrement(&allocated[targetIndex]) == 1);
					AtomicIncrement(&nodeCount);
					AtomicIncrement(&totalAddCount);
				}
				targetIndex = (targetIndex + 1) % COUNT;
			}
		}
	};

	auto funcRem = [&]()
	{
		uint targetIndex = 0;
		while ( !terminateAllThreads )
		{
			if (nodeCount > REM_TARGET)
			{
				if (AtomicCompareExchangeStrong<u8>(&allocated[targetIndex], 1, 2) == 2)
				{
					ListTester* obj = &testArray[targetIndex];
					obj->m_ListNode.RemoveFromList();
					FFTL_ASSERT_ALWAYS(!obj->m_ListNode.is_linked());
					AtomicIncrement(&obj->m_nRemCount);
					FFTL_ASSERT_ALWAYS(AtomicDecrement(&allocated[targetIndex]) == 1);
					AtomicDecrement(&nodeCount);
					AtomicIncrement(&totalRemCount);
				}
				targetIndex = (targetIndex + 1) % COUNT;
			}
		}
	};

	ThreadFunctor threadFuncUpdate0{ funcUpdateFor };
	ThreadFunctor threadFuncUpdate1{ funcUpdateRev };
	ThreadFunctor threadFuncAdd0{ funcAdd };
	ThreadFunctor threadFuncAdd1{ funcAdd };
	ThreadFunctor threadFuncRem0{ funcRem };
	ThreadFunctor threadFuncRem1{ funcRem };

	threadFuncUpdate0.Start("threadFuncUpdate0");
	threadFuncUpdate1.Start("threadFuncUpdate1");
	threadFuncAdd0.Start("threadFuncAdd0");
	threadFuncAdd1.Start("threadFuncAdd1");
	threadFuncRem0.Start("threadFuncRem0");
	threadFuncRem1.Start("threadFuncRem1");

	for (uint i = 0; i < 10; ++i)
	{
		ThreadSleep(1000);
		FFTL_LOG_MSG("[%s] UpdateFor: %llu, UpdateRev: %llu, Add: %llu, Rem: %llu, NodeCount: %llu\n", __FUNCTION__, totalUpdateForCount, totalUpdateRevCount, totalAddCount, totalRemCount, nodeCount);
	}
	terminateAllThreads = true;

	threadFuncUpdate0.Join();
	threadFuncUpdate1.Join();
	threadFuncAdd0.Join();
	threadFuncAdd1.Join();
	threadFuncRem0.Join();
	threadFuncRem1.Join();

	FFTL_LOG_MSG("[%s] All threads terminated gracefully\n", __FUNCTION__ );
}

void MemPoolThreadSafetyTest()
{
	constexpr uint COUNT = 256;
	constexpr uint ADD_TARGET = COUNT * 3 / 4;
	constexpr uint REM_TARGET = COUNT / 2;

	u64 totalUpdateForCount = 0;
	u64 totalUpdateRevCount = 0;
	u64 nodeCount = 0;
	u64 totalAddCount = 0;
	u64 totalRemCount = 0;
	u64 timeAdd = 0;
	u64 timeRem = 0;

	FixedArray<ListTester*, COUNT> testArray{ nullptr };
	FixedBlockMemPoolStatic_ThreadSafe<ListTester, COUNT> testPool;
	FixedArray<u8, COUNT> allocated{ false };

	bool terminateAllThreads = false;

	auto funcUpdateFor = [&]()
	{
		while (!terminateAllThreads)
		{
			for (auto&& allocFlag : allocated)
			{
				if (AtomicCompareExchangeStrong<u8>(&allocFlag, 3, 2) == 2)
				{
					const auto idx = &allocFlag - allocated.data();
					testArray[idx]->Update([&]() { AtomicIncrement(&totalUpdateForCount); });
					FFTL_ASSERT_ALWAYS(AtomicDecrement(&allocFlag) == 3);
				}
			}
		}
	};
	auto funcUpdateRev = [&]()
	{
		while (!terminateAllThreads)
		{
			for (auto&& allocFlag : FFTL::reverse(allocated))
			{
				if (AtomicCompareExchangeStrong<u8>(&allocFlag, 3, 2) == 2)
				{
					const auto idx = &allocFlag - allocated.data();
					testArray[idx]->Update([&]() { AtomicIncrement(&totalUpdateRevCount); });
					FFTL_ASSERT_ALWAYS(AtomicDecrement(&allocFlag) == 3);
				}
			}
		}
	};

	auto funcAdd = [&]()
	{
		uint targetIndex = 0;
		while (!terminateAllThreads)
		{
			if (nodeCount < ADD_TARGET)
			{
				if (AtomicCompareExchangeStrong<u8>(&allocated[targetIndex], 1, 0) == 0)
				{
					Timer timer;
					timer.Start();
					ListTester* obj = testPool.Construct();
					timer.Stop();
					if (!obj)
						FFTL_ASSERT_ALWAYS(AtomicDecrement(&allocated[targetIndex]) == 1);
					else
					{
						testArray[targetIndex] = obj;
						AtomicIncrement(&obj->m_nAddCount);
						FFTL_ASSERT_ALWAYS(AtomicIncrement(&allocated[targetIndex]) == 1);
						AtomicIncrement(&nodeCount);
						AtomicIncrement(&totalAddCount);
						AtomicAdd(&timeAdd, timer.GetTicks());
					}
				}
				targetIndex = (targetIndex + 1) % COUNT;
			}
		}
	};

	auto funcRem = [&]()
	{
		uint targetIndex = 0;
		while (!terminateAllThreads)
		{
			if (nodeCount > REM_TARGET)
			{
				if (AtomicCompareExchangeStrong<u8>(&allocated[targetIndex], 1, 2) == 2)
				{
					ListTester* obj = testArray[targetIndex];
					testArray[targetIndex] = nullptr;
					AtomicIncrement(&obj->m_nRemCount);
					Timer timer;
					timer.Start();
					testPool.Free(obj);
					timer.Stop();
					FFTL_ASSERT_ALWAYS(AtomicDecrement(&allocated[targetIndex]) == 1);
					AtomicDecrement(&nodeCount);
					AtomicIncrement(&totalRemCount);
					AtomicAdd(&timeRem, timer.GetTicks());
				}
				targetIndex = (targetIndex + 1) % COUNT;
			}
		}
	};

	ThreadFunctor threadFuncUpdate0{ funcUpdateFor };
	ThreadFunctor threadFuncUpdate1{ funcUpdateRev };
	ThreadFunctor threadFuncAdd0{ funcAdd };
	ThreadFunctor threadFuncAdd1{ funcAdd };
	ThreadFunctor threadFuncRem0{ funcRem };
	ThreadFunctor threadFuncRem1{ funcRem };

	threadFuncUpdate0.Start("threadFuncUpdate0");
	threadFuncUpdate1.Start("threadFuncUpdate1");
	threadFuncAdd0.Start("threadFuncAdd0");
	threadFuncAdd1.Start("threadFuncAdd1");
	threadFuncRem0.Start("threadFuncRem0");
	threadFuncRem1.Start("threadFuncRem1");

	for (uint i = 0; i < 10; ++i)
	{
		ThreadSleep(1000);
		FFTL_LOG_MSG("[%s] UpdateFor: %llu, UpdateRev: %llu, Add: %llu, Rem: %llu, NodeCount: %llu\n", __FUNCTION__, totalUpdateForCount, totalUpdateRevCount, totalAddCount, totalRemCount, nodeCount);
	}

	terminateAllThreads = true;

	threadFuncUpdate0.Join();
	threadFuncUpdate1.Join();
	threadFuncAdd0.Join();
	threadFuncAdd1.Join();
	threadFuncRem0.Join();
	threadFuncRem1.Join();

	FFTL_LOG_MSG("[%s] [Performance] Avg Add: %f, Avg Rem: %f \n", __FUNCTION__, Timer::ToMicroseconds(timeAdd) / totalAddCount, Timer::ToMicroseconds(timeRem) / totalRemCount);
	FFTL_LOG_MSG("[%s] All threads terminated gracefully\n", __FUNCTION__);
}

// h must be size [N + M - 1]
void Convolve( const float* f, uint N, const float* g, uint M, float* h )
{
	MemZero( h, N + M );
	for ( uint n = 0; n < N; ++n )
	{
		for ( uint m = 0; m < M; ++m )
		{
			h[n + m] += f[n] * g[m];
		}
	}
}


int RunTests()
{
#if defined(FFTL_PLATFORM_PLAYSTATION)

#	if defined(FFTL_PLATFORM_PROSPERO)
#		define FFTL_TEST_PS_PLATFORM_NAME "Prospero"
#	elif defined(FFTL_PLATFORM_ORBIS)
#		define FFTL_TEST_PS_PLATFORM_NAME "Orbis"
#	endif
	int startResult;
#	if defined(FFTL_BUILD_DEBUG)
	SceKernelModule handle = sceKernelLoadStartModule("/app0/Code/VS14/Generated/Output/" FFTL_TEST_PS_PLATFORM_NAME "/Debug/AudioEngine_" FFTL_TEST_PS_PLATFORM_NAME ".prx", 0, nullptr, 0, nullptr, &startResult);
#	else
	SceKernelModule handle = sceKernelLoadStartModule("/app0/Code/VS14/Generated/Output/" FFTL_TEST_PS_PLATFORM_NAME "/Release/AudioEngine_" FFTL_TEST_PS_PLATFORM_NAME ".prx", 0, nullptr, 0, nullptr, &startResult);
#	endif
	(void)handle;
#endif

	//	Make sure we always flush denormals to zero
	FFTL::SetCpuFlushDenormalMode(true);

#if defined(_MSC_VER)
//	__HrLoadAllImportsForDll("AudioEngine.dll");
#endif

//	FFTL::ImpulseTest();
//	FFTL::ImpulseTest2();
	FFTL::verifyConvolution();
//	FFTL::convolutionTest();
	FFTL::verifyFFT();
	FFTL::verifyRealFFT();
//	FFTL::perfTest();
//	FFTL::LinkedListThreadSafetyTest();
	FFTL::MemPoolThreadSafetyTest();

//	return FFTL::CvtSd2ToWav(L"M:\\Clone Defect");
//	return FFTL::MultitrackTest();
	return 0;
}


} // namespace FFTL
using namespace FFTL;

inline float GetEps(float e)
{
	int a = *reinterpret_cast<int*>(&e) + 1;
	return *reinterpret_cast<float*>(&a) - e;
}

#if defined(_MSC_VER)
#include <Delayimp.h>
#endif


bool bb0[1 << 12];
bool bb1[1 << 12];
bool bb2[1 << 12];
int main()
{
	constexpr uint loopCount = 1024;

	FFTL::Timer timer;

	timer.Start();
	for ( uint t = 0; t < loopCount; ++t )
	{
		for ( uint i = 0; i < sizeof( bb0 ); ++i )
		{
			bb2[i] = (bb0[i] | bb1[i]);
		}
	}
	timer.Stop();
	FFTL_LOG_MSG( "%f us\n", timer.GetMicroseconds() / loopCount );
	FFTL_LOG_MSG( "%u bb2\n", bb2[rand() % sizeof(bb2)]);

	return FFTL::RunTests();
}


#endif


#if defined(FFTL_PLATFORM_WINDOWS)
#include <iostream>
#include <string>
#include <functional>
#include <filesystem>

#include "Core/Platform/File.h"

#endif

