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

#include "../Platform/Mutex.h"
#include "../Utils/Casts.h"

//	This totally works, but surprisingly, it appears that using mutexes performs slightly better, regardless of contention.
//#define FFTL_FIXEDBLOCKPOOL_ATOMIC 1


namespace FFTL
{


//	A memory pool of fixed size known at compile time.
//	All objects in the pool will be allocated a single fixed memory size,
//	so _AllocSize must be greater than or equal to the largest inherited class.
template <class _BaseType, uint _MaxCount, uint _AllocSize = sizeof(_BaseType)>
class FixedBlockMemPoolStatic
{
public:
	typedef u16 size_type;

	// Default constructor
	FixedBlockMemPoolStatic()
		: m_NumUsedEntries(0)
#if defined(FFTL_ENABLE_PROFILING)
		, m_HighwaterMarkEntries(0)
		, m_TotalAllocs(0)
		, m_TotalFrees(0)
#endif
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
		memset(m_Pool, 0xad, _AllocSize*_MaxCount);
#endif
	}

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	template <class _InheritedType> 
	FFTL_NODISCARD _InheritedType* AllocateObject()
	{
		static_assert(_AllocSize >= static_cast<uint>(sizeof(_InheritedType)), "_AllocSize must be at least as large as the inherited type");

		size_type usedCount = m_NumUsedEntries;
		if (usedCount != _MaxCount)
		{
			FFTL_ASSERT(usedCount < _MaxCount);
			FFTL_ASSERT(m_FreeIndexStack[usedCount] < _MaxCount);
			_InheritedType* pType = static_cast<_InheritedType*>( GetEntry( m_FreeIndexStack[usedCount] ) );
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

	//	These versions don't require template arguments
	FFTL_NODISCARD _BaseType* AllocateObject() { return AllocateObject<_BaseType>(); }

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	template <class _InheritedType> void FreeObject(_InheritedType* pInstance)
	{
		FFTL_ASSERT(pInstance <= GetEntry(_MaxCount-1));
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

	// RETURNS:	True if this is a valid heap pointer owned by this heap.
	// Note that this only performs a range check to see if this is within
	// our heap, because if this pointer has already been freed, it will still
	// return true as long as it's within the heap bounds.
	FFTL_NODISCARD bool IsContainedPointer(const void* pInstance) const
	{
		return ((const _BaseType*)pInstance >= GetEntry(0)) && ((const _BaseType*)pInstance <= GetEntry(_MaxCount-1));
	}

	// RETURNS:	The index into the pool corresponding to the pointer location
	FFTL_NODISCARD size_type GetObjectIndex(const _BaseType* pInstance) const
	{
		FFTL_ASSERT(IsContainedPointer(pInstance));
		return size_type((tAllocStub*)pInstance - m_Pool);
	}

	FFTL_NODISCARD static uint GetMaxCount()			{ return _MaxCount; }
	FFTL_NODISCARD static uint GetHeapSize()			{ return _MaxCount*_AllocSize; }
	FFTL_NODISCARD static uint GetAllocSize()			{ return _AllocSize; }

	FFTL_NODISCARD bool GetIsFull() const				{ return m_NumUsedEntries == _MaxCount; }
	FFTL_NODISCARD uint GetNumUsed() const				{ return m_NumUsedEntries; }
	FFTL_NODISCARD uint GetNumAvailable() const			{ return _MaxCount - m_NumUsedEntries; }
	FFTL_NODISCARD uint GetMemoryUsed() const			{ return m_NumUsedEntries*_AllocSize; }
	FFTL_NODISCARD uint GetMemoryAvailable() const		{ return GetNumAvailable()*_AllocSize; }

#if defined(FFTL_ENABLE_PROFILING)
	FFTL_NODISCARD size_type		GetHighwaterMarkEntries() const	{ return m_HighwaterMarkEntries; }
	FFTL_NODISCARD u32				GetTotalAllocs() const			{ return m_TotalAllocs; }
	FFTL_NODISCARD u32				GetTotalFrees() const			{ return m_TotalFrees; }
#endif // !_PRODUCTION

protected:
	struct alignas(_BaseType) tAllocStub { byte m_Bytes[_AllocSize]; };

	FFTL_NODISCARD _BaseType*			GetEntry(uint i)		{ return reinterpret_cast<_BaseType*>(&m_Pool[i]); }
	FFTL_NODISCARD const _BaseType*	GetEntry(uint i) const	{ return reinterpret_cast<const _BaseType*>(&m_Pool[i]); }

	tAllocStub						m_Pool[_MaxCount];
	size_type						m_FreeIndexStack[_MaxCount];
	size_type						m_NumUsedEntries;
#if defined(FFTL_ENABLE_PROFILING)
	size_type						m_HighwaterMarkEntries;
	u32								m_TotalAllocs;
	u32								m_TotalFrees;
#endif
};


//	This version contains a mutex which it locks for allocations and frees.
template <class _BaseType, uint _MaxCount, uint _AllocSize=sizeof(_BaseType)>
class FixedBlockMemPoolStatic_ThreadSafe : public FixedBlockMemPoolStatic<_BaseType, _MaxCount, _AllocSize>
{
	typedef FixedBlockMemPoolStatic_ThreadSafe<_BaseType, _MaxCount, _AllocSize> _Myclass;
	typedef FixedBlockMemPoolStatic<_BaseType, _MaxCount, _AllocSize> _Mybase;
	using typename _Mybase::size_type;

public:

	FixedBlockMemPoolStatic_ThreadSafe()
		: _Mybase()
#if defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
		, m_HeadIndex(0)
		, m_TailIndex(0)
#if defined(FFTL_ENABLE_PROFILING)
		, m_CollisionCount(0)
#endif
#endif
	{
		static_assert((_MaxCount & (_MaxCount - 1)) == 0, "_MaxCount must be a power of 2 for this lock-free method to work");
	}

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	template <class _InheritedType>
	FFTL_NODISCARD _InheritedType* AllocateObject()
	{
#if !defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
		MutexScopedLock lock(&m_Mutex);
		return _Mybase::template AllocateObject<_InheritedType>();
#else
		static_assert(_AllocSize >= static_cast<uint>(sizeof(_InheritedType)), "_AllocSize must be at least as large as the inherited type");

		const size_type usedCount = AtomicIncUnless(&this->m_NumUsedEntries, _MaxCount);
		if (usedCount != _MaxCount)
		{
#if defined(FFTL_ENABLE_PROFILING)
			std::atomic_compare_exchange_strong(&this->m_HighwaterMarkEntries, usedCount, usedCount + 1);
			std::atomic_fetch_add(&this->m_TotalAllocs, 1);
#endif // !_PRODUCTION

_RESTART_ADD:
			//	Increment the head index of the queue
			const size_type stackIndex = std::atomic_fetch_add(&this->m_HeadIndex, 1) & (_MaxCount-1);
//			FFTL_ASSERT(stackIndex != (this->m_TailIndex & (_MaxCount-1)) || usedCount == 0);

			//	Mark this one as used and signal FreeObject to continue if it was waiting on this index
			volatile size_type* pOldEntryIndex = this->m_FreeIndexStack + stackIndex;

			//	If the unlikely scenario occurs where we've waited in this thread long enough to the point where other threads have spun
			// around us, we can just retry getting the stack index.
			const size_type entryIndex = std::atomic_exchange(pOldEntryIndex, 0xffff);
			if (FFTL_UNLIKELY(entryIndex == 0xffff))
			{
#if defined(FFTL_ENABLE_PROFILING)
				std::atomic_fetch_add(&this->m_CollisionCount, 1);
#endif // !_PRODUCTION
				goto _RESTART_ADD;
			}

			FFTL_ASSERT(stackIndex < _MaxCount);
			FFTL_ASSERT(entryIndex < _MaxCount);
			_InheritedType* pType = static_cast<_InheritedType*>(this->GetEntry(entryIndex));
			return pType;
		}
		return nullptr;
#endif
	}

	//	These versions don't require template arguments
	FFTL_NODISCARD _BaseType* AllocateObject() { return AllocateObject<_BaseType>(); }

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	template <class _InheritedType>
	void FreeObject(_InheritedType* pInstance)
	{
#if !defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
		MutexScopedLock lock(&m_Mutex);
		_Mybase::FreeObject(pInstance);
#else
#if defined(FFTL_ENABLE_PROFILING)
		//	0xad will be the audio clearing signature
		memset(pInstance, 0xad, _AllocSize);
#endif
		FFTL_ASSERT(static_cast<void*>(pInstance) <= static_cast<void*>(this->GetEntry(_MaxCount - 1)));
		FFTL_ASSERT(static_cast<void*>(pInstance) >= static_cast<void*>(this->GetEntry(0)));

#if defined(FFTL_ENABLE_PROFILING)
		std::atomic_fetch_add(&this->m_TotalFrees, 1);
#endif // !_PRODUCTION

_RESTART_ADD:
		//	Increment the tail index of the queue
		const size_type stackIndex = std::atomic_fetch_add(&this->m_TailIndex, 1) & (_MaxCount-1);
		const size_type blockIndex = GetObjectIndex(pInstance);
		volatile size_type* pEntryIndex = this->m_FreeIndexStack + stackIndex;

		//	If the unlikely scenario occurs where we've waited in this thread long enough to the point where other threads have spun
		// around us, we can just retry getting the stack index.
		if (FFTL_UNLIKELY(!std::atomic_compare_exchange_strong(pEntryIndex, 0xffff, blockIndex)))
		{
#if defined(FFTL_ENABLE_PROFILING)
			std::atomic_fetch_add(&this->m_CollisionCount, 1);
#endif // !_PRODUCTION
			goto _RESTART_ADD;
		}

		//	Decrement the used entries atomically
		const size_type usedCount = std::atomic_fetch_add(&this->m_NumUsedEntries, (size_type)-1);
		FFTL_ASSERT(usedCount != 0);
		(void)usedCount;
#endif
	}


#if defined(FFTL_ENABLE_PROFILING)
#if defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
	FFTL_NODISCARD u32							GetCollisionCount() const	{ return m_CollisionCount; }
#else
	FFTL_NODISCARD static u32					GetCollisionCount()			{ return 0; }
#endif
#endif // !_PRODUCTION

private:
#if !defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
	Mutex						m_Mutex;
#else
	size_type					m_HeadIndex;
	size_type					m_TailIndex;
#if defined(FFTL_ENABLE_PROFILING)
	u32							m_CollisionCount;
#endif // !_PRODUCTION
#endif // !FFTL_FIXEDBLOCKPOOL_ATOMIC
};




//	A memory pool of fixed size not known at compile time.
//	All objects in the pool will be allocated a single fixed memory size,
//	so _AllocSize must be greater than or equal to the largest inherited class.
class FixedBlockMemPool
{
public:
	typedef u16 size_type;

	// Default constructor
	FixedBlockMemPool();

	// Detailed constructor
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount*allocSize
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
	// pPool:		Pointer to a user preallocated block of memory.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// maxCount:	Maximum number of allocations we can handle.	NOTE: pPool must be large enough to hold maxCount*allocSize
	// allocSize:	The block size of the allocation desired.		NOTE: pPool must be large enough to hold maxCount*allocSize
	void Init(void* pPool, size_t maxCount, size_t allocSize);

	// PURPOSE:		Initializes the pool if we used a default constructor.
	// maxCount:	Maximum number of allocations we can handle.
	// allocSize:	The block size of the allocation desired.
	// alignment:	The desired alignment boundary of the allocations.
	void Init(uint maxCount, size_t allocSize, size_t alignment);

	// PURPOSE:		Tears down the pool and frees the contained memory if it is owned.
	void Shutdown();

	// PURPOSE:		Tears down the pool and frees the contained memory if it is owned.
	~FixedBlockMemPool();

	// PURPOSE: Gets a pointer to the next free memory space.
	// RETURN : Pointer to the newly allocated memory, or nullptr if the pool is full
	FFTL_NODISCARD void* AllocateObject();

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	void FreeObject(void* pInstance);

	// RETURNS:	True if this is a valid heap pointer owned by this heap.
	// Note that this only performs a range check to see if this is within
	// our heap, because if this pointer has already been freed, it will still
	// return true as long as it's within the heap bounds.
	bool IsContainedPointer(const void* pInstance) const
	{
		return (pInstance >= GetEntry(0)) && (pInstance <= GetEntry(safestatic_cast<uint>(_MaxCount-1)));
	}

	// RETURNS:	The index into the pool corresponding to the pointer location
	FFTL_NODISCARD size_type GetObjectIndex(const void* pInstance)
	{
		FFTL_ASSERT(IsContainedPointer(pInstance));
		return size_type( ((byte*)pInstance - m_Pool) / _AllocSize );
	}

	FFTL_NODISCARD uint		GetMaxCount()					{ return _MaxCount; }
	FFTL_NODISCARD uint		GetHeapSize()					{ return _MaxCount*_AllocSize; }
	FFTL_NODISCARD uint		GetAllocSize()					{ return _AllocSize; }

	FFTL_NODISCARD bool		GetIsFull() const				{ return m_NumUsedEntries == _MaxCount; }
	FFTL_NODISCARD uint		GetNumUsed() const				{ return m_NumUsedEntries; }
	FFTL_NODISCARD uint		GetNumAvailable() const			{ return /*safestatic_cast<uint>*/(_MaxCount - m_NumUsedEntries); }
	FFTL_NODISCARD uint		GetMemoryUsed() const			{ return m_NumUsedEntries*_AllocSize; }
	FFTL_NODISCARD uint		GetMemoryAvailable() const		{ return GetNumAvailable()*_AllocSize; }

#if defined(FFTL_ENABLE_PROFILING)
	FFTL_NODISCARD size_type	GetHighwaterMarkEntries() const	{ return m_HighwaterMarkEntries; }
	FFTL_NODISCARD u32			GetTotalAllocs() const			{ return m_TotalAllocs; }
	FFTL_NODISCARD u32			GetTotalFrees() const			{ return m_TotalFrees; }
#endif // !_PRODUCTION

protected:
	FFTL_NODISCARD void*			GetEntry(size_t i)			{ FFTL_ASSERT(i < _MaxCount); return &m_Pool[i*_AllocSize]; }
	FFTL_NODISCARD const void*		GetEntry(size_t i) const	{ FFTL_ASSERT(i < _MaxCount); return &m_Pool[i*_AllocSize]; }

	byte*			m_Pool{nullptr};
	size_type*		m_FreeIndexStack{nullptr};
	size_type		m_NumUsedEntries{0};
	size_type		_MaxCount{0};
	u32				_AllocSize{0};
#if defined(FFTL_ENABLE_PROFILING)
	size_type		m_HighwaterMarkEntries{0};
	u32				m_TotalAllocs;
	u32				m_TotalFrees;
#endif
	bool			m_bPoolOwned{false};
};


//	This version contains a mutex which it locks for allocations and frees.
class FFTL_NODISCARD FixedBlockMemPool_ThreadSafe : public FixedBlockMemPool
{
	typedef FixedBlockMemPool_ThreadSafe _Myclass;
	typedef FixedBlockMemPool _Mybase;

public:

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
	void* AllocateObject();

	// PURPOSE: Marks the allocated space at the pointer location as free.
	//			Does not call the object destructor.
	// NOTES  : Pretty much assumes that you know what you're doing, and not trying to return
	//			an instance more than once, or a pointer to an object outside the bounds of our array,
	//			although there are some assertions to warn you if you might be.
	void FreeObject(void* pInstance);

#if defined(FFTL_ENABLE_PROFILING)
#if defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
	FFTL_NODISCARD u32	GetCollisionCount() const	{ return m_CollisionCount; }
#else
	FFTL_NODISCARD static u32	GetCollisionCount()	{ return 0; }
#endif
#endif // !_PRODUCTION

private:
#if !defined(FFTL_FIXEDBLOCKPOOL_ATOMIC)
	Mutex						m_Mutex;
#else
	size_type					m_HeadIndex;
	size_type					m_TailIndex;
#if defined(FFTL_ENABLE_PROFILING)
	u32							m_CollisionCount;
#endif // !_PRODUCTION
#endif // !FFTL_FIXEDBLOCKPOOL_ATOMIC
};




////////////////////////////////////////////////////////////////
// PersistentPool
//	Simply contains an owned array of objects that are constructed when the pool is.
//	The user will be responsible for knowing when the objects are available or not.
template <class T, uint _MaxCount>
class PersistentPool
{
public:	
	typedef u16 size_type;

	PersistentPool()
	{
		m_NumUsedEntries = 0;

		//	Sequence the indices
		for (uint i = 0; i < _MaxCount; ++i)
		{
			m_FreeIndexStack[i] = (size_type)i;
		}
	}

	FFTL_NODISCARD static uint GetMaxCount() { return _MaxCount; }
	FFTL_NODISCARD uint GetNumUsedEntries() const { return m_NumUsedEntries; }

	// RETURN : the pointer to the next free entry, or nullptr if all are in use
	FFTL_NODISCARD T* GetFreeInstance()
	{
		if (m_NumUsedEntries != _MaxCount)
		{
//			FFTL_ASSERT(m_FreeIndexStack[m_NumUsedEntries] != 0xffff);
			T* pType = &m_Pool[ m_FreeIndexStack[m_NumUsedEntries] ];
//			m_FreeIndexStack[m_NumUsedEntries] = 0xffff; // Mark this one as used for debug purposes
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
//			FFTL_ASSERT(m_FreeIndexStack[m_NumUsedEntries] == 0xffff);
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

