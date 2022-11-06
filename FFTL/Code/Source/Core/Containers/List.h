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

#define FFTL_Define_List(T, ElementTypeNodeMember, __listName__) FFTL::List<T, T::ElementTypeNodeMember ## __memberNameTag__> __listName__
#define FFTL_Define_List_Array(T, ElementTypeNodeMember, __listName__, N) FFTL::FixedArray< FFTL::List<T, T::ElementTypeNodeMember ## __memberNameTag__>, N> __listName__


template <typename T, typename Tag> class List;
template <typename T, typename Tag> class ListIterator;
template <typename T, typename Tag> class ListIteratorFor;
template <typename T, typename Tag> class ListIteratorRev;


template <typename T, typename Tag>
class FFTL_NODISCARD ListNode
{
	friend class List<T, Tag>;
	friend class ListIterator<T, Tag>;
	friend class ListIteratorFor<T, Tag>;
	friend class ListIteratorRev<T, Tag>;

public:
	ListNode()
		: m_pNextNode(nullptr)
		, m_pPrevNode(nullptr)
	{
	}
	ListNode(ListNode<T, Tag>* in_NextNode, ListNode<T, Tag>* in_PrevNode)
		: m_pNextNode(in_NextNode)
		, m_pPrevNode(in_PrevNode)
	{
	}
	~ListNode()
	{
		if (m_pNextNode)
			m_pNextNode->m_pPrevNode = m_pPrevNode;
		if (m_pPrevNode)
			m_pPrevNode->m_pNextNode = m_pNextNode;
	}

	FFTL_NODISCARD T* GetElement() { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<T*>(reinterpret_cast<char*>(this) - Tag::MyOffset()); }
	FFTL_NODISCARD const T* GetElement() const { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) - Tag::MyOffset()); }

	FFTL_NODISCARD static ListNode<T, Tag>* GetNodeFromElement(T* pElem) { return reinterpret_cast<ListNode<T, Tag>*>(reinterpret_cast<char*>(pElem) + Tag::MyOffset()); }
	FFTL_NODISCARD static const ListNode<T, Tag>* GetNodeFromElement(const T* pElem) { return reinterpret_cast<const ListNode<T, Tag>*>(reinterpret_cast<const char*>(pElem) + Tag::MyOffset()); }

	FFTL_NODISCARD const ListNode<T, Tag>* GetNextNode() const { return m_pNextNode; }
	FFTL_NODISCARD ListNode<T, Tag>* GetNextNode() { return m_pNextNode; }
	FFTL_NODISCARD const ListNode<T, Tag>* GetPrevNode() const { return m_pPrevNode; }
	FFTL_NODISCARD ListNode<T, Tag>* GetPrevNode() { return m_pPrevNode; }

	FFTL_NODISCARD bool IsBackEnd() const { return m_pNextNode == nullptr; }
	FFTL_NODISCARD bool IsFrontEnd() const { return m_pPrevNode == nullptr; }
	FFTL_NODISCARD bool IsLinked() const { return m_pNextNode != nullptr || m_pPrevNode != nullptr; }

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
		AddBefore( GetNodeFromElement(in_pElement) );
	}

	void AddAfter(T* in_pElement)
	{
		AddAfter( GetNodeFromElement(in_pElement) );
	}

	void RemoveFromList()
	{
		if (m_pNextNode)
		{
			m_pNextNode->m_pPrevNode = m_pPrevNode;
			FFTL_ASSERT(m_pPrevNode);
			m_pPrevNode->m_pNextNode = m_pNextNode;
		}
		m_pNextNode = nullptr;
		m_pPrevNode = nullptr;
	}

