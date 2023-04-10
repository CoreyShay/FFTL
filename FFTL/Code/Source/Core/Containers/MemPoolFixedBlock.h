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

#include "../Platform/Atomic.h"
#include "../Utils/MetaProgramming.h"

//	Preferred. Faster than mutexes.
#define FFTL_FIXEDBLOCKPOOL_ATOMIC 1

#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
#	include "../Platform/Mutex.h"
#endif


namespace FFTL
{


//	A memory pool of fixed size known at compile time.
//	All objects in the pool will be allocated a single fixed memory size,
//	so _AllocSize must be greater than or equal to the largest inherited class.
template <class _BaseType, uint _MaxCount, uint _AllocSize = sizeof(_BaseType)>
class FixedBlockMemPoolStatic
{
public:
	using size_type = typename std::conditional<_MaxCount < 256, u8, typename std::conditional<_MaxCount < 65536, u16, u32>::type>::type; // Ensures size_type is the smallest possible unsigned integer type
	static constexpr size_type INDEX_USED_SIGNATURE = static_cast<size_type>(~0);

	// Default constructor
	FixedBlockMemPoolStatic()
	{
		static_assert(_MaxCount < (1 << (sizeof(m_NumUsedEntries) * 8)), "_MaxCount must be smaller than the integral size type can store");
		static_assert(_AllocSize >= static_cast<uint>(sizeof(_BaseType)), "_AllocSize must be at least as large as the base type");

		//	Sequence the indices
		for (uint i = 0; i < _MaxCount; ++i)
		{
			m_FreeIndexStack[i] = (size_type)i;
		}

#if defined(FFTL_ENABLE_PROFILING)
		//	0xad will be the audio signature
		MemSet(m_Pool, 0xad, _AllocSize * _MaxCount);
#endif
	}

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	template <class _InheritedType, typename... E>
	FFTL_NODISCARD _InheritedType* Construct(E&& ...constructArgs)
	{
		static_assert(_AllocSize >= static_cast<uint>(sizeof(_InheritedType)), "_AllocSize must be at least as large as the inherited type");

		size_type usedCount = m_NumUsedEntries;
		if (usedCount != _MaxCount)
		{
			FFTL_ASSERT(usedCount < _MaxCount);
			FFTL_ASSERT(m_FreeIndexStack[usedCount] < _MaxCount);
			_InheritedType* pType = static_cast<_InheritedType*>( GetEntry( m_FreeIndexStack[usedCount] ) );
			m_FreeIndexStack[usedCount] = INDEX_USED_SIGNATURE; // Mark this one as used for debug purposes
			m_NumUsedEntries = ++usedCount;
#if defined(FFTL_ENABLE_PROFILING)
			m_HighwaterMarkEntries = Max(m_HighwaterMarkEntries, usedCount);
			m_TotalAllocs++;
#endif
			//	Call the constructor
			::new (pType) _InheritedType(std::forward<E>(constructArgs)...);

			return pType;
		}
		return nullptr;
	}

	template <typename... E>
	FFTL_NODISCARD _BaseType* Construct(E&& ...constructArgs) { return Construct<_BaseType>(std::forward<E>(constructArgs)...); }

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	template <class _InheritedType> void Free(_InheritedType* pInstance)
	{
		FFTL_ASSERT(IsContainedPointer(pInstance));

		//	Call the destructor
		pInstance->~_InheritedType();

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
			MemSet(pInstance, 0xad, _AllocSize);
			m_TotalFrees++;
#endif
		}
	}

	// RETURNS:	True if this is a valid heap pointer owned by this heap.
	// Note that this only performs a range check to see if this is within
	// our heap, because if this pointer has already been freed, it will still
	// return true as long as it's within the heap bounds.
	FFTL_NODISCARD bool IsContainedPointer(const void* pInstance) const
	{
		return ((const _BaseType*)pInstance >= GetEntry(0)) && ((const _BaseType*)pInstance <= GetEntry(_MaxCount - 1));
	}

