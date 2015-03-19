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

#ifndef _FFTL_MATH_SSE2_INL
#define _FFTL_MATH_SSE2_INL


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4127 ) //conditional expression is constant
#endif

#define _MM_SHUFFLE_XYZW(x,y,z,w) _MM_SHUFFLE(w,z,y,x)

namespace FFTL
{




template<>
FFTL_FORCEINLINE float Min(float a, float b)
{
	return _mm_cvtss_f32( _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE double Min(double a, double b)
{
	return _mm_cvtsd_f64( _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}

template<>
FFTL_FORCEINLINE float Max(float a, float b)
{
	return _mm_cvtss_f32( _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE double Max(double a, double b)
{
	return _mm_cvtsd_f64( _mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}






FFTL_FORCEINLINE Vec4f V4fZero()
{
	return _mm_setzero_ps();
}
FFTL_FORCEINLINE Vec4f V4fLoadA(const f32* pf)
{
	return _mm_load_ps(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadU(const f32* pf)
{
	return _mm_loadu_ps(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	return _mm_load_ss(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	return _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)pf);
}
FFTL_FORCEINLINE Vec4f V4fLoad3(const f32* pf)
{
	__m128 v = _mm_load_ss(pf+2);
	v = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,0,3,3));
	v = _mm_loadl_pi(v, (const __m64*)pf);
	return v;
}
FFTL_FORCEINLINE void V4fStoreA(f32* pf, Vec4f_In v)
{
	_mm_store_ps(pf, v);
}
FFTL_FORCEINLINE void V4fStoreU(f32* pf, Vec4f_In v)
{
	_mm_storeu_ps(pf, v);
}
FFTL_FORCEINLINE void V4fStore1(f32* pf, Vec4f_In v)
{
	_mm_store_ss(pf, v);
}
FFTL_FORCEINLINE void V4fStore2(f32* pf, Vec4f_In v)
{
	_mm_storel_pi((__m64*)pf, v);
}
FFTL_FORCEINLINE void V4fStore3(f32* pf, Vec4f_In v)
{
	_mm_storel_pi((__m64*)pf, v);
	_mm_store_ss(pf+2, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)));
}
FFTL_FORCEINLINE Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w)
{
	return _mm_setr_ps(x, y, z, w);
}
FFTL_FORCEINLINE Vec4f V4fSplat4(f32 f)
{
	return _mm_set1_ps(f);
}
FFTL_FORCEINLINE Vec4f V4fSplat4(const f32* pf)
{
#if FFTL_AVX
	return _mm_broadcast_ss(pf);
#else
	return _mm_load1_ps(pf);
#endif
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	return _mm_and_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	return _mm_andnot_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	return _mm_or_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	return _mm_xor_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	return _mm_add_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	return _mm_sub_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	return _mm_mul_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
	return _mm_div_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	return _mm_sqrt_ps(v);
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fHSumV(v) );
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	Vec4f r;
#if FFTL_SSE4
	r = _mm_hadd_ps(v, v);
	r = _mm_hadd_ps(r, r);
#else
	r = _mm_add_ps( v, _mm_shuffle_ps(v, v, _MM_SHUFFLE_XYZW(1,0,3,2)) );
	r = _mm_add_ps( r, _mm_shuffle_ps(r, r, _MM_SHUFFLE_XYZW(2,2,0,0)) );
#endif
	return r;
}
FFTL_FORCEINLINE Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpeq_ps(a, b);
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	return _mm_movemask_ps(v);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	return _mm_movemask_ps( _mm_cmpeq_ps(a, b) ) == 15;
}
FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpacklo_ps( a, b );
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpackhi_ps( a, b );
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	return _mm_shuffle_ps(v, v, _MM_SHUFFLE_XYZW(3,2,1,0));
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return _mm_cvtss_f32(v);
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_Y,sh_X,sh_X,sh_X>(v) );
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_Z,sh_X,sh_X,sh_X>(v) );
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_W,sh_X,sh_X,sh_X>(v) );
}



// Converts 0x00010203/0x04050607/0x08090A0B/0x0C0D0E0F and 0x10111213/0x14151617/0x18191A1B/0x1C1D1E1F to 0/1/2/3
#define _SHUFFLEMASK(x) ( (x & 0xC) >> 2 )

template <enShuf _X, enShuf _Y, enShuf _Z, enShuf _W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In v )
{
	FFTL_StaticAssert(	(_X==sh_X || _X==sh_Y || _X==sh_Z || _X==sh_W) &&
		(_Y==sh_X || _Y==sh_Y || _Y==sh_Z || _Y==sh_W) &&
		(_Z==sh_X || _Z==sh_Y || _Z==sh_Z || _Z==sh_W) &&
		(_W==sh_X || _W==sh_Y || _W==sh_Z || _W==sh_W)  );

	FFTL_StaticAssert( !(_X==sh_X && _Y==sh_Y && _Z==sh_Z && _W==sh_W) ); // This permute does nothing meaningful!

	return _mm_shuffle_ps( v, v, _MM_SHUFFLE_XYZW( _X, _Y, _Z, _W ) );
}

