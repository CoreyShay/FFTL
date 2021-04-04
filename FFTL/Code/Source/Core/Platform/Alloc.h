#pragma once

#include "../defs.h"
#include "../Math/MathCommon.h"

#if defined(_POSIX_VERSION)
#	include <stdlib.h>
#else
#	include <malloc.h>
#endif

namespace FFTL
{


[[nodiscard]] inline void* Alloc(size_t size, size_t alignment)
{
	FFTL_ASSERT(size > 0);
#if defined(_MSC_VER)
	return _aligned_malloc(size, alignment);
#else
	void* pRet;
#if FFTL_64BIT
	alignment = Max(alignment, static_cast<size_t>(8));
#else
	alignment = Max(alignment, static_cast<size_t>(4));
#endif
	FFTL_VERIFY_EQ(0, posix_memalign(&pRet, alignment, size));
	return pRet;
#endif
}

template<typename T>
[[nodiscard]] inline T* Alloc(size_t count, size_t alignment = alignof(T))
{
	return static_cast<T*>(Alloc(sizeof(T) * count, alignment));
}

inline void Free(void* pMem)
{
#if defined(_MSC_VER)
	_aligned_free(pMem);
#else
	free(pMem);
#endif
}

} // namespce FFTL


#define FFTL_STACK_ALLOC(__type__, __count__) static_cast<__type__*>(alloca(__count__ * sizeof(__type__)))