	// RETURNS:	The index into the pool corresponding to the pointer location
	FFTL_NODISCARD size_type GetIndex(const _BaseType* pInstance) const
	{
		FFTL_ASSERT(IsContainedPointer(pInstance));
		return size_type((tAllocStub*)pInstance - m_Pool.data());
	}

	FFTL_NODISCARD static constexpr uint GetMaxCount()	{ return _MaxCount; }
	FFTL_NODISCARD static constexpr uint GetHeapSize()	{ return _MaxCount*_AllocSize; }
	FFTL_NODISCARD static constexpr uint GetAllocSize()	{ return _AllocSize; }

	FFTL_NODISCARD bool GetIsEmpty() const				{ return m_NumUsedEntries == 0; }
	FFTL_NODISCARD bool GetIsFull() const				{ return m_NumUsedEntries == _MaxCount; }
	FFTL_NODISCARD uint GetNumUsed() const				{ return m_NumUsedEntries; }
	FFTL_NODISCARD uint GetNumAvailable() const			{ return _MaxCount - m_NumUsedEntries; }
	FFTL_NODISCARD uint GetMemoryUsed() const			{ return m_NumUsedEntries*_AllocSize; }
	FFTL_NODISCARD uint GetMemoryAvailable() const		{ return GetNumAvailable()*_AllocSize; }

#if defined(FFTL_ENABLE_PROFILING)
	FFTL_NODISCARD size_type	GetHighwaterMarkEntries() const		{ return m_HighwaterMarkEntries; }
	FFTL_NODISCARD u32			GetTotalAllocs() const				{ return m_TotalAllocs; }
	FFTL_NODISCARD u32			GetTotalFrees() const				{ return m_TotalFrees; }
#endif // defined(FFTL_ENABLE_PROFILING)

	FFTL_NODISCARD _BaseType*		GetEntry(uint i) { return reinterpret_cast<_BaseType*>(&m_Pool[i]); }
	FFTL_NODISCARD const _BaseType*	GetEntry(uint i) const { return reinterpret_cast<const _BaseType*>(&m_Pool[i]); }

protected:
	struct alignas(_BaseType) tAllocStub { byte m_Bytes[_AllocSize]; };

	FixedArray<tAllocStub, _MaxCount>	m_Pool;
	size_type							m_FreeIndexStack[_MaxCount];
	size_type							m_NumUsedEntries = 0;
#if defined(FFTL_ENABLE_PROFILING)
	size_type							m_HighwaterMarkEntries = 0;
	u32									m_TotalAllocs = 0;
	u32									m_TotalFrees = 0;
#endif
};


//	This version contains a mutex which it locks for allocations and frees.
template <class _BaseType, uint _MaxCount, uint _AllocSize = sizeof(_BaseType)>
class FixedBlockMemPoolStatic_ThreadSafe : public FixedBlockMemPoolStatic<_BaseType, _MaxCount, _AllocSize>
{
	typedef FixedBlockMemPoolStatic_ThreadSafe<_BaseType, _MaxCount, _AllocSize> _Myclass;
	typedef FixedBlockMemPoolStatic<_BaseType, _MaxCount, _AllocSize> _Mybase;
	using typename _Mybase::size_type;
	using _Mybase::INDEX_USED_SIGNATURE;

public:
	FixedBlockMemPoolStatic_ThreadSafe()
		: _Mybase()
	{
	}

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	template <class _InheritedType, typename... E>
	FFTL_NODISCARD _InheritedType* Construct(E&& ...constructArgs)
	{
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
		MutexScopedLock lock(&m_Mutex);
		return _Mybase::template Construct<_InheritedType>(std::forward<E>(constructArgs)...);
#else
		static_assert(_AllocSize >= static_cast<uint>(sizeof(_InheritedType)), "_AllocSize must be at least as large as the inherited type");

		_InheritedType *pType = nullptr;

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

		//	Call the constructor
		::new (pType) _InheritedType(std::forward<E>(constructArgs)...);

		return pType;
#endif
	}