//	While this function may look scary, all the branches and switches actually compile away in release builds, leaving only 1 or 2 SSE instructions.
template <enShuf2 _X, enShuf2 _Y, enShuf2 _Z, enShuf2 _W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In a, Vec4f_In b )
{
	//	Template inputs need to be shuffle enumerations sh_(X/Y/Z/W)(1/2)
	FFTL_StaticAssert(
		(_X==sh_X1 || _X==sh_Y1 || _X==sh_Z1 || _X==sh_W1	||
		_X==sh_X2 || _X==sh_Y2 || _X==sh_Z2 || _X==sh_W2 )	&&
		(_Y==sh_X1 || _Y==sh_Y1 || _Y==sh_Z1 || _Y==sh_W1	||
		_Y==sh_X2 || _Y==sh_Y2 || _Y==sh_Z2 || _Y==sh_W2 )	&&
		(_Z==sh_X1 || _Z==sh_Y1 || _Z==sh_Z1 || _Z==sh_W1	||
		_Z==sh_X2 || _Z==sh_Y2 || _Z==sh_Z2 || _Z==sh_W2 )	&&
		(_W==sh_X1 || _W==sh_Y1 || _W==sh_Z1 || _W==sh_W1	||
		_W==sh_X2 || _W==sh_Y2 || _W==sh_Z2 || _W==sh_W2 )	 );

	//	If you try to shuffle this way, you'll just end up with one of the 2 input arguments. Don't do that.
	FFTL_StaticAssert(
		!(_X==sh_X1 && _Y==sh_Y1 && _Z==sh_Z1 && _W==sh_W1) &&
		!(_X==sh_X2 && _Y==sh_Y2 && _Z==sh_Z2 && _W==sh_W2) );

	//	If this error fires, it means you should use the single parameter V4fShuffle function.
	FFTL_StaticAssert(
		!( (_X&0xF0F0F0F0) != 0 && (_Y&0xF0F0F0F0) != 0 && (_Z&0xF0F0F0F0) != 0 && (_W&0xF0F0F0F0) != 0 ) &&
		!( (_X&0xF0F0F0F0) == 0 && (_Y&0xF0F0F0F0) == 0 && (_Z&0xF0F0F0F0) == 0 && (_W&0xF0F0F0F0) == 0 ) );


	const uint shufX = (_X==sh_X1||_X==sh_Y1||_X==sh_Z1||_X==sh_W1) ? 0 : 1;
	const uint shufY = (_Y==sh_X1||_Y==sh_Y1||_Y==sh_Z1||_Y==sh_W1) ? 0 : 1;
	const uint shufZ = (_Z==sh_X1||_Z==sh_Y1||_Z==sh_Z1||_Z==sh_W1) ? 0 : 1;
	const uint shufW = (_W==sh_X1||_W==sh_Y1||_W==sh_Z1||_W==sh_W1) ? 0 : 1;


	//	Catch the easy SSE condition when _X and _Y come from a different vector than _Z and _W.
	// Uses only 1 SSE shuffle instruction.
	if (shufX == shufY && shufZ == shufW)
	{
		if (shufX == 0)	//	_X and _Y from v1. _Z and _W from v2.
			return _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
		else			//	_X and _Y from v2. _Z and _W from v1.
			return _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
	}

	//	Next 4 cases, when we need 2 32 bit words from each vector, but it can't be handled with a single SSE instruction,
	// we use one _mm_shuffle_ps instruction, then one _mm_shuffle_epi32 instruction.
	else if (shufX==0 && shufY==1 && shufZ==0 && shufW==1)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Z), _SHUFFLEMASK(_Y), _SHUFFLEMASK(_W)) );
		return V4fPermute<sh_X,sh_Z,sh_Y,sh_W>(vShuffleTemp);
	}
	else if (shufX==0 && shufY==1 && shufZ==1 && shufW==0)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X),_SHUFFLEMASK( _W), _SHUFFLEMASK(_Y), _SHUFFLEMASK(_Z)) );
		return V4fPermute<sh_X,sh_Z,sh_W,sh_Y>(vShuffleTemp);
	}
	else if (shufX==1 && shufY==0 && shufZ==0 && shufW==1)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Y), _SHUFFLEMASK(_Z), _SHUFFLEMASK(_X), _SHUFFLEMASK(_W)) );
		return V4fPermute<sh_Z,sh_X,sh_Y,sh_W>(vShuffleTemp);
	}
	else if (shufX==1 && shufY==0 && shufZ==1 && shufW==0)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Y), _SHUFFLEMASK(_W), _SHUFFLEMASK(_X), _SHUFFLEMASK(_Z)) );
		return V4fPermute<sh_Z,sh_X,sh_W,sh_Y>(vShuffleTemp);
	}


	//	Next 2 cases, when we need 3 32 bit words from one vector, and 1 word from the other, we use 2 _mm_shuffle_ps instructions.
	else if (
		(shufX==0 && shufY==1 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==0 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==0 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==1 && shufW==0)	)
	{
		//	When we need one word from V1
		if( shufX==0 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), sh_W, _SHUFFLEMASK(_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, b, _MM_SHUFFLE_XYZW(sh_X, sh_Z, _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
		}
		else if( shufY==0 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), sh_W, _SHUFFLEMASK(_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, b, _MM_SHUFFLE_XYZW(sh_X, sh_Z, _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
		}
		else if( shufZ==0 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Z), sh_W, _SHUFFLEMASK(_W), sh_W) );
			return _mm_shuffle_ps( b, vShuffleTemp, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), sh_X, sh_Z) );
		}
		else // shufW==0
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Z), sh_W, _SHUFFLEMASK(_W), sh_W) );
			return _mm_shuffle_ps( b, vShuffleTemp, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), sh_X, sh_Z) );
		}
	}
	else if (
		(shufX==1 && shufY==0 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==1 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==1 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==0 && shufW==1)	)
	{
		//	When we need one word from V2

		if( shufX==1 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), sh_W, _SHUFFLEMASK(_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, a, _MM_SHUFFLE_XYZW(sh_X, sh_Z, _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
		}
		else if( shufY==1 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), sh_W, _SHUFFLEMASK(_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, a, _MM_SHUFFLE_XYZW(sh_X, sh_Z, _SHUFFLEMASK(_Z), _SHUFFLEMASK(_W)) );
		}
		else if( shufZ==1 )
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Z), sh_W, _SHUFFLEMASK(_W), sh_W) );
			return _mm_shuffle_ps( a, vShuffleTemp, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), sh_X, sh_Z) );
		}
		else // shufW==1
		{
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_Z), sh_W, _SHUFFLEMASK(_W), sh_W) );
			return _mm_shuffle_ps( a, vShuffleTemp, _MM_SHUFFLE_XYZW(_SHUFFLEMASK(_X), _SHUFFLEMASK(_Y), sh_X, sh_Z) );
		}
	}

	else
	{
		//	I didn't think of this case. I guess nobody's perfect.
		__debugbreak();
		return _mm_setzero_ps();
	}
}

