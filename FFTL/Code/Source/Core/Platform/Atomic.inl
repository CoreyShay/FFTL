#if !defined(FFTL_ATOMIC_H)
#error "Atomic.inl included without Atomic.h"
#endif



namespace FFTL
{

template<typename T>
FFTL_FORCEINLINE void AtomicStore(volatile T* p, T val)
{
	AtomicStore(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE void AtomicStore(std::atomic<T>* p, T val)
{
	std::atomic_store(p, val);
}

template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T AtomicLoad(volatile const T* p)
{
	return AtomicLoad(reinterpret_cast<const std::atomic<T>*>(const_cast<const T*>(p)));
}

template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T AtomicLoad(const std::atomic<T>* p)
{
	return std::atomic_load(p);
}

template<typename T>
FFTL_FORCEINLINE T AtomicAdd(volatile T* p, T val)
{
	return AtomicAdd(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicAdd(std::atomic<T>* p, T val)
{
	return std::atomic_fetch_add(p, val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicSub(volatile T* p, T val)
{
	return AtomicSub(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicSub(std::atomic<T>* p, T val)
{
	return std::atomic_fetch_sub(p, val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrement(volatile T* p)
{
	return AtomicIncrement(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)));
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrement(std::atomic<T>* p)
{
	return AtomicAdd(p, static_cast<T>(1));
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrementUnless(volatile T* p, T comp)
{
	return AtomicIncrementUnless(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), comp);
}

template<typename T>
FFTL_FORCEINLINE T AtomicIncrementUnless(std::atomic<T>* p, T comp)
{
	T oldVal = AtomicLoad(p);
	while (true)
	{
		const T incVal = oldVal + 1;
		if (incVal >= comp)
			return oldVal;
		if (AtomicCompareExchangeWeakRef(p, incVal, oldVal))
			return oldVal;
	}
}

template<typename T>
FFTL_FORCEINLINE T AtomicDecrement(volatile T* p)
{
	return AtomicDecrement(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)));
}

template<typename T>
FFTL_FORCEINLINE T AtomicDecrement(std::atomic<T>* p)
{
	return AtomicSub(p, static_cast<T>(1));
}

template<typename T>
FFTL_FORCEINLINE T AtomicOr(volatile T* p, T val)
{
	return AtomicOr(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicOr(std::atomic<T>* p, T val)
{
	return std::atomic_fetch_or(p, val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicXOr(volatile T* p, T val)
{
	return AtomicXOr(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicXOr(std::atomic<T>* p, T val)
{
	return std::atomic_fetch_xor(p, val);
}

template<typename T>
FFTL_FORCEINLINE T AtomicAnd(volatile T* p, T val)
{
	return std::atomic_fetch_and(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}
template<typename T>
FFTL_FORCEINLINE T AtomicAnd(std::atomic<T>* p, T val)
{
	return std::atomic_fetch_and(p, val);
}

template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T AtomicExchange(volatile T* p, T val)
{
	return AtomicExchange(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), val);
}

template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T AtomicExchange(std::atomic<T>* p, T val)
{
	return std::atomic_exchange(p, val);
}

#if !defined(_MSC_VER)
template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeWeak(volatile T* p, T newVal, T comp)
{
	return AtomicCompareExchangeWeak(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), newVal, comp);
}

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeWeak(std::atomic<T>* p, T newVal, T comp)
{
	std::atomic_compare_exchange_weak(p, &comp, newVal);
	return comp;
}

template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeStrong(volatile T* p, T newVal, T comp)
{
	return AtomicCompareExchangeWeak(p, newVal, comp);
}
template<typename T>
FFTL_FORCEINLINE T AtomicCompareExchangeStrong(std::atomic<T>* p, T newVal, T comp)
{
	std::atomic_compare_exchange_strong(p, &comp, newVal);
	return comp;
}
#endif

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeWeakRef(volatile T* p, T newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeWeakRef(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), newVal, inout_comp_result);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeWeakRef(std::atomic<T>* p, T newVal, T& inout_comp_result)
{
	return std::atomic_compare_exchange_weak(p, &inout_comp_result, newVal);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeStrongRef(volatile T* p, T newVal, T& inout_comp_result)
{
	return AtomicCompareExchangeStrongRef(reinterpret_cast<std::atomic<T>*>(const_cast<T*>(p)), newVal, inout_comp_result);
}

template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeStrongRef(std::atomic<T>* p, T newVal, T& inout_comp_result)
{
	return std::atomic_compare_exchange_strong(p, &inout_comp_result, newVal);
}


} // namespace FFTL
