/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: vec4 - Vector class with 4 components, with float specialization. SSE used where possible.

#pragma once

#include "Vector.h"


namespace FFTL
{

//
//	Functions unique to vec4
//
vec4	Cross(const vec4& a, const vec4& b, const vec4& c);
void	Transpose4x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& io_d);
vec4	TransformXandY(const vec2& v, const vec4& col0, const vec4& col1);
void	Transpose3x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& ot_d);
void	Transpose3x4_3x3(vec4& io_a, vec4& io_b, vec4& io_c);


} // namespace FFTL

#include "Vector4.inl"

