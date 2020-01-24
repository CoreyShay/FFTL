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

#pragma once

#include "_pch_Core.h"

#if defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)
#	define FFTL_CPU_INFO_COND_CONSTEXPR constexpr
#else
#	define FFTL_CPU_INFO_COND_CONSTEXPR
#endif
namespace FFTL
{


class CpuInfo
{
public:
	enum class Architecture : u8
	{
		SSE,
		SSE2,
		SSE3,
		SSE4,
		AVX,
		AVX2,
		COUNT
	};

	static constexpr const char* const ARCH_NAMES[] =
	{
		"Architecture::SSE",
		"Architecture::SSE2",
		"Architecture::SSE3",
		"Architecture::SSE4",
		"Architecture::AVX",
		"Architecture::AVX2",
	};
	static_assert(ArraySize(ARCH_NAMES) == underlying_cast(Architecture::COUNT), "Size of ARCH_NAMES vs Architecture enum doesn't match");

	enum class ArchFlags : u8
	{
		DEFAULT	=	0,
		SSE		=	1 << underlying_cast(Architecture::SSE),
		SSE2	=	1 << underlying_cast(Architecture::SSE2),
		SSE3	=	1 << underlying_cast(Architecture::SSE3),
		SSE4	=	1 << underlying_cast(Architecture::SSE4),
		AVX		=	1 << underlying_cast(Architecture::AVX),
		AVX2	=	1 << underlying_cast(Architecture::AVX2),
	};

	enum class Extension : u8
	{
		FMA3,
		FMA4,
		COUNT
	};

	static constexpr const char* const EXT_NAMES[] =
	{
		"Extension::FMA3",
		"Extension::FMA4",
	};
	static_assert(ArraySize(EXT_NAMES) == underlying_cast(Extension::COUNT), "Size of EXT_NAMES vs Extension enum doesn't match");

	enum class ExtFlags : u8
	{
		DEFAULT	=	0,
		FMA3	=	1 << underlying_cast(Extension::FMA3),
		FMA4	=	1 << underlying_cast(Extension::FMA4),
	};

	static constexpr ArchFlags ToFlag(Architecture a) { return static_cast<ArchFlags>(1 << underlying_cast(a)); }
	static constexpr ExtFlags ToFlag(Extension a) { return static_cast<ExtFlags>(1 << underlying_cast(a)); }


	static void Init();
	static FFTL_CPU_INFO_COND_CONSTEXPR ArchFlags GetArchFlags();
	static FFTL_CPU_INFO_COND_CONSTEXPR ExtFlags GetExtFlags();
	static FFTL_CPU_INFO_COND_CONSTEXPR bool GetIsArchitectureEnabled(Architecture arch);
	static FFTL_CPU_INFO_COND_CONSTEXPR bool GetIsExtensionEnabled(Extension ext);
	static FFTL_CPU_INFO_COND_CONSTEXPR ReturnCode SetArchitectureEnabled(Architecture arch, bool bEnable);
	static FFTL_CPU_INFO_COND_CONSTEXPR ReturnCode SetExtensionEnabled(Extension ext, bool bEnable);
	static constexpr const char* GetArchitectureName(Architecture arch);
	static constexpr const char* GetExtensionName(Extension ext);
	static FFTL_CPU_INFO_COND_CONSTEXPR const char* GetHighestArchitectureName(ArchFlags archFlags, Architecture startFrom = Architecture::COUNT - 1);
	static FFTL_CPU_INFO_COND_CONSTEXPR const char* GetHighestArchitectureName();
	static FFTL_CPU_INFO_COND_CONSTEXPR const char* GetHighestExtensionName(ExtFlags extFlags, Extension startFrom = Extension::COUNT - 1);

	static FFTL_SSE_ONLY(constexpr) bool GetSupports_SSE();
	static FFTL_SSE2_ONLY(constexpr) bool GetSupports_SSE2();
	static FFTL_SSE3_ONLY(constexpr) bool GetSupports_SSE3();
	static FFTL_SSE4_ONLY(constexpr) bool GetSupports_SSE4();
	static FFTL_AVX_ONLY(constexpr) bool GetSupports_AVX();
	static FFTL_AVX2_ONLY(constexpr) bool GetSupports_AVX2();