	template <typename... E>
	FFTL_NODISCARD _BaseType* Construct(E&& ...constructArgs) { return Construct<_BaseType>(std::forward<E>(constructArgs)...); }

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	template <class _InheritedType>
	void Free(_InheritedType* pInstance)
	{
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
		MutexScopedLock lock(&m_Mutex);
		_Mybase::Free(pInstance);
#else
		FFTL_ASSERT(this->IsContainedPointer(pInstance));
		FFTL_ASSERT(AtomicLoad(&this->m_NumUsedEntries) > 0);

		//	Call the destructor
		pInstance->~_InheritedType();

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


#if defined(FFTL_ENABLE_PROFILING)
#	if FFTL_FIXEDBLOCKPOOL_ATOMIC
	FFTL_NODISCARD u32					GetUsedCollisionCount() const	{ return m_UsedCollisionCount; }
	FFTL_NODISCARD u32					GetEntryCollisionCount() const	{ return m_EntryCollisionCount; }
#	else
	FFTL_NODISCARD static constexpr u32	GetUsedCollisionCount()			{ return 0; }
	FFTL_NODISCARD static constexpr u32	GetEntryCollisionCount()		{ return 0; }
#	endif
#endif // defined(FFTL_ENABLE_PROFILING)

private:
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
	Mutex						m_Mutex;
#elif defined(FFTL_ENABLE_PROFILING)
	u32							m_UsedCollisionCount = 0;
	u32							m_EntryCollisionCount = 0;
#endif
};




//	A memory pool of fixed size not known at compile time.
//	All objects in the pool will be allocated a single fixed memory size,
//	so _AllocSize must be greater than or equal to the largest inherited class.
class FixedBlockMemPool
{
public:
	typedef u32 size_type;
	static constexpr size_type INDEX_USED_SIGNATURE = static_cast<size_type>(~0);

	// Default constructor
	FixedBlockMemPool() = default;

	// Detailed constructor
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount * allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount * allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount * allocSize
	FixedBlockMemPool(void* pPool, uint maxCount, uint allocSize);

	// Detailed constructor
	// maxCount:	Maximum number of allocations we can handle.
	// allocSize:	The block size of the allocation desired.
	// alignment:	The desired alignment boundary of the allocations.
	FixedBlockMemPool(uint maxCount, uint allocSize, uint alignment);

	//	Copies / moves not allowed
	FixedBlockMemPool(const FixedBlockMemPool&) = delete;
	FixedBlockMemPool(FixedBlockMemPool&&) = delete;
	FixedBlockMemPool& operator=(const FixedBlockMemPool&) = delete;
	FixedBlockMemPool& operator=(FixedBlockMemPool&&) = delete;


	// PURPOSE:		Initializes the pool if we used a default constructor.
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount * allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount * allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount * allocSize
	void Init(void* pPool, size_type maxCount, size_type allocSize);

	// PURPOSE:		Initializes the pool if we used a default constructor.
	// maxCount:	Maximum number of allocations we can handle.
	// allocSize:	The block size of the allocation desired.
	// alignment:	The desired alignment boundary of the allocations.
	void Init(uint maxCount, size_type allocSize, size_type alignment);

	// PURPOSE:		Tears down the pool and frees the contained memory if it is owned.
	void Shutdown();

	// PURPOSE:		Tears down the pool and frees the contained memory if it is owned.
	~FixedBlockMemPool();

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	FFTL_NODISCARD void* Allocate();

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	void Free(void* pInstance);

	// RETURNS:	True if this is a valid heap pointer owned by this heap.
	// Note that this only performs a range check to see if this is within
	// our heap, because if this pointer has already been freed, it will still
	// return true as long as it's within the heap bounds.
	bool IsContainedPointer(const void* pInstance) const
	{
		return (pInstance >= GetEntry(0)) && (pInstance <= GetEntry(_MaxCount - 1));
	}

	// RETURNS:	The index into the pool corresponding to the pointer location
	FFTL_NODISCARD size_type GetIndex(const void* pInstance)
	{
		FFTL_ASSERT(IsContainedPointer(pInstance));
		return size_type( ((byte*)pInstance - m_Pool) / _AllocSize );
	}

