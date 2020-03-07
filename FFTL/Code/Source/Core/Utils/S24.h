#pragma once

#include "../defs.h"


namespace FFTL
{


struct FFTL_NODISCARD s24
{
	s24();
	constexpr s24(int r);
	/*constexpr*/ operator int() const;

	u8 b[3];
};

FFTL_FORCEINLINE s24::s24()
{
#if defined(FFTL_ENABLE_ASSERT)
	static constexpr u8 BYTES[4] = { 0,1,2,3 };
#	if defined(FFTL_LITTLEENDIAN)
	FFTL_ASSERT_MSG((u32)0x03020100 == *reinterpret_cast<const u32*>(&BYTES), "Little endian expected!");
#	else
	FFTL_ASSERT_MSG((u32)0x00010203 == *reinterpret_cast<const u32*>(&BYTES), "Big endian expected!");
#	endif
#endif
}

FFTL_FORCEINLINE constexpr s24::s24(int r)
#if defined(FFTL_LITTLEENDIAN)
	: b{ (u8)r, (u8)(r >> 8), (u8)(r >> 16) }
#else
	: b{ (u8)(r >> 16), (u8)(r >> 8), (u8)r }
#endif
{
}

FFTL_FORCEINLINE /*constexpr*/ s24::operator int() const
{
#if defined(FFTL_LITTLEENDIAN)
	s32 i = ((s32)b[0] << 8) | ((s32)b[1] << 16) | ((s32)b[2] << 24);
	i >>= 8;
#else
	s32 i = ((s32)b[0] << 24) | ((s32)b[1] << 16) | ((s32)b[2] << 8);
	i >>= 8;
#endif
	return i;
}


}



