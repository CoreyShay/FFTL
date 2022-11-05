#pragma once

#if !defined(FFTL_ATOMIC_H)
#error "Atomic_GCC.inl included without Atomic.h"
#endif

namespace FFTL
{

#if 0
template<typename T, typename = typename std::enable_if<std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, T>::type>
FFTL_FORCEINLINE bool AtomicCompareExchangeWeakRef(volatile T* p, const T& newVal, T& inout_comp_result)
{
	const LONG64 lo = reinterpret_cast<const LONG64*>(&newVal)[0];
	const LONG64 hi = reinterpret_cast<const LONG64*>(&newVal)[1];
	return InterlockedCompareExchange128(reinterpret_cast<volatile LONG64*>(p), hi, lo, reinterpret_cast<LONG64*>(&inout_comp_result));
}

template<typename T, typename = typename std::enable_if<std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, T>::type>
FFTL_FORCEINLINE bool AtomicCompareExchangeWeakRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeWeakRef(const_cast<volatile T*>(reinterpret_cast<T*>(p)), newVal, inout_comp_result);
}

template<typename T, typename = typename std::enable_if<std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, T>::type>
FFTL_FORCEINLINE bool AtomicCompareExchangeStrongRef(volatile T* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeWeakRef(p, newVal, inout_comp_result);
}

template<typename T, typename = typename std::enable_if<std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, T>::type>
FFTL_FORCEINLINE bool AtomicCompareExchangeStrongRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeStrongRef(const_cast<volatile T*>(reinterpret_cast<T*>(p)), newVal, inout_comp_result);
}
#endif

} // namespace FFTL