	static FFTL_SIMD_F32x8_ONLY(constexpr) bool GetSupports_SIMD_F32x8();
	static FFTL_SIMD_I32x8_ONLY(constexpr) bool GetSupports_SIMD_I32x8();
	static FFTL_SIMD_F32x4_ONLY(constexpr) bool GetSupports_SIMD_F32x4();
	static FFTL_SIMD_I32x4_ONLY(constexpr) bool GetSupports_SIMD_I32x4();

private:
	static FFTL_CPU_INFO_COND_CONSTEXPR ArchFlags DetectArchitectureSupport();
	static FFTL_CPU_INFO_COND_CONSTEXPR ExtFlags DetectExtensionSupport();

#if defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)
#	if defined(__ORBIS__) || defined(_DURANGO)
	static constexpr ArchFlags	s_archFlags	= ArchFlags::SSE | ArchFlags::SSE2 | ArchFlags::SSE3 | ArchFlags::SSE4 | ArchFlags::AVX;
	static constexpr ExtFlags	s_extFlags	= ExtFlags::DEFAULT;
#	elif defined(__PROSPERO__)
	static constexpr ArchFlags	s_archFlags = ArchFlags::SSE | ArchFlags::SSE2 | ArchFlags::SSE3 | ArchFlags::SSE4 | ArchFlags::AVX | ArchFlags::AVX2;
	static constexpr ExtFlags	s_extFlags = ExtFlags::FMA3;
#	else
	static constexpr ArchFlags	s_archFlags = ArchFlags::DEFAULT;
	static constexpr ExtFlags	s_extFlags = ExtFlags::DEFAULT;
#	endif
#else
	static ArchFlags			s_archFlags;
	static ExtFlags				s_extFlags;
#endif
};


#if defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)
FFTL_FORCEINLINE void CpuInfo::Init()
{
}
#endif

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR CpuInfo::ArchFlags CpuInfo::GetArchFlags()
{
	return s_archFlags;
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR CpuInfo::ExtFlags CpuInfo::GetExtFlags()
{
	return s_extFlags;
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR bool CpuInfo::GetIsArchitectureEnabled(Architecture arch)
{
	return underlying_cast(GetArchFlags() & ToFlag(arch)) != 0;
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR bool CpuInfo::GetIsExtensionEnabled(Extension ext)
{
	return underlying_cast(GetExtFlags() & ToFlag(ext)) != 0;
}

FFTL_FORCEINLINE constexpr const char* CpuInfo::GetArchitectureName(Architecture arch)
{
	return arch < Architecture::COUNT ? ARCH_NAMES[underlying_cast(arch)] : "ARCHITECURE_INVALID";
}

FFTL_FORCEINLINE constexpr const char* CpuInfo::GetExtensionName(Extension ext)
{
	return ext < Extension::COUNT ? EXT_NAMES[underlying_cast(ext)] : "Extension::INVALID";
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR const char* CpuInfo::GetHighestArchitectureName(ArchFlags archFlags, Architecture startFrom)
{
	return
		GetIsArchitectureEnabled(static_cast<Architecture>(startFrom)) ?
		GetArchitectureName(static_cast<Architecture>(startFrom)) :
		GetHighestArchitectureName(archFlags, startFrom - 1);
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR const char* CpuInfo::GetHighestArchitectureName()
{
	return GetHighestArchitectureName(GetArchFlags());
}

FFTL_FORCEINLINE FFTL_CPU_INFO_COND_CONSTEXPR const char* CpuInfo::GetHighestExtensionName(ExtFlags extFlags, Extension startFrom)
{
	return
		GetIsExtensionEnabled(static_cast<Extension>(startFrom)) ?
		GetExtensionName(static_cast<Extension>(startFrom)) :
		GetHighestExtensionName(extFlags, startFrom - 1);
}

#if defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)
FFTL_FORCEINLINE constexpr CpuInfo::ArchFlags CpuInfo::DetectArchitectureSupport()
{
#if defined(__ORBIS__) || defined(_DURANGO)
	return ArchFlags::SSE | ArchFlags::SSE2 | ArchFlags::SSE3 | ArchFlags::SSE4 | ArchFlags::AVX;
#elif defined(__PROSPERO__)
	return ArchFlags::SSE | ArchFlags::SSE2 | ArchFlags::SSE3 | ArchFlags::SSE4 | ArchFlags::AVX | ArchFlags::AVX2;
#else
	return ArchFlags::DEFAULT;
#endif
}
FFTL_FORCEINLINE constexpr CpuInfo::ExtFlags CpuInfo::DetectExtensionSupport()
{
#if defined(__PROSPERO__)
	return ExtFlags::FMA3;
#else
	return ExtFlags::DEFAULT;
#endif
}
#endif



FFTL_FORCEINLINE FFTL_SSE_ONLY(constexpr) bool CpuInfo::GetSupports_SSE()
{
#if defined(FFTL_SSE)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE);
#endif
}
FFTL_FORCEINLINE FFTL_SSE2_ONLY(constexpr) bool CpuInfo::GetSupports_SSE2()
{
#if defined(FFTL_SSE2)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE2);
#endif
}
FFTL_FORCEINLINE FFTL_SSE3_ONLY(constexpr) bool CpuInfo::GetSupports_SSE3()
{
#if defined(FFTL_SSE3)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE3);
#endif
}
FFTL_FORCEINLINE FFTL_SSE4_ONLY(constexpr) bool CpuInfo::GetSupports_SSE4()
{
#if defined(FFTL_SSE4)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE4);
#endif
}
FFTL_FORCEINLINE FFTL_AVX_ONLY(constexpr) bool CpuInfo::GetSupports_AVX()
{
#if defined(FFTL_AVX)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::AVX);
#endif
}
FFTL_FORCEINLINE FFTL_AVX2_ONLY(constexpr) bool CpuInfo::GetSupports_AVX2()
{
#if defined(FFTL_AVX2)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::AVX2);
#endif
}

FFTL_FORCEINLINE FFTL_SIMD_F32x8_ONLY(constexpr) bool CpuInfo::GetSupports_SIMD_F32x8()
{
#if defined(FFTL_SIMD_F32x8)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::AVX2);
#endif
}
FFTL_FORCEINLINE FFTL_SIMD_I32x8_ONLY(constexpr) bool CpuInfo::GetSupports_SIMD_I32x8()
{
#if defined(FFTL_SIMD_I32x8)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::AVX2);
#endif
}
FFTL_FORCEINLINE FFTL_SIMD_F32x4_ONLY(constexpr) bool CpuInfo::GetSupports_SIMD_F32x4()
{
#if defined(FFTL_SIMD_F32x4)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE);
#endif
}
FFTL_FORCEINLINE FFTL_SIMD_I32x4_ONLY(constexpr) bool CpuInfo::GetSupports_SIMD_I32x4()
{
#if defined(FFTL_SIMD_I32x4)
	return true;
#else
	return GetIsArchitectureEnabled(Architecture::SSE2);
#endif
}



}


