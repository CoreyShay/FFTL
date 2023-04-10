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

#include "../defs.h"
#include "../Math/MathCommon.h"
#include "../Platform/Atomic.h"

// Intrusive linked lists by containment
namespace FFTL
{


//	Intrusive List sample usage :
/*

class Object
{
private:
	int Stuff1;
	float Stuff2;
	FFTL_Define_ListAtomicNode(private, Object, m_ListNode); // scope access must be specified or else all members below will be public!
};

class ObjectManager
{
public:
	FFTL_Define_ListAtomic(Object, m_ListNode, m_ObjectList);
};

*/


#define FFTL_Define_ListAtomicNode(__access__, T, __nodeName__)					\
public:																			\
struct __nodeName__ ## __memberNameTag__										\
{																				\
	static constexpr ptrdiff_t MyOffset() { return offsetof(T, __nodeName__); }	\
};																				\
__access__:																		\
FFTL::ListAtomicNode<T, __nodeName__ ## __memberNameTag__> __nodeName__

#define FFTL_Define_ListAtomic(T, ElementTypeNodeMember, __listName__) FFTL::ListAtomic<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember> __listName__
#define FFTL_Define_ListAtomic_Array(T, ElementTypeNodeMember, __listName__, N) FFTL::FixedArray< FFTL::ListAtomic<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember>, N> __listName__


template <typename T, typename Tag> class ListAtomicNode;
template <typename T, typename Tag, ListAtomicNode<T, Tag> T::* PtrToMember> class ListAtomic;
template <typename T, typename Tag> class ListAtomicIterator;
template <typename T, typename Tag> class ListAtomicIteratorFor;
template <typename T, typename Tag> class ListAtomicIteratorRev;


template <typename T, typename Tag>
class FFTL_NODISCARD alignas(2 * sizeof(size_t)) ListAtomicNode
{
	template <typename _T, typename _Tag, ListAtomicNode<_T, _Tag> _T::* PtrToMember> friend class ListAtomic;
	friend class ListAtomicIterator<T, Tag>;
	friend class ListAtomicIteratorFor<T, Tag>;
	friend class ListAtomicIteratorRev<T, Tag>;
	static constexpr size_t LOCK_BIT_MASK = 1;

public:
	using IteratorFor = ListAtomicIteratorFor<T, Tag>;
	using IteratorRev = ListAtomicIteratorRev<T, Tag>;

	ListAtomicNode()
		: m_pNextNode(nullptr)
		, m_pPrevNode(nullptr)
	{
	}

	~ListAtomicNode()
	{
		//	Don't attempt removal of the front or back end nodes or nodes that aren't linked
		if (m_pNextNode && m_pPrevNode)
			RemoveFromList();
	}

	FFTL_NODISCARD T* get_element() { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<T*>(reinterpret_cast<char*>(this) - Tag::MyOffset()); }
	FFTL_NODISCARD const T* get_element() const { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) - Tag::MyOffset()); }

	FFTL_NODISCARD static ListAtomicNode<T, Tag>* get_node(T* pElem) { return reinterpret_cast<ListAtomicNode<T, Tag>*>(reinterpret_cast<char*>(pElem) + Tag::MyOffset()); }
	FFTL_NODISCARD static const ListAtomicNode<T, Tag>* get_node(const T* pElem) { return reinterpret_cast<const ListAtomicNode<T, Tag>*>(reinterpret_cast<const char*>(pElem) + Tag::MyOffset()); }

	FFTL_NODISCARD bool is_back() const { return m_pNextNode == nullptr; }
	FFTL_NODISCARD bool is_front() const { return m_pPrevNode == nullptr; }
	FFTL_NODISCARD bool is_linked() const { return m_pNextNode != nullptr || m_pPrevNode != nullptr; }

	//	Do not call this function if iterating through the list, or it will deadlock. In that case, use the overloads below and pass in an iterator.
	void RemoveFromList()
	{
		//	Don't allow removal of the front or back end nodes or nodes that aren't linked
		FFTL_ASSERT(m_pNextNode);
		FFTL_ASSERT(m_pPrevNode);

		while (true)
		{
			LockBoth();

			ListAtomicNode<T, Tag>* pNext = RemLockFlag(m_pNextNode);
			ListAtomicNode<T, Tag>* pPrev = RemLockFlag(m_pPrevNode);
	
			if (pNext == nullptr)
			{
				FFTL_ASSERT(pPrev == nullptr);
				break;
			}

			FFTL_ASSERT(pPrev);

			ListAtomicNode<T, Tag>* pNextPrev = TryLockLink(&pNext->m_pPrevNode);
			if (pNextPrev != nullptr) FFTL_LIKELY
			{
				FFTL_ASSERT(pNextPrev == this);

				if (AtomicTryStore(&pPrev->m_pNextNode, pNext))
				{
					//	Success. Unlock the recently locked link while resetting it to skip this node.
					AtomicStore(&pNext->m_pPrevNode, pPrev);
					break;
				}

				//	Restore the locked node
				AtomicStore(&pNext->m_pPrevNode, pNextPrev);
			}

			//	Unlock everything to allow other threads to continue and try again
			UnlockBoth();
		}

		*reinterpret_cast<mask32x4*>(this) = mask32x4::Zero();
		std::atomic_thread_fence(std::memory_order_release);
	}

