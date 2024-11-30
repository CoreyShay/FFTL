#pragma once

#define FFTL_ATOMIC_H

#include "../defs.h"
#include <atomic>

namespace FFTL
{

template<typename T> void AtomicStore(volatile T* p, T val);
template<typename T> void AtomicStore(std::atomic<T>* p, T val);
template<typename T> FFTL_NODISCARD T AtomicLoad(volatile const T* p);
template<typename T> FFTL_NODISCARD T AtomicLoad(const std::atomic<T>* p);
template<typename T> T AtomicAdd(volatile T* p, T val);
template<typename T> T AtomicAdd(std::atomic<T>* p, T val);
template<typename T> T AtomicSub(volatile T* p, T val);
template<typename T> T AtomicSub(std::atomic<T>* p, T val);
template<typename T> T AtomicIncrement(volatile T* p);
template<typename T> T AtomicIncrement(std::atomic<T>* p);
template<typename T> T AtomicIncrementUnless(volatile T* p, T comp);
template<typename T> T AtomicIncrementUnless(std::atomic<T>* p, T comp);
template<typename T> T AtomicDecrement(volatile T* p);
template<typename T> T AtomicDecrement(std::atomic<T>* p);
template<typename T> T AtomicOr(volatile T* p, T val);
template<typename T> T AtomicOr(std::atomic<T>* p, T val);
template<typename T> T AtomicXOr(volatile T* p, T val);
template<typename T> T AtomicXOr(std::atomic<T>* p, T val);
template<typename T> T AtomicAnd(volatile T* p, T val);
template<typename T> T AtomicAnd(std::atomic<T>* p, T val);
template<typename T> FFTL_NODISCARD T AtomicExchange(volatile T* p, T val);
template<typename T> FFTL_NODISCARD T AtomicExchange(std::atomic<T>* p, T val);
template<typename T> T AtomicCompareExchangeWeak(volatile T* p, T newVal, T comp);
template<typename T> T AtomicCompareExchangeWeak(std::atomic<T>* p, T newVal, T comp);
template<typename T> T AtomicCompareExchangeStrong(volatile T* p, T newVal, T comp);
template<typename T> T AtomicCompareExchangeStrong(std::atomic<T>* p, T newVal, T comp);

template<typename T> typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeWeakRef(volatile T* p, T newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeWeakRef(std::atomic<T>* p, T newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeStrongRef(volatile T* p, T newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<std::atomic<T>::is_always_lock_free || !(std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16), bool>::type AtomicCompareExchangeStrongRef(std::atomic<T>* p, T newVal, T& inout_comp_result);

//	If we get a linker error here, then we need to implement the 128-bit CAS functions for this platform. See Atomic_Windows.inl or Atomic_GCC.inl for examples.
template<typename T> typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeWeakRef(volatile T* p, const T& newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeWeakRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeStrongRef(volatile T* p, const T& newVal, T& inout_comp_result);
template<typename T> typename std::enable_if<!std::atomic<T>::is_always_lock_free && std::is_trivial_v<T> && sizeof(T) == 16 && alignof(T) == 16, bool>::type AtomicCompareExchangeStrongRef(std::atomic<T>* p, const T& newVal, T& inout_comp_result);

template<typename T> const std::atomic<T>& ToAtomic(const T& a) { return reinterpret_cast<const std::atomic<T>&>(a); }
template<typename T> std::atomic<T>& ToAtomic(T& a) { return reinterpret_cast<std::atomic<T>&>(a); }

} // namespace FFTL

#include "Atomic.inl"

#if defined(_MSC_VER)
#include "Windows/Atomic_Windows.inl"
#elif defined(__GNUC__) || defined(__clang__)
#include "GCC/Atomic_GCC.inl"
#endif

#undef FFTL_ATOMIC_H
