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
	FFTL_Define_ListNode(private, Object, m_ListNode); // scope access must be specified or else all members below will be public!
};

class ObjectManager
{
public:
	FFTL_Define_List(Object, m_ListNode, m_ObjectList);
};

*/


#define FFTL_Define_ListNode(__access__, T, __nodeName__)						\
public:																			\
struct __nodeName__ ## __memberNameTag__										\
{																				\
	static constexpr ptrdiff_t MyOffset() { return offsetof(T, __nodeName__); }	\
};																				\
__access__:																		\
FFTL::ListNode<T, __nodeName__ ## __memberNameTag__> __nodeName__

#define FFTL_Define_List(T, ElementTypeNodeMember, __listName__) FFTL::List<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember> __listName__
#define FFTL_Define_List_Array(T, ElementTypeNodeMember, __listName__, N) FFTL::FixedArray< FFTL::List<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember>, N> __listName__


template <typename T, typename Tag> class ListNode;
template <typename T, typename Tag, ListNode<T, Tag> T::* PtrToMember> class List;
template <typename T, typename Tag> class ListIterator;
template <typename T, typename Tag> class ListIteratorFor;
template <typename T, typename Tag> class ListIteratorRev;


template <typename T, typename Tag>
class FFTL_NODISCARD ListNode
{
	template <typename _T, typename _Tag, ListNode<_T, _Tag> _T::* PtrToMember> friend class List;
	friend class ListIterator<T, Tag>;
	friend class ListIteratorFor<T, Tag>;
	friend class ListIteratorRev<T, Tag>;

public:
	ListNode()
		: m_pNextNode(nullptr)
		, m_pPrevNode(nullptr)
	{
	}

	~ListNode()
	{
		if (m_pNextNode)
			m_pNextNode->m_pPrevNode = m_pPrevNode;
		if (m_pPrevNode)
			m_pPrevNode->m_pNextNode = m_pNextNode;
	}

	FFTL_NODISCARD T* get_element() { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<T*>(reinterpret_cast<char*>(this) - Tag::MyOffset()); }
	FFTL_NODISCARD const T* get_element() const { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) - Tag::MyOffset()); }

	FFTL_NODISCARD static ListNode<T, Tag>* get_node(T* pElem) { return reinterpret_cast<ListNode<T, Tag>*>(reinterpret_cast<char*>(pElem) + Tag::MyOffset()); }
	FFTL_NODISCARD static const ListNode<T, Tag>* get_node(const T* pElem) { return reinterpret_cast<const ListNode<T, Tag>*>(reinterpret_cast<const char*>(pElem) + Tag::MyOffset()); }

	FFTL_NODISCARD const ListNode<T, Tag>* GetNextNode() const { return m_pNextNode; }
	FFTL_NODISCARD ListNode<T, Tag>* GetNextNode() { return m_pNextNode; }
	FFTL_NODISCARD const ListNode<T, Tag>* GetPrevNode() const { return m_pPrevNode; }
	FFTL_NODISCARD ListNode<T, Tag>* GetPrevNode() { return m_pPrevNode; }

	FFTL_NODISCARD bool is_back() const { return m_pNextNode == nullptr; }
	FFTL_NODISCARD bool is_front() const { return m_pPrevNode == nullptr; }
	FFTL_NODISCARD bool is_linked() const { return m_pNextNode != nullptr || m_pPrevNode != nullptr; }

	void RemoveFromList()
	{
		if ( m_pNextNode )
		{
			m_pNextNode->m_pPrevNode = m_pPrevNode;
			FFTL_ASSERT(m_pPrevNode);
			m_pPrevNode->m_pNextNode = m_pNextNode;
		}
		m_pNextNode = nullptr;
		m_pPrevNode = nullptr;
	}

	void AddBefore(ListNode<T, Tag>* in_pNode)
	{
		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr && in_pNode->m_pPrevNode == nullptr);
		in_pNode->m_pNextNode = this;
		in_pNode->m_pPrevNode = m_pPrevNode;
		m_pPrevNode->m_pNextNode = in_pNode;
		m_pPrevNode = in_pNode;
	}

	void AddAfter(ListNode<T, Tag>* in_pNode)
	{
		FFTL_ASSERT(in_pNode->m_pNextNode == nullptr && in_pNode->m_pPrevNode == nullptr);
		in_pNode->m_pNextNode = m_pNextNode;
		in_pNode->m_pPrevNode = this;
		m_pNextNode->m_pPrevNode = in_pNode;
		m_pNextNode = in_pNode;
	}

	void AddBefore(T* in_pElement)
	{
		AddBefore( get_node(in_pElement) );
	}

	void AddAfter(T* in_pElement)
	{
		AddAfter( get_node(in_pElement) );
	}

