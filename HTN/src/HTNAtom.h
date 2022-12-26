#pragma once

#include <optional>
#include <variant>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
	~HTNAtomList();

	void Add(const HTNAtom& _Data);

	int GetSize() const;

private:
	HTNAtomNode* mHead = nullptr;
	HTNAtomNode* mTail = nullptr;
	int mSize = 0;
};

class HTNAtom
{
public:
	HTNAtom() = default;

	HTNAtom(const int inValue)
	{
		mData = inValue;
	}

	HTNAtom(const float inValue)
	{
		mData = inValue;
	}

	HTNAtom(const char* inValue)
	{
		mData = inValue;
	}

	template<typename T>
	const T& GetValue() const
	{
		return std::get<T>(mData.value());
	}

	template<typename T>
	bool IsType() const
	{
		return std::holds_alternative<T>(mData.value());
	}

	bool IsSet() const
	{
		return mData.has_value();
	}

	// TODO: I think you need to implement operator==
	
	// Adds a new element to this list type, and return the next element in the list.
	HTNAtom* AddElementList(HTNAtom* inElement)
	{
		// TODO:
		(void*)inElement;
		return nullptr;
	}

	int GetListNumItems() const
	{
		if (!IsSet())
		{
			return -1;
		}

		if (!IsType<HTNAtomList>())
		{
			return -1;
		}

		// TODO...
		return 0;
	}

	// Return the list element
	HTNAtom* GetListElement(int inElementIdx) const
	{
		(void*)&inElementIdx;
		return nullptr;
	}

private:
	typedef std::variant<int, float, const char*, HTNAtomList> HTNAtomType;
	std::optional<HTNAtomType> mData;
};

class HTNAtomNode
{
public:
	HTNAtomNode(const HTNAtom& _Data);

	void SetNext(HTNAtomNode* _Next);
	HTNAtomNode* GetNext() const;

private:
	HTNAtom mData;
	HTNAtomNode* mNext = nullptr;
};

inline HTNAtomList::~HTNAtomList()
{
	for (HTNAtomNode* Current = mHead; Current;)
	{
		HTNAtomNode* Next = Current->GetNext();
		delete Current;
		Current = Next;
	}

	mHead = nullptr;
	mTail = nullptr;
}

inline void HTNAtomList::Add(const HTNAtom& _Data)
{
	HTNAtomNode* Node = new HTNAtomNode(_Data);

	if (!mHead)
	{
		mHead = Node;
	}

	if (mTail)
	{
		mTail->SetNext(Node);
	}

	mTail = Node;

	mSize++;
}

inline int HTNAtomList::GetSize() const
{
	return mSize;
}

inline HTNAtomNode::HTNAtomNode(const HTNAtom& _Data) : mData(_Data)
{
}

inline void HTNAtomNode::SetNext(HTNAtomNode* _Next)
{
	mNext = _Next;
}

inline HTNAtomNode* HTNAtomNode::GetNext() const
{
	return mNext;
}
