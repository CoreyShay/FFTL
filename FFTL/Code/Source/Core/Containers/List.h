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

// Intrusive linked lists by containment
namespace FFTL
{


//	Contained List sample usage :
/*

class Object
{
	friend class ObjectManager;
private:
	int Stuff1;
	float Stuff2;
	FFTL_Define_ListNode(Object, m_ListNode);
};

class ObjectManager
{
public:
	FFTL_Define_List(Object, m_ListNode, m_ObjectList);
};

*/


#define FFTL_Define_ListNode(T, __nodeName__)									\
struct __nodeName__ ## __memberNameTag__										\
{																				\
	static constexpr ptrdiff_t MyOffset() { return offsetof(T, __nodeName__); }	\
};																				\
FFTL::ListNode<T, __nodeName__ ## __memberNameTag__> __nodeName__

#define FFTL_Define_List(T, ElementTypeNodeMember, __listName__) FFTL::TContainedLinkedList<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember> __listName__
#define FFTL_Define_List_Array(T, ElementTypeNodeMember, __listName__, N) FFTL::FixedArray< FFTL::TContainedLinkedList<T, T::ElementTypeNodeMember ## __memberNameTag__, &T::ElementTypeNodeMember>, N > __listName__


//	You create these, and manually add them to the ListDouble class
template <typename T, typename Tag>
class ListNode
{
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

	T* GetElement() { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<T*>(reinterpret_cast<char*>(this) - Tag::MyOffset()); }
	const T* GetElement() const { FFTL_ASSERT(m_pNextNode && m_pPrevNode); return reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) - Tag::MyOffset()); }

	static ListNode<T, Tag>* GetNodeFromElement(T* pElem) { return reinterpret_cast<ListNode<T, Tag>*>(reinterpret_cast<char*>(pElem) + Tag::MyOffset()); }
	static const ListNode<T, Tag>* GetNodeFromElement(const T* pElem) { return reinterpret_cast<const ListNode<T, Tag>*>(reinterpret_cast<const char*>(pElem) + Tag::MyOffset()); }

	const ListNode<T, Tag>* GetNextNode() const { return m_pNextNode; }
	ListNode<T, Tag>* GetNextNode() { return m_pNextNode; }
	const ListNode<T, Tag>* GetPrevNode() const { return m_pPrevNode; }
	ListNode<T, Tag>* GetPrevNode() { return m_pPrevNode; }

	bool IsBackEnd() const { return m_pNextNode == nullptr; }
	bool IsFrontEnd() const { return m_pPrevNode == nullptr; }
	bool IsLinked() const { return m_pNextNode != nullptr || m_pPrevNode != nullptr; }

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






template <typename T, typename Tag, ListNode<T, Tag> T::* PtrToMember>
class TContainedLinkedListIterator
{
public:
	explicit TContainedLinkedListIterator(ListNode<T, Tag>* pFirstLink)
		: m_pCurrentLink(pFirstLink)
	{ }
	explicit TContainedLinkedListIterator(T* FirstLink)
		: m_pCurrentLink(FirstLink->PtrToMember)
	{ }

	/**
	* Advances the iterator to the next element.
	*/
	FFTL_FORCEINLINE void Next()
	{
		FFTL_ASSERT(m_pCurrentLink);
		this->m_pCurrentLink = this->m_pCurrentLink->GetNextNode();
	}

	/**
	* Advances the iterator to the previous element.
	*/
	FFTL_FORCEINLINE void Prev()
	{
		FFTL_ASSERT(m_pCurrentLink);
		this->m_pCurrentLink = this->m_pCurrentLink->GetPrevNode();
	}

	FFTL_FORCEINLINE TContainedLinkedListIterator& operator++()
	{
		this->Next();
		return *this;
	}

	FFTL_FORCEINLINE TContainedLinkedListIterator operator++(int)
	{
		auto Tmp = *this;
		this->Next();
		return Tmp;
	}

	FFTL_FORCEINLINE TContainedLinkedListIterator& operator--()
	{
		this->Prev();
		return *this;
	}

	FFTL_FORCEINLINE TContainedLinkedListIterator operator--(int)
	{
		auto Tmp = *this;
		this->Prev();
		return Tmp;
	}

	FFTL_FORCEINLINE T* Get()
	{
		FFTL_ASSERT(this->m_pCurrentLink);
		return this->m_pCurrentLink->GetElement();
	}

	FFTL_FORCEINLINE T& operator->()
	{
		return *Get();
	}

	FFTL_FORCEINLINE T& operator*()
	{
		return *Get();
	}

	FFTL_FORCEINLINE bool IsBackEnd() const
	{
		return this->m_pCurrentLink->IsBackEnd();
	}

	FFTL_FORCEINLINE bool IsFrontEnd() const
	{
		return this->m_pCurrentLink->IsFrontEnd();
	}

private:

	ListNode<T, Tag>* m_pCurrentLink;

	FFTL_FORCEINLINE friend bool operator==(const TContainedLinkedListIterator& lhs, const TContainedLinkedListIterator& rhs) { return lhs.m_pCurrentLink == rhs.m_pCurrentLink; }
	FFTL_FORCEINLINE friend bool operator!=(const TContainedLinkedListIterator& lhs, const TContainedLinkedListIterator& rhs) { return lhs.m_pCurrentLink != rhs.m_pCurrentLink; }
};


/**
* This is a very simple doubly linked list class that links nodes together created externally.
* It does not own the nodes, and will not delete them. You need to do that yourself.
*/
template <typename T, typename Tag, ListNode<T, Tag> T::* PtrToMember>
class TContainedLinkedList
{
public:
	TContainedLinkedList()
		: m_HeadSentinel(&m_TailSentinel, nullptr)
		, m_TailSentinel(nullptr, &m_HeadSentinel)
	{
	}

	~TContainedLinkedList()
	{
		for (auto* pCur = m_HeadSentinel.GetNextNode(); pCur;)
		{
			auto pNodeCur = pCur;
			pCur = pCur->GetNextNode();
			pNodeCur->RemoveFromList();
		}
	}

	void AddHead(T* in_pElement)
	{
		FFTL_ASSERT((in_pElement->*PtrToMember).GetNextNode()==nullptr);
		FFTL_ASSERT((in_pElement->*PtrToMember).GetPrevNode()==nullptr);
		FFTL_ASSERT(&(in_pElement->*PtrToMember) != &m_HeadSentinel && &(in_pElement->*PtrToMember) != &m_TailSentinel);

		m_HeadSentinel.AddAfter(&(in_pElement->*PtrToMember));
	}

	void AddTail(T* in_pElement)
	{
		FFTL_ASSERT((in_pElement->*PtrToMember).GetNextNode()==nullptr);
		FFTL_ASSERT((in_pElement->*PtrToMember).GetPrevNode()==nullptr);
		FFTL_ASSERT(&(in_pElement->*PtrToMember) != &m_HeadSentinel && &(in_pElement->*PtrToMember) != &m_TailSentinel);

		m_TailSentinel.AddBefore(&(in_pElement->*PtrToMember));
	}

	static void RemoveElement(T* in_pElement)
	{
		(in_pElement->*PtrToMember).RemoveFromList();
	}

	static void RemoveNode(ListNode<T, Tag>* in_pNode)
	{
		in_pNode->RemoveFromList();
	}

	T* RemoveHead()
	{
		auto pNext = m_HeadSentinel.GetNextNode();
		if (pNext)
		{
			pNext->RemoveFromList();
			return pNext->GetElement();
		}
		return nullptr;
	}

	T* RemoveTail()
	{
		auto pPrev = m_TailSentinel.GetPrevNode();
		if (pPrev)
		{
			pPrev->RemoveFromList();
			return pPrev->GetElement();
		}
		return nullptr;
	}

	static TContainedLinkedListIterator<T, Tag, PtrToMember> CreateIterator(ListNode<T, Tag>* pStartNode)
	{
		return TContainedLinkedListIterator<T, Tag, PtrToMember>(pStartNode);
	}
	static TContainedLinkedListIterator<T, Tag, PtrToMember> CreateIterator(T* pStartNode)
	{
		return TContainedLinkedListIterator<T, Tag, PtrToMember>(pStartNode);
	}
	TContainedLinkedListIterator<T, Tag, PtrToMember> CreateIteratorHead()
	{
		return CreateIterator(m_HeadSentinel.GetNextNode());
	}
	TContainedLinkedListIterator<T, Tag, PtrToMember> CreateIteratorTail()
	{
		return CreateIterator(m_TailSentinel.GetPrevNode());
	}
	TContainedLinkedListIterator<T, Tag, PtrToMember> CreateIteratorEnd()
	{
		return CreateIterator(&m_TailSentinel);
	}

	const ListNode<T, Tag>* GetHead() const { return this->m_HeadSentinel.GetNextNode(); }
	ListNode<T, Tag>* GetHead() { return this->m_HeadSentinel.GetNextNode(); }
	const ListNode<T, Tag>* GetTail() const { return this->m_TailSentinel.GetPrevNode(); }
	ListNode<T, Tag>* GetTail() { return this->m_TailSentinel.GetPrevNode(); }
	bool IsEmpty() const { return this->m_HeadSentinel.GetNextNode() == &this->m_TailSentinel; }

	TContainedLinkedListIterator<T, Tag, PtrToMember> begin() { return CreateIteratorHead(); }
	TContainedLinkedListIterator<T, Tag, PtrToMember> end() { return CreateIteratorEnd(); }

protected:
	ListNode<T, Tag> m_HeadSentinel;
	ListNode<T, Tag> m_TailSentinel;
};


}