	FFTL_NODISCARD size_type	GetMaxCount()					{ return _MaxCount; }
	FFTL_NODISCARD size_type	GetHeapSize()					{ return _MaxCount * _AllocSize; }
	FFTL_NODISCARD size_type	GetAllocSize()					{ return _AllocSize; }

	FFTL_NODISCARD bool			GetIsEmpty() const				{ return m_NumUsedEntries == 0; }
	FFTL_NODISCARD bool			GetIsFull() const				{ return m_NumUsedEntries == _MaxCount; }
	FFTL_NODISCARD size_type	GetNumUsed() const				{ return m_NumUsedEntries; }
	FFTL_NODISCARD size_type	GetNumAvailable() const			{ return _MaxCount - m_NumUsedEntries; }
	FFTL_NODISCARD uint			GetMemoryUsed() const			{ return m_NumUsedEntries * _AllocSize; }
	FFTL_NODISCARD uint			GetMemoryAvailable() const		{ return GetNumAvailable() * _AllocSize; }

#if defined(FFTL_ENABLE_PROFILING)
	FFTL_NODISCARD size_type	GetHighwaterMarkEntries() const	{ return m_HighwaterMarkEntries; }
	FFTL_NODISCARD u32			GetTotalAllocs() const			{ return m_TotalAllocs; }
	FFTL_NODISCARD u32			GetTotalFrees() const			{ return m_TotalFrees; }
#endif // defined(FFTL_ENABLE_PROFILING)

protected:
	FFTL_NODISCARD void*		GetEntry(size_type i)		{ FFTL_ASSERT(i < _MaxCount); return &m_Pool[i * _AllocSize]; }
	FFTL_NODISCARD const void*	GetEntry(size_type i) const	{ FFTL_ASSERT(i < _MaxCount); return &m_Pool[i * _AllocSize]; }

	byte*			m_Pool = nullptr;
	size_type*		m_FreeIndexStack = nullptr;
	size_type		m_NumUsedEntries = 0;
	size_type		_MaxCount = 0;
	size_type		_AllocSize = 0;
#if defined(FFTL_ENABLE_PROFILING)
	size_type		m_HighwaterMarkEntries = 0;
	u32				m_TotalAllocs = 0;
	u32				m_TotalFrees = 0;
#endif
	bool			m_bPoolOwned = false;
};


//	This version contains a mutex which it locks for allocations and frees.
class FFTL_NODISCARD FixedBlockMemPool_ThreadSafe : public FixedBlockMemPool
{
public:
	typedef FixedBlockMemPool_ThreadSafe _Myclass;
	typedef FixedBlockMemPool _Mybase;
	using typename _Mybase::size_type;
	using _Mybase::INDEX_USED_SIGNATURE;

	//	Default constructor
	FixedBlockMemPool_ThreadSafe()  = default;

	// Detailed constructor
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount*allocSize
	FixedBlockMemPool_ThreadSafe(void* pPool, uint maxCount, uint allocSize);

	// Detailed constructor
	// maxCount:	Maximum number of allocations we can handle.
	// allocSize:	The block size of the allocation desired.
	// alignment:	The desired alignment boundary of the allocations.
	FixedBlockMemPool_ThreadSafe(uint maxCount, uint allocSize, uint alignment);

	//	Copies / moves not allowed
	FixedBlockMemPool_ThreadSafe(const FixedBlockMemPool_ThreadSafe&) = delete;
	FixedBlockMemPool_ThreadSafe(FixedBlockMemPool_ThreadSafe&&) = delete;
	FixedBlockMemPool_ThreadSafe& operator=(const FixedBlockMemPool_ThreadSafe&) = delete;
	FixedBlockMemPool_ThreadSafe& operator=(FixedBlockMemPool_ThreadSafe&&) = delete;

	// PURPOSE:		Initializes the pool if we used a default constructor.
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount*allocSize
	void Init(void* pPool, uint maxCount, uint allocSize);

