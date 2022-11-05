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

#include "../Platform/Alloc.h"
#include "../Utils/StdFunctions.h"



namespace FFTL
{


// Detailed constructors
inline FixedBlockMemPool::FixedBlockMemPool(void* pPool, uint maxCount, uint allocSize)
{
	Init(pPool, maxCount, allocSize);
}
inline FixedBlockMemPool::FixedBlockMemPool(uint maxCount, uint allocSize, uint alignment)
{
	Init(maxCount, allocSize, alignment);
}

inline void FixedBlockMemPool::Init(void* pPool, size_type maxCount, size_type allocSize)
{
	FFTL_ASSERT(m_Pool == nullptr);
	FFTL_ASSERT(pPool);

	_MaxCount = safestatic_cast<decltype(_MaxCount)>(maxCount);
	_AllocSize = safestatic_cast<decltype(_AllocSize)>(allocSize);

	m_Pool = static_cast<byte*>(pPool);
	m_FreeIndexStack = static_cast<size_type*>(GetEntry(maxCount));

	//	Sequence the indices
	for (size_type i = 0; i < _MaxCount; ++i)
	{
		m_FreeIndexStack[i] = safestatic_cast<size_type>(i);
	}

#if defined(FFTL_ENABLE_PROFILING)
	//	0xad will be the audio signature
	MemSet(m_Pool, 0xad, allocSize * maxCount);
#endif
}

inline void FixedBlockMemPool::Init(uint maxCount, size_type allocSize, size_type alignment)
{
	allocSize = AlignForward(alignment, allocSize);
	void* pPool = Alloc(allocSize * maxCount + maxCount * sizeof(*m_FreeIndexStack), alignment);
	Init(pPool, maxCount, allocSize);
	m_bPoolOwned = true;
}

inline void FixedBlockMemPool::Shutdown()
{
	if ((m_Pool != nullptr) && m_bPoolOwned)
		Free(m_Pool);
	m_Pool = nullptr;
	m_NumUsedEntries = 0;
	m_bPoolOwned = false;
}

inline FixedBlockMemPool::~FixedBlockMemPool()
{
	Shutdown();
}

// PURPOSE: Gets a pointer to the next free memory space.
// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
inline void* FixedBlockMemPool::Allocate()
{
	size_type usedCount = m_NumUsedEntries;
	if (usedCount != _MaxCount)
	{
		FFTL_ASSERT(usedCount < _MaxCount);
		FFTL_ASSERT(m_FreeIndexStack[usedCount] < _MaxCount);
		void* pType = GetEntry(m_FreeIndexStack[usedCount]);
		m_FreeIndexStack[usedCount] = INDEX_USED_SIGNATURE; // Mark this one as used for debug purposes
		m_NumUsedEntries = ++usedCount;
#if defined(FFTL_ENABLE_PROFILING)
		m_HighwaterMarkEntries = Max(m_HighwaterMarkEntries, usedCount);
		m_TotalAllocs++;
#endif
		return pType;
	}
	return nullptr;
}

// PURPOSE: Marks the allocated space at the pointer location as free.
//			Does not call the object destructor.
// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
//			an instance more than once, or a pointer to an object outside the bounds of our array,
//			although there are some assertions to warn you if you might be.
inline void FixedBlockMemPool::Free(void* pInstance)
{
	FFTL_ASSERT(pInstance <= GetEntry(_MaxCount - 1));
	FFTL_ASSERT(pInstance >= GetEntry(0));
	size_type usedCount = m_NumUsedEntries;
	FFTL_ASSERT(usedCount != 0);
	if (usedCount != 0)
	{
		--usedCount;
		FFTL_ASSERT(m_FreeIndexStack[usedCount] == INDEX_USED_SIGNATURE);
		m_FreeIndexStack[usedCount] = GetIndex(pInstance);
		m_NumUsedEntries = usedCount;
#if defined(FFTL_ENABLE_PROFILING)
		//	0xad will be the audio signature
		memset(pInstance, 0xad, _AllocSize);
		m_TotalFrees++;
#endif
	}
}




// Detailed constructor
inline FixedBlockMemPool_ThreadSafe::FixedBlockMemPool_ThreadSafe(void* pPool, uint maxCount, uint allocSize)
{
	m_Pool = nullptr;
	m_NumUsedEntries = 0;
#if defined(FFTL_ENABLE_PROFILING)
	m_HighwaterMarkEntries = 0;
#endif
	m_bPoolOwned = false;

	Init(pPool, maxCount, allocSize);
}

// Detailed constructor
inline FixedBlockMemPool_ThreadSafe::FixedBlockMemPool_ThreadSafe(uint maxCount, uint allocSize, uint alignment)
{
	m_Pool = nullptr;
	m_NumUsedEntries = 0;
#if defined(FFTL_ENABLE_PROFILING)
	m_HighwaterMarkEntries = 0;
#endif
	m_bPoolOwned = false;

	Init(maxCount, allocSize, alignment);
}

inline void FixedBlockMemPool_ThreadSafe::Init(void* pPool, uint maxCount, uint allocSize)
{
	_Mybase::Init(pPool, maxCount, allocSize);
#if FFTL_FIXEDBLOCKPOOL_ATOMIC && defined(FFTL_ENABLE_PROFILING)
	m_UsedCollisionCount = 0;
	m_EntryCollisionCount = 0;
#endif
}

inline void FixedBlockMemPool_ThreadSafe::Init(uint maxCount, uint allocSize, uint alignment)
{
	_Mybase::Init(maxCount, allocSize, alignment);
#if FFTL_FIXEDBLOCKPOOL_ATOMIC && defined(FFTL_ENABLE_PROFILING)
	m_UsedCollisionCount = 0;
	m_EntryCollisionCount = 0;
#endif
}

// PURPOSE: Gets a pointer to the next free memory space.
// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
inline void* FixedBlockMemPool_ThreadSafe::Allocate()
{
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
	MutexScopedLock lock(&m_Mutex);
	return _Mybase::Allocate();
#else
	void *pType = nullptr;

	//	Increment the used count atomically but only if the pool isn't or has become full.
	size_type usedCount = AtomicLoad(&this->m_NumUsedEntries);
	do FFTL_UNLIKELY
	{
		FFTL_ASSERT(usedCount <= _MaxCount);
	if (usedCount == _MaxCount) FFTL_UNLIKELY
	{
		//	If the pool is full, return early
		return pType;
	}
	} while (![&]()
		{
			const bool ret = AtomicCompareExchangeWeakRef<size_type>(&this->m_NumUsedEntries, usedCount + 1u, usedCount);
#	if defined(FFTL_ENABLE_PROFILING)
			if (!ret)
				AtomicIncrement(&this->m_UsedCollisionCount);
#	endif
			return ret;
		}());

	//	Atomically mark this stack entry as used so that any concurrent free operation will spin before overwriting it.
	size_type pointerIndex;
	while ((pointerIndex = AtomicExchange(&this->m_FreeIndexStack[usedCount], INDEX_USED_SIGNATURE)) == INDEX_USED_SIGNATURE) FFTL_UNLIKELY
	{
#	if defined(FFTL_ENABLE_PROFILING)
		AtomicIncrement(&this->m_EntryCollisionCount);
#	endif
	}

	pType = this->GetEntry(pointerIndex);

#	if defined(FFTL_ENABLE_PROFILING)
	for (size_type highWaterMark = AtomicLoad(&this->m_HighwaterMarkEntries); highWaterMark < usedCount + 1u && AtomicCompareExchangeWeakRef<size_type>(&this->m_HighwaterMarkEntries, usedCount + 1u, highWaterMark);)
	{
	}
	AtomicIncrement(&this->m_TotalAllocs);
#	endif

	return pType;
#endif
}

// PURPOSE: Marks the allocated space at the pointer location as free.
//			Does not call the object destructor.
// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
//			an instance more than once, or a pointer to an object outside the bounds of our array,
//			although there are some assertions to warn you if you might be.
inline void FixedBlockMemPool_ThreadSafe::Free(void* pInstance)
{
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
	MutexScopedLock lock(&m_Mutex);
	_Mybase::Free(pInstance);
#else
	FFTL_ASSERT(this->IsContainedPointer(pInstance));
	FFTL_ASSERT(AtomicLoad(&this->m_NumUsedEntries) > 0);

	if constexpr (FFTL_USING_ASSERTS)
	{
		//	0xad will be the audio signature
		MemSet(pInstance, 0xad, _AllocSize);
	}

	const size_type ptrIndex = this->GetIndex(pInstance);

	//	Atomic decrement
	const size_type usedCount = AtomicDecrement(&this->m_NumUsedEntries) - 1;

	//	Catastrophic assert
	FFTL_ASSERT(usedCount < _MaxCount);

	//	Atomically spin if a concurrent allocate operation hasn't yet marked this stack entry as used.
	while (AtomicCompareExchangeWeak(&this->m_FreeIndexStack[usedCount], ptrIndex, INDEX_USED_SIGNATURE) != INDEX_USED_SIGNATURE) FFTL_UNLIKELY
	{
#	if defined(FFTL_ENABLE_PROFILING)
		AtomicIncrement(&this->m_EntryCollisionCount);
#	endif
	}

#	if defined(FFTL_ENABLE_PROFILING)
	AtomicIncrement(&this->m_TotalFrees);
#	endif

#endif
}


} // namespace FFTL