private:
	void RemoveFromList(ListAtomicIteratorFor<T, Tag>& iter)
	{
		FFTL_ASSERT(iter.m_pCurrentNode == this);
		FFTL_ASSERT(IsLocked(m_pNextNode));
		FFTL_ASSERT(is_linked());

		//	Don't allow removal of the front or back end nodes or nodes that aren't linked
		FFTL_ASSERT(m_pNextNode);
		FFTL_ASSERT(m_pPrevNode);

		ListAtomicNode<T, Tag>* pNext = RemLockFlag(m_pNextNode);

		while (true)
		{
			ListAtomicNode<T, Tag>* pPrev = LockLink(&m_pPrevNode);

			FFTL_ASSERT(pNext);
			FFTL_ASSERT(pPrev);

			ListAtomicNode<T, Tag>* pNextPrev = TryLockLink(&pNext->m_pPrevNode);
			if (pNextPrev != nullptr) FFTL_LIKELY
			{
				FFTL_ASSERT(pNextPrev == this);

				if (AtomicTryStore(&pPrev->m_pNextNode, pNext)) FFTL_LIKELY
				{
					//	Success. Unlock the recently locked link while resetting it to skip this node.
					AtomicStore(&pNext->m_pPrevNode, pPrev);
					break;
				}

				//	Restore the locked node
				AtomicStore(&pNext->m_pPrevNode, pNextPrev);
			}

			//	Unlock everything to allow other threads to continue and try again
			UnlockLink(&m_pPrevNode);
		}

		//	Lock the next link if we are not advancing to the sentinel
		if (!pNext->is_back())
			LockLink(&pNext->m_pNextNode);

		iter.m_pCurrentNode = pNext;

		*reinterpret_cast<mask32x4*>(this) = mask32x4::Zero();
		std::atomic_thread_fence(std::memory_order_release);
	}

	void AddBefore(ListAtomicNode<T, Tag>* in_pNode)
	{
		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr && in_pNode->m_pPrevNode == nullptr);

		in_pNode->m_pNextNode = this;

		while (true)
		{
			ListAtomicNode<T, Tag>* pPrev = LockLink(&this->m_pPrevNode);

			in_pNode->m_pPrevNode = pPrev;
			if (AtomicTryStore(&pPrev->m_pNextNode, in_pNode)) FFTL_LIKELY
				break;

			//	Unlock and try again
			AtomicStore(&this->m_pPrevNode, pPrev);
		}
		AtomicStore(&this->m_pPrevNode, in_pNode);
	}

	void AddAfter(ListAtomicNode<T, Tag>* in_pNode)
	{
		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr && in_pNode->m_pPrevNode == nullptr);

		in_pNode->m_pPrevNode = this;

		while (true)
		{
			ListAtomicNode<T, Tag>* pNext = LockLink(&m_pNextNode);

			in_pNode->m_pNextNode = pNext;
			if (AtomicTryStore(&pNext->m_pPrevNode, in_pNode)) FFTL_LIKELY
				break;

			//	Unlock and try again
			AtomicStore(&pNext->m_pPrevNode, in_pNode);
		}
		AtomicStore(&m_pNextNode, in_pNode);
	}

	void AddBefore(T* in_pElement)
	{
		AddBefore( get_node(in_pElement) );
	}

	void AddAfter(T* in_pElement)
	{
		AddAfter( get_node(in_pElement) );
	}

	FFTL_NODISCARD static bool AtomicTryStore(ListAtomicNode<T, Tag>** ppNodeL, ListAtomicNode<T, Tag>* pNodeR)
	{
		size_t* pLink = reinterpret_cast<size_t*>(ppNodeL);
		const size_t newVal = *reinterpret_cast<size_t*>(&pNodeR);
		FFTL_ASSERT((newVal & LOCK_BIT_MASK) == 0);
		size_t oldVal = AtomicLoad(pLink) & ~LOCK_BIT_MASK;
		const size_t updatedVal = AtomicCompareExchangeWeak(pLink, newVal, oldVal);
		return updatedVal == oldVal;
	}

	FFTL_NODISCARD static ListAtomicNode<T, Tag>* TryLockLink(ListAtomicNode<T, Tag>** ppNode)
	{
		size_t* pLink = reinterpret_cast<size_t*>(ppNode);
		size_t oldVal = AtomicLoad(pLink);
		const size_t newVal = oldVal | LOCK_BIT_MASK;
		oldVal = oldVal & ~LOCK_BIT_MASK;
		const size_t updatedVal = AtomicCompareExchangeWeak(pLink, newVal, oldVal);
		return reinterpret_cast<ListAtomicNode<T, Tag>*>(updatedVal == oldVal ? updatedVal : 0);
	}

	static ListAtomicNode<T, Tag>* LockLink(ListAtomicNode<T, Tag>** ppNode)
	{
		size_t* pLink = reinterpret_cast<size_t*>(ppNode);
		size_t oldVal = AtomicLoad(pLink);
		while (true)
		{
			FFTL_ASSERT(oldVal != 0); // Do not attempt to lock null nodes
			const size_t newVal = oldVal | LOCK_BIT_MASK;
			oldVal = oldVal & ~LOCK_BIT_MASK;
			const size_t updatedVal = AtomicCompareExchangeWeak(pLink, newVal, oldVal);
			if (updatedVal == oldVal) FFTL_LIKELY
				break;
			oldVal = updatedVal;
		}
		return reinterpret_cast<ListAtomicNode<T, Tag>*>(oldVal);
	}

	static ListAtomicNode<T, Tag>* UnlockLink(ListAtomicNode<T, Tag>** ppNode)
	{
		size_t* pLink = reinterpret_cast<size_t*>(ppNode);
		ListAtomicNode<T, Tag>* pLockedNode = reinterpret_cast<ListAtomicNode<T, Tag>*>(AtomicAnd(pLink, ~LOCK_BIT_MASK));
		FFTL_ASSERT(IsLocked(pLockedNode) || pLockedNode == nullptr);
		return pLockedNode;
	}

	void LockBoth()
	{
		if constexpr (sizeof(*this) == 16)
		{
			const mask32x4 mask = mask32x4::GenMaskFromInts<1, 0, 1, 0>();
			mask32x4* pThis = reinterpret_cast<mask32x4*>(this);
			mask32x4 oldVal = *pThis;
			while (true)
			{
				const mask32x4 newVal = oldVal | mask;
				oldVal = AndNot(oldVal, mask);
				if (AtomicCompareExchangeWeakRef(pThis, newVal, oldVal)) FFTL_LIKELY
					break;
			}
		}
		else if constexpr (sizeof(*this) == 8)
		{
			static_assert(sizeof(u64) == 8);
			constexpr u64 mask = (1ull << 32ull) | 1ull;
			u64* pThis = reinterpret_cast<u64*>(this);
			u64 oldVal = AtomicLoad(pThis);
			while (true)
			{
				const u64 newVal = oldVal | mask;
				oldVal = oldVal & ~mask;
				if (AtomicCompareExchangeWeakRef<u64>(pThis, newVal, oldVal)) FFTL_LIKELY
					break;
			}
		}

		static_assert(sizeof(*this) == 16 || sizeof(*this) == 8, "Unhandled pointer size case");
	}

	void UnlockBoth()
	{
		FFTL_ASSERT(reinterpret_cast<size_t>(m_pNextNode) & 1);
		FFTL_ASSERT(reinterpret_cast<size_t>(m_pPrevNode) & 1);
		if constexpr (sizeof(*this) == 16)
		{
			const mask32x4 mask = mask32x4::GenMaskFromInts<1, 0, 1, 0>();
			mask32x4* pThis = reinterpret_cast<mask32x4*>(this);
			mask32x4 oldVal = *pThis;
			const mask32x4 newVal = AndNot(oldVal, mask);
			FFTL_VERIFY(AtomicCompareExchangeStrongRef(pThis, newVal, oldVal));
			FFTL_ASSERT(CmpEq(oldVal & mask, mask).ToIntMask() == 15);
		}
		else if constexpr (sizeof(*this) == 8)
		{
			constexpr u64 mask = (1ull << 32ull) | 1ull;
			u64* pThis = reinterpret_cast<u64*>(this);
			u64 oldVal = AtomicLoad(pThis);
			const u64 newVal = oldVal & ~mask;
			FFTL_VERIFY(AtomicCompareExchangeStrongRef(pThis, newVal, oldVal));
			FFTL_ASSERT((oldVal & mask) == mask);
		}

		static_assert(sizeof(*this) == 16 || sizeof(*this) == 8, "Unhandled pointer size case");
	}

	FFTL_NODISCARD static constexpr ListAtomicNode<T, Tag>* AddLockFlag(ListAtomicNode<T, Tag>* pNode) { return reinterpret_cast<ListAtomicNode<T, Tag>*>(reinterpret_cast<size_t>(pNode) | LOCK_BIT_MASK); }
	FFTL_NODISCARD static constexpr ListAtomicNode<T, Tag>* RemLockFlag(ListAtomicNode<T, Tag>* pNode) { return reinterpret_cast<ListAtomicNode<T, Tag>*>(reinterpret_cast<size_t>(pNode) & ~LOCK_BIT_MASK); }
	FFTL_NODISCARD static constexpr bool IsLocked(ListAtomicNode<T, Tag>* pNode) { return (reinterpret_cast<size_t>(pNode) & LOCK_BIT_MASK) != 0; }

	ListAtomicNode(ListAtomicNode<T, Tag>* in_NextNode, ListAtomicNode<T, Tag>* in_PrevNode)
		: m_pNextNode(in_NextNode)
		, m_pPrevNode(in_PrevNode)
	{
	}

	ListAtomicNode<T, Tag>* m_pNextNode;
	ListAtomicNode<T, Tag>* m_pPrevNode;
};


