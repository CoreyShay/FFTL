/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Some useful ARM NEON specific inline functions to be shared.

#pragma once

#include <arm_neon.h>


namespace FFTL
{



template<bool bX, bool bY, bool bZ, bool bW>
[[nodiscard]] float32x4_t neon_blend(const float32x4_t& a, const float32x4_t& b);

template<>
[[nodiscard]] float32x4_t neon_blend<0, 0, 0, 0>(const float32x4_t& a, const float32x4_t& b);

template<>
[[nodiscard]] float32x4_t neon_blend<1, 1, 1, 1>(const float32x4_t& a, const float32x4_t& b);

// r = mask ? a : b;
[[nodiscard]] float32x4_t neon_blend(const uint32x4_t& mask, const float32x4_t& a, const float32x4_t& b);

template<bool bX, bool bY, bool bZ, bool bW>
[[nodiscard]] float32x4_t neon_zero_elements(const float32x4_t& v);

template<>
[[nodiscard]] float32x4_t neon_zero_elements<1, 1, 1, 1>(const float32x4_t& v);

[[nodiscard]] float32x4_t neon_negate_ps(const float32x4_t& v);
[[nodiscard]] float32x4_t neon_divide_ps(float32x4_t a, float32x4_t b);
[[nodiscard]] float32x4_t neon_truncate_ps(const float32x4_t& v);

[[nodiscard]] int32x4_t neon_iFloor_ps(const float32x4_t& v);
[[nodiscard]] float32x4_t neon_fFloor_ps(const float32x4_t& v);
[[nodiscard]] int32x4_t neon_iCeil_ps(const float32x4_t& v);
[[nodiscard]] float32x4_t neon_fCeil_ps(const float32x4_t& v);

void neon_transpose4x4(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, float32x4_t& io_d);
void neon_transpose4x3(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, const float32x4_t& in_d);
void neon_transpose3x4(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, float32x4_t& ot_d);


} // namespace FFTL