private:
	ListNode(ListNode<T, Tag>* in_NextNode, ListNode<T, Tag>* in_PrevNode)
		: m_pNextNode(in_NextNode)
		, m_pPrevNode(in_PrevNode)
	{
	}

	ListNode<T, Tag>* m_pNextNode;
	ListNode<T, Tag>* m_pPrevNode;
};


template <typename T, typename Tag>
class FFTL_NODISCARD ListIterator
{
public:
	explicit ListIterator(ListNode<T, Tag>* pFirstNode)
		: m_pCurrentNode(pFirstNode)
	{
		FFTL_ASSERT(pFirstNode);
	}
	explicit ListIterator(T* pFirstNode)
		: m_pCurrentNode(ListNode<T, Tag>::get_node(pFirstNode))
	{
	}

	ListIterator(const ListIterator&) = delete;
	ListIterator& operator=(const ListIterator&) = delete;

	FFTL_NODISCARD FFTL_FORCEINLINE T* get()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		return this->m_pCurrentNode->get_element();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T* operator->()
	{
		return get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T& operator*()
	{
		return *get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool is_back() const
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		return this->m_pCurrentNode->is_back();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool is_front() const
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		return this->m_pCurrentNode->is_front();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const ListIterator& lhs, const ListIterator& rhs) { return lhs.m_pCurrentNode == rhs.m_pCurrentNode; }
	FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const ListIterator& lhs, const ListIterator& rhs) { return lhs.m_pCurrentNode != rhs.m_pCurrentNode; }
	FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator==(const ListIterator& lhs, const ListNode<T, Tag>* rhs) { return lhs.m_pCurrentNode == rhs; }
	FFTL_NODISCARD FFTL_FORCEINLINE friend bool operator!=(const ListIterator& lhs, const ListNode<T, Tag>* rhs) { return lhs.m_pCurrentNode != rhs; }

protected:

	~ListIterator() = default;

	ListNode<T, Tag>* m_pCurrentNode;
};

template <typename T, typename Tag>
class FFTL_NODISCARD ListIteratorFor : public ListIterator<T, Tag>
{
public:
	using NodeType = ListNode<T, Tag>;

	explicit ListIteratorFor(NodeType* pFirstNode)
		: ListIterator<T, Tag>(pFirstNode)
	{
	}
	explicit ListIteratorFor(T* pFirstNode)
		: ListIteratorFor<T, Tag>(NodeType::get_node(pFirstNode))
	{
	}
	ListIteratorFor(ListIteratorFor&& rhs)
		: ListIterator<T, Tag>(std::exchange(rhs.m_pCurrentNode, nullptr))
	{
	}

	ListIteratorFor(const ListIteratorFor&) = delete;
	ListIteratorFor& operator=(const ListIteratorFor&) = delete;
	ListIteratorFor& operator=(ListIteratorFor&&) = delete;

	/**
	* Advances the iterator to the next element.
	*/
	void advance()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		this->m_pCurrentNode = this->m_pCurrentNode->GetNextNode();
	}

	ListIteratorFor& operator++()
	{
		this->advance();
		return *this;
	}

	void remove_and_advance()
	{
		ListNode<T, Tag>* pCurrentNode = this->m_pCurrentNode;
		advance();
		pCurrentNode->RemoveFromList();
	}

	//	Don't overload these
	ListIteratorFor operator++(int) = delete;
	ListIteratorFor operator--(int) = delete;
	ListIteratorFor& operator--() = delete;
};

template <typename T, typename Tag>
class FFTL_NODISCARD ListIteratorRev : public ListIterator<T, Tag>
{
public:
	using NodeType = ListNode<T, Tag>;

	explicit ListIteratorRev(NodeType* pFirstNode)
		: ListIterator<T, Tag>(pFirstNode)
	{
	}
	explicit ListIteratorRev(T* pFirstNode)
		: ListIteratorFor<T, Tag>(NodeType::get_node(pFirstNode))
	{
	}
	ListIteratorRev(ListIteratorRev&& rhs)
		: ListIterator<T, Tag>(std::exchange(rhs.m_pCurrentNode, nullptr))
	{
	}

	ListIteratorRev(const ListIteratorRev&) = delete;
	ListIteratorRev& operator=(const ListIteratorRev&) = delete;
	ListIteratorRev& operator=(ListIteratorRev&&) = delete;

