/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: vec2 - Vector class with 4 components, 2 usable, with float specialization. SSE used where possible.

#pragma once

#include "Vector.h"

namespace FFTL
{


//
//	Functions unique to vec2
//

//	Maybe not a real thing, but this gets you a vector orthogonal to a
FFTL_NODISCARD vec2 Cross(const vec2& a);



} // namespace FFTL

#include "Vector2.inl"
