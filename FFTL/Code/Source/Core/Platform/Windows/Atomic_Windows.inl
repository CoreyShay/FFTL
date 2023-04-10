#pragma once

#if !defined(FFTL_ATOMIC_H)
#	error "Atomic_Windows.inl included without Atomic.h"
#endif

#include <intrin0.inl.h>

namespace FFTL
{

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeWeak(volatile T* p, T newVal, T comp)
{
	if constexpr (std::is_trivial_v<T> && sizeof(T) == 8)
	{
		return static_cast<T>(_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(p), static_cast<__int64>(newVal), static_cast<__int64>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 4)
	{
		return static_cast<T>(_InterlockedCompareExchange(reinterpret_cast<volatile long*>(p), static_cast<long>(newVal), static_cast<long>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 2)
	{
		return static_cast<T>(_InterlockedCompareExchange16(reinterpret_cast<volatile short*>(p), static_cast<short>(newVal), static_cast<short>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 1)
	{
		return static_cast<T>(_InterlockedCompareExchange8(reinterpret_cast<volatile char*>(p), static_cast<char>(newVal), static_cast<char>(comp)));
	}
	else
	{
		std::atomic_compare_exchange_weak(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), &comp, newVal);
		return comp;
	}
}

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeWeak(std::atomic<T>* p, T newVal, T comp)
{
	return AtomicCompareExchangeWeak(const_cast<volatile T*>(reinterpret_cast<T*>(p)), newVal, comp);
}

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeStrong(volatile T* p, T newVal, T comp)
{
	if constexpr (std::is_trivial_v<T> && sizeof(T) == 8)
	{
		return static_cast<T>(_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(p), static_cast<__int64>(newVal), static_cast<__int64>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 4)
	{
		return static_cast<T>(_InterlockedCompareExchange(reinterpret_cast<volatile long*>(p), static_cast<long>(newVal), static_cast<long>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 2)
	{
		return static_cast<T>(_InterlockedCompareExchange16(reinterpret_cast<volatile short*>(p), static_cast<short>(newVal), static_cast<short>(comp)));
	}
	else if constexpr (std::is_trivial_v<T> && sizeof(T) == 1)
	{
		return static_cast<T>(_InterlockedCompareExchange8(reinterpret_cast<volatile char*>(p), static_cast<char>(newVal), static_cast<char>(comp)));
	}
	else
	{
		std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), &comp, newVal);
		return comp;
	}
}
template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeStrong(std::atomic<T>* p, T newVal, T comp)
{
	return AtomicCompareExchangeWeak(p, newVal, comp);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeWeakRef(volatile T* p, const T& newVal, T& inout_comp_result)
{
	const __int64 lo = reinterpret_cast<const __int64*>(&newVal)[0];
	const __int64 hi = reinterpret_cast<const __int64*>(&newVal)[1];
	return _InterlockedCompareExchange128(reinterpret_cast<volatile __int64*>(p), hi, lo, reinterpret_cast<__int64*>(&inout_comp_result));
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
