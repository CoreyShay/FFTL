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
#include "_pch_Core.h"

#include "MemPoolFixedBlock.h"
#include "Platform/Alloc.h"
#include "Utils/StdFunctions.h"



namespace FFTL
{


// Default constructor
FixedBlockMemPool::FixedBlockMemPool()
: m_Pool(nullptr)
, m_FreeIndexStack(nullptr)
, m_NumUsedEntries(0)
, _MaxCount(0)
, _AllocSize(0)
#if defined(FFTL_ENABLE_PROFILING)
, m_HighwaterMarkEntries(0)
#endif
, m_bPoolOwned(false)
{
}

// Detailed constructor
FixedBlockMemPool::FixedBlockMemPool(void* pPool, uint maxCount, uint allocSize)
: m_Pool(nullptr)
, m_NumUsedEntries(0)
#if defined(FFTL_ENABLE_PROFILING)
, m_HighwaterMarkEntries(0)
#endif
, m_bPoolOwned(false)
{
	Init(pPool, maxCount, allocSize);
}

FixedBlockMemPool::FixedBlockMemPool(uint maxCount, uint allocSize, uint alignment)
: m_Pool(nullptr)
, m_NumUsedEntries(0)
#if defined(FFTL_ENABLE_PROFILING)
, m_HighwaterMarkEntries(0)
#endif
, m_bPoolOwned(false)
{
	Init(maxCount, allocSize, alignment);
}

void FixedBlockMemPool::Init(void* pPool, size_t maxCount, size_t allocSize)
{
	FFTL_ASSERT(m_Pool == nullptr);
	FFTL_ASSERT(pPool);

	_MaxCount = safestatic_cast<decltype(_MaxCount)>(maxCount);
	_AllocSize = safestatic_cast<decltype(_AllocSize)>(allocSize);

	m_Pool = (byte*)pPool;
	m_FreeIndexStack = (size_type*)GetEntry(maxCount);

	//	Sequence the indices
	for (auto i = 0; i < _MaxCount; ++i)
	{
		m_FreeIndexStack[i] = safestatic_cast<size_type>(i);
	}

#if defined(FFTL_ENABLE_PROFILING)
	//	0xad will be the audio signature
	memset(m_Pool, 0xad, allocSize * maxCount);
#endif
}

void FixedBlockMemPool::Init(uint maxCount, size_t allocSize, size_t alignment)
{
	allocSize = AlignForward(alignment, allocSize);
	void* pPool = Alloc(allocSize*maxCount + maxCount*sizeof(*m_FreeIndexStack), alignment);
	Init(pPool, maxCount, allocSize);
	m_bPoolOwned = true;
}

void FixedBlockMemPool::Shutdown()
{
	if (m_Pool && m_bPoolOwned)
		Free(m_Pool);
	m_Pool = nullptr;
	m_NumUsedEntries = 0;
	m_bPoolOwned = false;
}

FixedBlockMemPool::~FixedBlockMemPool()
{
	Shutdown();
}

// PURPOSE: Gets a pointer to the next free memory space.
// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
void* FixedBlockMemPool::AllocateObject()
{
	size_type usedCount = m_NumUsedEntries;
	if (usedCount != _MaxCount)
	{
		FFTL_ASSERT(usedCount < _MaxCount);
		FFTL_ASSERT(m_FreeIndexStack[usedCount] < _MaxCount);
		void* pType = GetEntry(m_FreeIndexStack[usedCount]);
		m_FreeIndexStack[usedCount] = 0xffff; // Mark this one as used for debug purposes
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
void FixedBlockMemPool::FreeObject(void* pInstance)
{
	FFTL_ASSERT(pInstance <= GetEntry(_MaxCount - 1));
	FFTL_ASSERT(pInstance >= GetEntry(0));
	size_type usedCount = m_NumUsedEntries;
	FFTL_ASSERT(usedCount != 0);
	if (usedCount != 0)
	{
		--usedCount;
		FFTL_ASSERT(m_FreeIndexStack[usedCount] == 0xffff);
		m_FreeIndexStack[usedCount] = GetObjectIndex(pInstance);
		m_NumUsedEntries = usedCount;
#if defined(FFTL_ENABLE_PROFILING)
		//	0xad will be the audio signature
		memset(pInstance, 0xad, _AllocSize);
		m_TotalFrees++;
#endif
	}
}




// Detailed constructor
FixedBlockMemPool_ThreadSafe::FixedBlockMemPool_ThreadSafe(void* pPool, uint maxCount, uint allocSize)
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
FixedBlockMemPool_ThreadSafe::FixedBlockMemPool_ThreadSafe(uint maxCount, uint allocSize, uint alignment)
{
	m_Pool = nullptr;
	m_NumUsedEntries = 0;
#if defined(FFTL_ENABLE_PROFILING)
	m_HighwaterMarkEntries = 0;
#endif
	m_bPoolOwned = false;

	Init(maxCount, allocSize, alignment);
}

void FixedBlockMemPool_ThreadSafe::Init(void* pPool, uint maxCount, uint allocSize)
{
	_Mybase::Init(pPool, maxCount, allocSize);
#if SL_FIXEDBLOCKPOOL_MUTEX_FREE
	m_HeadIndex = 0;
	m_TailIndex = 0;
#if defined(FFTL_ENABLE_PROFILING)
	m_CollisionCount = 0;
#endif
#endif
}

void FixedBlockMemPool_ThreadSafe::Init(uint maxCount, uint allocSize, uint alignment)
{
	_Mybase::Init(maxCount, allocSize, alignment);
#if SL_FIXEDBLOCKPOOL_MUTEX_FREE
	m_HeadIndex = 0;
	m_TailIndex = 0;
#if defined(FFTL_ENABLE_PROFILING)
	m_CollisionCount = 0;
#endif
#endif
}

// PURPOSE: Gets a pointer to the next free memory space.
// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
void* FixedBlockMemPool_ThreadSafe::AllocateObject()
{
#if !SL_FIXEDBLOCKPOOL_MUTEX_FREE
	MutexScopedLock lock(&m_Mutex);
	return _Mybase::AllocateObject();
#else
	const size_type usedCount = slAtomicFetchInc(&this->m_NumUsedEntries);
	if (usedCount < _MaxCount)
	{
#if defined(FFTL_ENABLE_PROFILING)
		slCompareExchange(&this->m_HighwaterMarkEntries, usedCount, usedCount + 1);
		slAtomicInc(&this->m_TotalAllocs);
#endif // !_PRODUCTION

	_RESTART_ADD:
		//	Increment the head index of the queue
		const size_type stackIndex = slAtomicFetchInc(&this->m_HeadIndex) & (_MaxCount - 1);

		//	Mark this one as used and signal FreeObject to continue if it was waiting on this index
		volatile size_type* pOldEntryIndex = this->m_FreeIndexStack + stackIndex;

		//	If the unlikely scenario occurs where we've waited in this thread long enough to the point where other threads have spun
		// around us, we can just retry getting the stack index.
		const size_type entryIndex = slXchg(pOldEntryIndex, 0xffff);
		if (slUNLIKELY(entryIndex == 0xffff))
		{
#if defined(FFTL_ENABLE_PROFILING)
			slAtomicInc(&this->m_CollisionCount);
#endif // !_PRODUCTION
			goto _RESTART_ADD;
		}

		FFTL_ASSERT(stackIndex < _MaxCount);
		FFTL_ASSERT(entryIndex < _MaxCount);
		void* pType = this->GetEntry(entryIndex);
		return pType;
	}
	slAtomicDec(&this->m_NumUsedEntries);
	return nullptr;
#endif
}

// PURPOSE: Marks the allocated space at the pointer location as free.
//			Does not call the object destructor.
// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
//			an instance more than once, or a pointer to an object outside the bounds of our array,
//			although there are some assertions to warn you if you might be.
void FixedBlockMemPool_ThreadSafe::FreeObject(void* pInstance)
{
#if !SL_FIXEDBLOCKPOOL_MUTEX_FREE
	MutexScopedLock lock(&m_Mutex);
	_Mybase::FreeObject(pInstance);
#else
#if defined(FFTL_ENABLE_PROFILING)
	//	0xad will be the audio clearing signature
	memset(pInstance, 0xad, _AllocSize);
#endif
	FFTL_ASSERT(pInstance <= this->GetEntry(_MaxCount - 1));
	FFTL_ASSERT(pInstance >= this->GetEntry(0));

#if defined(FFTL_ENABLE_PROFILING)
	slAtomicInc(&this->m_TotalFrees);
#endif // !_PRODUCTION

_RESTART_ADD:
	//	Increment the tail index of the queue
	const size_type stackIndex = slAtomicFetchInc(&this->m_TailIndex) & (_MaxCount - 1);
	const size_type blockIndex = GetObjectIndex(pInstance);
	volatile size_type* pEntryIndex = this->m_FreeIndexStack + stackIndex;

	//	If the unlikely scenario occurs where we've waited in this thread long enough to the point where other threads have spun
	// around us, we can just retry getting the stack index.
	if (slUNLIKELY(!slCompareAndSwap(pEntryIndex, 0xffff, blockIndex)))
	{
#if defined(FFTL_ENABLE_PROFILING)
		slAtomicInc(&this->m_CollisionCount);
#endif // !_PRODUCTION
		goto _RESTART_ADD;
	}

	//	Decrement the used entries atomically
	const size_type usedCount = slAtomicFetchDec(&this->m_NumUsedEntries);
	FFTL_ASSERT(usedCount != 0);
	slUNREF(usedCount);
#endif
}


} // namespace FFTL
