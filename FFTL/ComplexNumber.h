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

#ifndef COMPLEXNUMBER_H
#define COMPLEXNUMBER_H

#include "defs.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#endif

namespace FFTL
{



template <typename T>
class alignas(2 * alignof(T)) cxNumber
{
public:
	T r;
	T i;

	FFTL_FORCEINLINE cxNumber() {}
	FFTL_FORCEINLINE cxNumber(const T& real, const T& imag) : r(real), i(imag) {}
	FFTL_FORCEINLINE cxNumber(const cxNumber& c) : r(c.r), i(c.i)		{}
	FFTL_FORCEINLINE cxNumber(const T& init) : r(init), i(init)			{}
	FFTL_FORCEINLINE cxNumber& operator=(const cxNumber& c)				{ r=c.r; i=c.i; return *this; }
	FFTL_FORCEINLINE cxNumber& operator=(const T& init)					{ r=init; i=init; return *this; }
	FFTL_FORCEINLINE bool operator==(const cxNumber& c) const			{ return r==c.r && i==c.i; }
	FFTL_FORCEINLINE bool operator!=(const cxNumber& c) const			{ return r!=c.r || i!=c.i; }
	FFTL_FORCEINLINE bool operator>(const T& f) const					{ return r>f; }
	FFTL_FORCEINLINE bool operator<(const T& f) const					{ return r<f; }
	FFTL_FORCEINLINE void Zero()										{ r=0; i=0; }
	FFTL_FORCEINLINE void Set(const T& real, const T& imag)				{ r=real; i=imag; }

	FFTL_FORCEINLINE cxNumber operator+(const cxNumber& c) const		{ return cxNumber(r+c.r, i+c.i); }
	FFTL_FORCEINLINE cxNumber operator-(const cxNumber& c) const		{ return cxNumber(r-c.r, i-c.i); }
	FFTL_FORCEINLINE cxNumber& operator+=(const cxNumber& c)			{ r+=c.r; i+=c.i; return *this; }
	FFTL_FORCEINLINE cxNumber& operator-=(const cxNumber& c)			{ r-=c.r; i-=c.i; return *this; }

	FFTL_FORCEINLINE cxNumber operator*(const cxNumber& c) const		{ return cxNumber(r*c.r - i*c.i, r*c.i + i*c.r); }
	FFTL_FORCEINLINE cxNumber operator*(const T& n) const				{ return cxNumber(r*n, i*n); }
	FFTL_FORCEINLINE cxNumber& operator*=(const cxNumber& c)			{ return *this = *this * c; }
	FFTL_FORCEINLINE cxNumber& operator*=(const T& n)					{ return *this = *this * n; }

	FFTL_FORCEINLINE cxNumber operator/(const T& n) const				{ return cxNumber(r/n, i/n); }
	FFTL_FORCEINLINE cxNumber& operator/=(const T& n)					{ return *this = *this / n; }

	//	Unary operators
	FFTL_FORCEINLINE cxNumber operator+() const							{ return *this; }
	FFTL_FORCEINLINE cxNumber operator-() const							{ return cxNumber(-r, -i); }

	cxNumber Conj() const												{ return cxNumber(r, -i); }
	cxNumber Flip() const												{ return cxNumber(i, r); }

	FFTL_FORCEINLINE T Dot(const cxNumber& c) const						{ return r*c.r + i*c.i; }
	FFTL_FORCEINLINE T Mag2() const										{ return r*r + i*i; }
	FFTL_FORCEINLINE T Mag() const										{ return sqrt(Mag2()); }
};

typedef cxNumber<f32> cx64;
typedef cxNumber<f64> cx128;


} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // COMPLEXNUMBER_H