	// PURPOSE:		Initializes the pool if we used a default constructor.
	// maxCount:	Maximum number of allocations we can handle.
	// allocSize:	The block size of the allocation desired.
	// alignment:	The desired alignment boundary of the allocations.
	void Init(uint maxCount, uint allocSize, uint alignment);

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	void* Allocate();

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	void Free(void* pInstance);

#if defined(FFTL_ENABLE_PROFILING)
#	if FFTL_FIXEDBLOCKPOOL_ATOMIC
	FFTL_NODISCARD u32					GetUsedCollisionCount() const	{ return m_UsedCollisionCount; }
	FFTL_NODISCARD u32					GetEntryCollisionCount() const	{ return m_EntryCollisionCount; }
#	else
	FFTL_NODISCARD static constexpr u32	GetUsedCollisionCount()			{ return 0; }
	FFTL_NODISCARD static constexpr u32	GetEntryCollisionCount()		{ return 0; }
#	endif
#endif // defined(FFTL_ENABLE_PROFILING)

private:
#if !FFTL_FIXEDBLOCKPOOL_ATOMIC
	Mutex						m_Mutex;
#elif defined(FFTL_ENABLE_PROFILING)
	u32							m_UsedCollisionCount = 0;
	u32							m_EntryCollisionCount = 0;
#endif
};




////////////////////////////////////////////////////////////////
// PersistentPool
//	Simply contains an owned array of objects that are constructed when the pool is.
//	The user will be responsible for knowing when the objects are available or not.
template <class T, uint _MaxCount>
class PersistentPool
{
public:
	using size_type = typename std::conditional<_MaxCount <= 256, u8, typename std::conditional<_MaxCount <= 65536, u16, u32>::type>::type; // Ensures size_type is the smallest possible unsigned integer type
	static constexpr size_type INDEX_USED_SIGNATURE = static_cast<size_type>(~0);

	PersistentPool()
	{
		m_NumUsedEntries = 0;

		//	Sequence the indices
		for (uint i = 0; i < _MaxCount; ++i)
		{
			m_FreeIndexStack[i] = (size_type)i;
		}
	}

	FFTL_NODISCARD static constexpr uint GetMaxCount() { return _MaxCount; }
	FFTL_NODISCARD uint GetNumUsedEntries() const { return m_NumUsedEntries; }

	// RETURN : the pointer to the next free entry, or nullptr if all are in use
	FFTL_NODISCARD T* GetFreeInstance()
	{
		if (m_NumUsedEntries != _MaxCount)
		{
//			FFTL_ASSERT(m_FreeIndexStack[m_NumUsedEntries] != INDEX_USED_SIGNATURE);
			T* pType = &m_Pool[ m_FreeIndexStack[m_NumUsedEntries] ];
//			m_FreeIndexStack[m_NumUsedEntries] = INDEX_USED_SIGNATURE; // Mark this one as used for debug purposes
			++m_NumUsedEntries;
			return pType;
		}
		return nullptr;
	}

	// PURPOSE: When you're finished using one particular instance of an object, free it up here
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	void ReturnInstance(T* pInstance)
	{
		FFTL_ASSERT(m_NumUsedEntries != 0);
		if (m_NumUsedEntries != 0)
		{
			--m_NumUsedEntries;
//			FFTL_ASSERT(m_FreeIndexStack[m_NumUsedEntries] == INDEX_USED_SIGNATURE);
			m_FreeIndexStack[m_NumUsedEntries] = size_type(pInstance - &m_Pool[0]);
		}
	}

	// PURPOSE: Useful when you want to take control of the contained objects directly
	FFTL_NODISCARD T& operator[](uint uIndex)
	{
		FFTL_ASSERT(uIndex < m_NumUsedEntries);
		return m_Pool[ m_FreeIndexStack[uIndex] ];
	}
	FFTL_NODISCARD const T& operator[](uint uIndex) const
	{
		FFTL_ASSERT(uIndex < m_NumUsedEntries);
		return m_Pool[m_FreeIndexStack[uIndex]];
	}

private:
	T			m_Pool[_MaxCount];
	size_type	m_FreeIndexStack[_MaxCount];
	size_type	m_NumUsedEntries;
};



} // namespace FFTL

#include "MemPoolFixedBlock.inl"