template <typename T, typename Tag>
class FFTL_NODISCARD ListAtomicIterator
{
public:
	friend class ListAtomicNode<T, Tag>;

	explicit ListAtomicIterator(ListAtomicNode<T, Tag>* pFirstNode)
		: m_pCurrentNode(pFirstNode)
	{
		FFTL_ASSERT(pFirstNode);
	}
	explicit ListAtomicIterator(T* pFirstNode )
		: m_pCurrentNode(ListAtomicNode<T, Tag>::get_node(pFirstNode))
	{
	}

	ListAtomicIterator(const ListAtomicIterator&) = delete;
	ListAtomicIterator& operator=(const ListAtomicIterator&) = delete;

	FFTL_NODISCARD FFTL_FORCEINLINE T* get()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		return ListAtomicNode<T, Tag>::RemLockFlag(this->m_pCurrentNode)->get_element();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T& operator->()
	{
		return *get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T& operator*()
	{
		return *get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool is_back() const
	{
		return this->m_pCurrentNode->is_back();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool is_front() const
	{
		return this->m_pCurrentNode->is_front();
	}

	FFTL_FORCEINLINE friend bool operator==(const ListAtomicIterator& lhs, const ListAtomicIterator& rhs) { return lhs.m_pCurrentNode == rhs.m_pCurrentNode; }
	FFTL_FORCEINLINE friend bool operator!=(const ListAtomicIterator& lhs, const ListAtomicIterator& rhs) { return lhs.m_pCurrentNode != rhs.m_pCurrentNode; }
	FFTL_FORCEINLINE friend bool operator==(const ListAtomicIterator& lhs, const ListAtomicNode<T, Tag>* rhs) { return lhs.m_pCurrentNode == rhs; }
	FFTL_FORCEINLINE friend bool operator!=(const ListAtomicIterator& lhs, const ListAtomicNode<T, Tag>* rhs) { return lhs.m_pCurrentNode != rhs; }

protected:

	~ListAtomicIterator() = default;

	ListAtomicNode<T, Tag>* m_pCurrentNode;
};

template <typename T, typename Tag>
class FFTL_NODISCARD ListAtomicIteratorFor : public ListAtomicIterator<T, Tag>
{
public:
	using NodeType = ListAtomicNode<T, Tag>;

	explicit ListAtomicIteratorFor(NodeType* pFirstNode)
		: ListAtomicIterator<T, Tag>(pFirstNode)
	{
		if (!pFirstNode->is_back())
			NodeType::LockLink(&pFirstNode->m_pNextNode);
	}
	explicit ListAtomicIteratorFor(T* pFirstNode )
		: ListAtomicIteratorFor<T, Tag>(NodeType::get_node(pFirstNode))
	{
	}
	ListAtomicIteratorFor(ListAtomicIteratorFor&& rhs)
		: ListAtomicIterator<T, Tag>(std::exchange(rhs.m_pCurrentNode, nullptr))
	{
	}

	~ListAtomicIteratorFor()
	{
		if (this->m_pCurrentNode)
		{
			//	Release the lock on the link
			NodeType::UnlockLink(&this->m_pCurrentNode->m_pNextNode);
		}
	}

	void remove_and_advance()
	{
		this->m_pCurrentNode->RemoveFromList(*this);
	}

	ListAtomicIteratorFor(const ListAtomicIteratorFor&) = delete;
	ListAtomicIteratorFor& operator=(const ListAtomicIteratorFor&) = delete;
	ListAtomicIteratorFor& operator=(ListAtomicIteratorFor&&) = delete;

	/**
	* Advances the iterator to the next element.
	*/
	void advance()
	{
#if defined(FFTL_ENABLE_ASSERT)
		{
			NodeType* tmpCur = this->m_pCurrentNode;
			NodeType* tmpCurNext = this->m_pCurrentNode->m_pNextNode;
			NodeType* tmpCurPrev = this->m_pCurrentNode->m_pPrevNode;

			FFTL_ASSERT(tmpCur);
			FFTL_ASSERT(tmpCurNext);
			FFTL_ASSERT(tmpCurPrev);
			FFTL_ASSERT(NodeType::IsLocked(tmpCurNext));
		}
#endif

		NodeType* pNextLink = NodeType::RemLockFlag(this->m_pCurrentNode->m_pNextNode);

		FFTL_ASSERT(pNextLink != nullptr);

		//	Lock the next link if we are not advancing to the sentinel
		if (!pNextLink->is_back())
			NodeType::LockLink(&pNextLink->m_pNextNode);

		//	Release the lock on the previous link
		AtomicStore(&this->m_pCurrentNode->m_pNextNode, pNextLink);

		this->m_pCurrentNode = pNextLink;
	}

	ListAtomicIteratorFor& operator++()
	{
		this->advance();
		return *this;
	}

	//	Don't overload these
	ListAtomicIteratorFor operator++(int) = delete;
	ListAtomicIteratorFor operator--(int) = delete;
	ListAtomicIteratorFor& operator--() = delete;
};

template <typename T, typename Tag>
class FFTL_NODISCARD ListAtomicIteratorRev : public ListAtomicIterator<T, Tag>
{
public:
	using NodeType = ListAtomicNode<T, Tag>;

	explicit ListAtomicIteratorRev(NodeType* pFirstNode)
		: ListAtomicIterator<T, Tag>(pFirstNode)
	{
		if (!pFirstNode->is_front())
			NodeType::LockLink(&pFirstNode->m_pPrevNode);
	}
	explicit ListAtomicIteratorRev(T* pFirstNode )
		: ListAtomicIteratorRev<T, Tag>(NodeType::get_node(pFirstNode))
	{
	}
	ListAtomicIteratorRev( ListAtomicIteratorRev&& rhs)
		: ListAtomicIterator<T, Tag>(std::exchange(rhs.m_pCurrentNode, nullptr))
	{
	}

	~ListAtomicIteratorRev()
	{
		if (this->m_pCurrentNode)
		{
			//	Release the lock on the link
			NodeType::UnlockLink(&this->m_pCurrentNode->m_pPrevNode);
		}
	}

	ListAtomicIteratorRev(const ListAtomicIteratorRev&) = delete;
	ListAtomicIteratorRev& operator=(const ListAtomicIteratorRev&) = delete;
	ListAtomicIteratorRev& operator=(ListAtomicIteratorRev&&) = delete;

	/**
	* Advances the iterator to the next element.
	*/
	void advance()
	{
#if defined(FFTL_ENABLE_ASSERT)
		{
			NodeType* tmpCur = this->m_pCurrentNode;
			NodeType* tmpCurNext = this->m_pCurrentNode->m_pNextNode;
			NodeType* tmpCurPrev = this->m_pCurrentNode->m_pPrevNode;

			FFTL_ASSERT(tmpCur);
			FFTL_ASSERT(tmpCurNext);
			FFTL_ASSERT(tmpCurPrev);
			FFTL_ASSERT(NodeType::IsLocked(tmpCurPrev));
		}
#endif

		NodeType* pPrevLink = NodeType::RemLockFlag(this->m_pCurrentNode->m_pPrevNode);

		FFTL_ASSERT(pPrevLink != nullptr);

		//	Lock the next link if we are not advancing to the sentinel
		if (!pPrevLink->is_front())
			NodeType::LockLink(&pPrevLink->m_pPrevNode);

		//	Release the lock on the previous link
		AtomicStore(&this->m_pCurrentNode->m_pPrevNode, pPrevLink);

		this->m_pCurrentNode = pPrevLink;
	}

	ListAtomicIteratorRev& operator++()
	{
		this->advance();
		return *this;
	}

	//	Don't overload these
	ListAtomicIteratorRev operator++(int) = delete;
	ListAtomicIteratorRev operator--(int) = delete;
	ListAtomicIteratorRev& operator--() = delete;
};


//	PtrToMember template param needed only for natvis
template <typename T, typename Tag, ListAtomicNode<T, Tag> T::* PtrToMember>
class FFTL_NODISCARD ListAtomic
{
public:
	using IteratorFor = ListAtomicIteratorFor<T, Tag>;
	using IteratorRev = ListAtomicIteratorRev<T, Tag>;

	ListAtomic()
		: m_HeadSentinel(&m_TailSentinel, nullptr)
		, m_TailSentinel(nullptr, &m_HeadSentinel)
	{
	}

	~ListAtomic()
	{
		for (ListAtomicIteratorFor it = begin(); it != end();)
		{
			it.remove_and_advance();
		}
	}

	void push_front(T* in_pElement)
	{
		ListAtomicNode<T, Tag>* in_pNode = ListAtomicNode<T, Tag>::get_node(in_pElement);

		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr);
		FFTL_ASSERT(in_pNode->m_pPrevNode == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_HeadSentinel.AddAfter(in_pNode);
	}

	void push_back(T* in_pElement)
	{
		ListAtomicNode<T, Tag>* in_pNode = ListAtomicNode<T, Tag>::get_node(in_pElement);

		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr);
		FFTL_ASSERT(in_pNode->m_pPrevNode == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_TailSentinel.AddBefore(in_pNode);
	}

	FFTL_NODISCARD bool empty() const { return this->m_HeadSentinel.m_pNextNode == &this->m_TailSentinel; }

	FFTL_NODISCARD ListAtomicIteratorFor<T, Tag> begin() { return CreateIteratorHead(); }
	FFTL_NODISCARD ListAtomicIteratorRev<T, Tag> rbegin() { return CreateIteratorTail(); }
	FFTL_NODISCARD ListAtomicNode<T, Tag>* end() { return &m_TailSentinel; }
	FFTL_NODISCARD ListAtomicNode<T, Tag>* rend() { return &m_HeadSentinel; }

private:
	FFTL_NODISCARD static ListAtomicIteratorFor<T, Tag> CreateIteratorFor(ListAtomicNode<T, Tag>* pStartNode)
	{
		return ListAtomicIteratorFor<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListAtomicIteratorFor<T, Tag> CreateIteratorFor(T* pStartNode)
	{
		return ListAtomicIteratorFor<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListAtomicIteratorRev<T, Tag> CreateIteratorRev(ListAtomicNode<T, Tag>* pStartNode)
	{
		return ListAtomicIteratorRev<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListAtomicIteratorRev<T, Tag> CreateIteratorRev(T* pStartNode)
	{
		return ListAtomicIteratorRev<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD ListAtomicIteratorFor<T, Tag> CreateIteratorHead()
	{
		ListAtomicNode<T, Tag>* pHead = ListAtomicNode<T, Tag>::LockLink(&m_HeadSentinel.m_pNextNode);
		ListAtomicIteratorFor<T, Tag> it = CreateIteratorFor(pHead);
		AtomicStore(&m_HeadSentinel.m_pNextNode, pHead);
		return it;
	}
	FFTL_NODISCARD ListAtomicIteratorRev<T, Tag> CreateIteratorTail()
	{
		ListAtomicNode<T, Tag>* pTail = ListAtomicNode<T, Tag>::LockLink(&m_TailSentinel.m_pPrevNode);
		ListAtomicIteratorRev<T, Tag> it = CreateIteratorRev(pTail);
		AtomicStore(&m_TailSentinel.m_pPrevNode, pTail);
		return it;
	}

	ListAtomicNode<T, Tag> m_HeadSentinel;
	ListAtomicNode<T, Tag> m_TailSentinel;
};


}