	/**
	* Advances the iterator to the next element.
	*/
	void advance()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		this->m_pCurrentNode = this->m_pCurrentNode->GetPrevNode();
	}

	ListIteratorRev& operator++()
	{
		this->advance();
		return *this;
	}

	void remove_and_advance()
	{
		ListNode<T, Tag>* pCurrentNode = this->m_pCurrentNode;
		advance();
		pCurrentNode->RemoveFromList();
	}

	//	Don't overload these
	ListIteratorRev operator++(int) = delete;
	ListIteratorRev operator--(int) = delete;
	ListIteratorRev& operator--() = delete;
};


//	PtrToMember template param needed only for natvis
template <typename T, typename Tag, ListNode<T, Tag> T::* PtrToMember>
class FFTL_NODISCARD List
{
public:
	List()
		: m_HeadSentinel(&m_TailSentinel, nullptr)
		, m_TailSentinel(nullptr, &m_HeadSentinel)
	{
	}

	~List()
	{
		for (auto* pCur = m_HeadSentinel.GetNextNode(); !pCur->is_back();)
		{
			auto pNodeCur = pCur;
			pCur = pCur->GetNextNode();
			pNodeCur->RemoveFromList();
		}
	}

	void push_front(T* in_pElement)
	{
		ListNode<T, Tag>* in_pNode = ListNode<T, Tag>::get_node(in_pElement);

		FFTL_ASSERT(in_pNode->GetNextNode() == nullptr);
		FFTL_ASSERT(in_pNode->GetPrevNode() == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_HeadSentinel.AddAfter(in_pNode);
	}

	void push_back(T* in_pElement)
	{
		ListNode<T, Tag>* in_pNode = ListNode<T, Tag>::get_node(in_pElement);

		FFTL_ASSERT(in_pNode->GetNextNode() == nullptr);
		FFTL_ASSERT(in_pNode->GetPrevNode() == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_TailSentinel.AddBefore(in_pNode);
	}

	FFTL_NODISCARD const ListNode<T, Tag>* head() const { return this->m_HeadSentinel.GetNextNode(); }
	FFTL_NODISCARD ListNode<T, Tag>* head() { return this->m_HeadSentinel.GetNextNode(); }
	FFTL_NODISCARD const ListNode<T, Tag>* tail() const { return this->m_TailSentinel.GetPrevNode(); }
	FFTL_NODISCARD ListNode<T, Tag>* tail() { return this->m_TailSentinel.GetPrevNode(); }
	FFTL_NODISCARD bool empty() const { return this->m_HeadSentinel.GetNextNode() == &this->m_TailSentinel; }

	FFTL_NODISCARD ListIteratorFor<T, Tag> begin() { return CreateIteratorHead(); }
	FFTL_NODISCARD ListIteratorRev<T, Tag> rbegin() { return CreateIteratorTail(); }
	FFTL_NODISCARD ListNode<T, Tag>* end() { return &m_TailSentinel; }
	FFTL_NODISCARD ListNode<T, Tag>* rend() { return &m_HeadSentinel; }

	static void erase(T* in_pElement) { ListNode<T, Tag>::get_node(in_pElement)->RemoveFromList(); }
	static void erase(ListNode<T, Tag>* in_pNode) { in_pNode->RemoveFromList(); }

private:
	FFTL_NODISCARD static ListIteratorFor<T, Tag> CreateIteratorFor(ListNode<T, Tag>* pStartNode)
	{
		return ListIteratorFor<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListIteratorFor<T, Tag> CreateIteratorFor(T* pStartNode)
	{
		return ListIteratorFor<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListIteratorRev<T, Tag> CreateIteratorRev(ListNode<T, Tag>* pStartNode)
	{
		return ListIteratorRev<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD static ListIteratorRev<T, Tag> CreateIteratorRev(T* pStartNode)
	{
		return ListIteratorRev<T, Tag>(pStartNode);
	}
	FFTL_NODISCARD ListIteratorFor<T, Tag> CreateIteratorHead()
	{
		return CreateIteratorFor(m_HeadSentinel.GetNextNode());
	}
	FFTL_NODISCARD ListIteratorRev<T, Tag> CreateIteratorTail()
	{
		return CreateIteratorRev(m_TailSentinel.GetPrevNode());
	}

	ListNode<T, Tag> m_HeadSentinel;
	ListNode<T, Tag> m_TailSentinel;
};


}