private:
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
		: m_pCurrentNode(ListNode<T, Tag>::GetNodeFromElement(pFirstNode))
	{
	}

	ListIterator(const ListIterator&) = delete;
	ListIterator& operator=(const ListIterator&) = delete;

	FFTL_NODISCARD FFTL_FORCEINLINE T* Get()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		return this->m_pCurrentNode->GetElement();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T& operator->()
	{
		return *Get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE T& operator*()
	{
		return *Get();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool IsBackEnd() const
	{
		return this->m_pCurrentNode->IsBackEnd();
	}

	FFTL_NODISCARD FFTL_FORCEINLINE bool IsFrontEnd() const
	{
		return this->m_pCurrentNode->IsFrontEnd();
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
		: ListIteratorFor<T, Tag>(NodeType::GetNodeFromElement(pFirstNode))
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
	void Advance()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		this->m_pCurrentNode = this->m_pCurrentNode->GetNextNode();
	}

	ListIteratorFor& operator++()
	{
		this->Advance();
		return *this;
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
		: ListIteratorFor<T, Tag>(NodeType::GetNodeFromElement(pFirstNode))
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
	void Advance()
	{
		FFTL_ASSERT(this->m_pCurrentNode);
		this->m_pCurrentNode = this->m_pCurrentNode->GetPrevNode();
	}

	ListIteratorRev& operator++()
	{
		this->Advance();
		return *this;
	}

	//	Don't overload these
	ListIteratorRev operator++(int) = delete;
	ListIteratorRev operator--(int) = delete;
	ListIteratorRev& operator--() = delete;
};


template <typename T, typename Tag>
class FFTL_NODISCARD List
{
public:
	static constexpr ptrdiff_t NODE_OFFSET = Tag::MyOffset();

	List()
		: m_HeadSentinel(&m_TailSentinel, nullptr)
		, m_TailSentinel(nullptr, &m_HeadSentinel)
	{
	}

	~List()
	{
		for (auto* pCur = m_HeadSentinel.GetNextNode(); !pCur->IsBackEnd();)
		{
			auto pNodeCur = pCur;
			pCur = pCur->GetNextNode();
			pNodeCur->RemoveFromList();
		}
	}

	void AddHead(T* in_pElement)
	{
		ListNode<T, Tag>* in_pNode = ListNode<T, Tag>::GetNodeFromElement(in_pElement);

		FFTL_ASSERT(in_pNode->GetNextNode() == nullptr);
		FFTL_ASSERT(in_pNode->GetPrevNode() == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_HeadSentinel.AddAfter(in_pNode);
	}

	void AddTail(T* in_pElement)
	{
		ListNode<T, Tag>* in_pNode = ListNode<T, Tag>::GetNodeFromElement(in_pElement);

		FFTL_ASSERT(in_pNode->GetNextNode() == nullptr);
		FFTL_ASSERT(in_pNode->GetPrevNode() == nullptr);
		FFTL_ASSERT(in_pNode != &m_HeadSentinel && in_pNode != &m_TailSentinel);

		m_TailSentinel.AddBefore(in_pNode);
	}

	static void RemoveElement(T* in_pElement)
	{
		ListNode<T, Tag>::GetNodeFromElement(in_pElement)->RemoveFromList();
	}

	static void RemoveNode(ListNode<T, Tag>* in_pNode)
	{
		in_pNode->RemoveFromList();
	}

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

	FFTL_NODISCARD const ListNode<T, Tag>* GetHead() const { return this->m_HeadSentinel.GetNextNode(); }
	FFTL_NODISCARD ListNode<T, Tag>* GetHead() { return this->m_HeadSentinel.GetNextNode(); }
	FFTL_NODISCARD const ListNode<T, Tag>* GetTail() const { return this->m_TailSentinel.GetPrevNode(); }
	FFTL_NODISCARD ListNode<T, Tag>* GetTail() { return this->m_TailSentinel.GetPrevNode(); }
	FFTL_NODISCARD bool IsEmpty() const { return this->m_HeadSentinel.GetNextNode() == &this->m_TailSentinel; }

	FFTL_NODISCARD ListIteratorFor<T, Tag> begin() { return CreateIteratorHead(); }
	FFTL_NODISCARD ListIteratorRev<T, Tag> rbegin() { return CreateIteratorTail(); }
	FFTL_NODISCARD ListNode<T, Tag>* end() { return &m_TailSentinel; }
	FFTL_NODISCARD ListNode<T, Tag>* rend() { return &m_HeadSentinel; }

protected:
	ListNode<T, Tag> m_HeadSentinel;
	ListNode<T, Tag> m_TailSentinel;
};


}


