/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: vec3 - Vector class with 4 components, with f32 specialization. SSE used where possible.

#pragma once

#include "Vector.h"


namespace FFTL
{


//
//	Functions unique to vec3
//
FFTL_NODISCARD vec3	Cross(const vec3& a, const vec3& b);
void				Transpose3x3(vec3& a, vec3& b, vec3& c);
void				Transpose3x3SaveW(vec3& a, vec3& b, vec3& c); // Preserves the original W values rather than overwriting.
void				Transpose4x3(vec4& io_a, vec4& io_b, vec4& io_c, const vec4& in_d);




} // namespace FFTL

#include "Vector3.inl"
