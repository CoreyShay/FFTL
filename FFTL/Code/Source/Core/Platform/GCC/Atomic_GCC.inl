#pragma once

#if !defined(FFTL_ATOMIC_H)
#	error "Atomic_GCC.inl included without Atomic.h"
#endif

namespace FFTL
{

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeWeakRef(volatile T* p, const T& newVal, T& inout_comp_result)
{
#if defined(FFTL_ARCHITECTURE_X64)
	const __int64_t lo = reinterpret_cast<const __int64_t*>(&newVal)[0];
	const __int64_t hi = reinterpret_cast<const __int64_t*>(&newVal)[1];
	volatile __int64_t* dest = reinterpret_cast<volatile __int64_t*>(p);
	__int64_t* compare = reinterpret_cast<__int64_t*>(&inout_comp_result);

	unsigned char ret;
	__asm__ __volatile__("lock cmpxchg16b %0; setz %b2"
		: "=m" (dest[0]), "=m" (dest[1]), "=r" (ret),
		  "=a" (compare[0]), "=d" (compare[1])
		: "m" (dest[0]), "m" (dest[1]), "3" (compare[0]), "4" (compare[1]),
		  "c" (hi), "b" (lo));
	return ret;
#else
	static_assert(std::atomic<T>::is_always_lock_free, "Implement a 128 bit CAS using either intrinsics or assembly for this platform right above here.");
	return false;
#endif
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeWeakRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeWeakRef(const_cast<volatile T*>(reinterpret_cast<T*>(p)), newVal, inout_comp_result);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeStrongRef(volatile T* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeWeakRef(p, newVal, inout_comp_result);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeStrongRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeStrongRef(const_cast<volatile T*>(reinterpret_cast<T*>(p)), newVal, inout_comp_result);
}

} // namespace FFTL
