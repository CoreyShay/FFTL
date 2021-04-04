#pragma once

#include "../defs.h"
#include <atomic>

namespace FFTL
{

template<typename T>
FFTL_FORCEINLINE void AtomicStore(volatile T* p, T val)
{
	std::atomic_store(reinterpret_cast<std::atomic<T>*>(p), val);
}

template<typename T>
[[nodiscard]] FFTL_FORCEINLINE T AtomicLoad(volatile const T* p)
{
	return std::atomic_load(reinterpret_cast<const std::atomic<T>*>(const_cast<const T*>(p)));
}

template<typename T>
[[nodiscard]] FFTL_FORCEINLINE T AtomicExchange(T* p, T val)
{
	return std::atomic_exchange(reinterpret_cast<std::atomic<T>*>(p), val);
}

template<typename T>
[[nodiscard]] FFTL_FORCEINLINE T AtomicExchange(volatile T* p, T val)
{
	return AtomicExchange(const_cast<T*>(p), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchange(volatile T* p, T val, T comp)
{
	std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), &comp, val);
	return comp;
}

template<typename T>
FFTL_FORCEINLINE T AtomicAdd(volatile T* p, T val)
{
	return std::atomic_fetch_add(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicSub(volatile T* p, T val)
{
	return std::atomic_fetch_sub(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrement(volatile T* p)
{
	return AtomicAdd(const_cast<T*>(p), static_cast<T>(1));
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrementUnless(volatile T* p, T comp)
{
	while (true)
	{
		const T oldVal = AtomicLoad(p);
		const T newVal = oldVal + 1;
		if (newVal >= comp || AtomicCompareExchange(p, newVal, oldVal) == oldVal)
			return oldVal;
	}
}

template<typename T>
FFTL_FORCEINLINE T AtomicDecrement(volatile T* p)
{
	return AtomicSub(p, static_cast<T>(1));
}

template<typename T>
FFTL_FORCEINLINE T AtomicOr(volatile T* p, T val)
{
	return std::atomic_fetch_or(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicXOr(volatile T* p, T val)
{
	return std::atomic_fetch_xor(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

} // namespce FFTL


#define FFTL_STACK_ALLOC(__type__, __count__) static_cast<__type__*>(alloca(__count__ * sizeof(__type__)))