//	We can have the compiler catch some special cases where we can use a single unpack instruction rather than 2 shuffle instructions.
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X1,sh_X2,sh_Y1,sh_Y2>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeXY( a, b );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X2,sh_X1,sh_Y2,sh_Y1>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeXY( b, a );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z1,sh_Z2,sh_W1,sh_W2>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeZW( a, b );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z2,sh_Z1,sh_W2,sh_W1>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeZW( b, a );
}








FFTL_FORCEINLINE Vec2d V2dZero()
{
	return _mm_setzero_pd();
}
FFTL_FORCEINLINE Vec2d V2dLoadA(const f64* pf)
{
	return _mm_load_pd(pf);
}
FFTL_FORCEINLINE Vec2d V2dLoadU(const f64* pf)
{
	return _mm_loadu_pd(pf);
}
FFTL_FORCEINLINE void V2dStoreA(f64* pf, Vec2d v)
{
	_mm_store_pd(pf, v);
}
FFTL_FORCEINLINE void V2dStoreU(f64* pf, Vec2d_In v)
{
	_mm_storeu_pd(pf, v);
}
FFTL_FORCEINLINE Vec2d V2dSet(f64 x, f64 y)
{
	return _mm_setr_pd(x, y);
}
FFTL_FORCEINLINE Vec2d V2dAnd(Vec2d_In a, Vec2d_In b)
{
	return _mm_and_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b)
{
	return _mm_andnot_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_or_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dXOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_xor_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAdd(Vec2d_In a, Vec2d_In b)
{
	return _mm_add_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSub(Vec2d_In a, Vec2d_In b)
{
	return _mm_sub_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dMul(Vec2d_In a, Vec2d_In b)
{
	return _mm_mul_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dDiv(Vec2d_In a, Vec2d_In b)
{
	return _mm_div_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSqrt(Vec2d_In v)
{
	return _mm_sqrt_pd(v);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec2d_In a, Vec2d_In b)
{
	return _mm_movemask_pd( _mm_cmpeq_pd(a, b) ) == 3;
}


FFTL_FORCEINLINE ScopedFlushDenormals::ScopedFlushDenormals()
{
	m_prevMode = _MM_GET_FLUSH_ZERO_MODE();
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}
FFTL_FORCEINLINE ScopedFlushDenormals::~ScopedFlushDenormals()
{
	_MM_SET_FLUSH_ZERO_MODE(m_prevMode);
}


} // namespace FFTL

#undef _MM_SHUFFLE_XYZW


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //_FFTL_MATH_SSE2_INL