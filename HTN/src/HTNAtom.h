#pragma once

#include <optional>
#include <variant>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
	HTNAtomList() = default;
	HTNAtomList(const HTNAtomList& inOther);

	~HTNAtomList();

	bool operator==(const HTNAtomList& inOther) const;

	void Add(const HTNAtom& inData);

	const HTNAtom* Find(const int inIndex) const;

	int GetSize() const
	{
		return mSize;
	}

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

	HTNAtom(const HTNAtomList& inValue)
	{
		mData = inValue;
	}

	bool operator==(const HTNAtom& inOther) const 
	{
		return (mData == inOther.mData);
	}

	template<typename T>
	T& GetValue()
	{
		return std::get<T>(mData.value());
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
	
	// Adds a new element to this list type
	void AddListElement(const HTNAtom& inElement);

	// Return the list element
	const HTNAtom* FindListElement(const int inElementIdx) const;

	int GetListNumItems() const;

	// TODO JOSE: Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
	// multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
	void UnBind();

private:
	std::optional<std::variant<int, float, const char*, HTNAtomList>> mData;
};

class HTNAtomNode
{
public:
	HTNAtomNode(const HTNAtom& inData) : mData(inData)
	{
	}

	const HTNAtom& GetData() const
	{
		return mData;
	}

	void SetNext(const HTNAtomNode* inNext)
	{
		mNext = inNext;
	}

	const HTNAtomNode* GetNext() const
	{
		return mNext;
	}

private:
	HTNAtom mData;
	const HTNAtomNode* mNext = nullptr;
};

inline HTNAtomList::HTNAtomList(const HTNAtomList& inOther)
{
	for (const HTNAtomNode* Current = inOther.mHead; Current; Current = Current->GetNext())
	{
		Add(Current->GetData());
	}
}

inline HTNAtomList::~HTNAtomList()
{
	for (const HTNAtomNode* Current = mHead; Current;)
	{
		const HTNAtomNode* Next = Current->GetNext();
		delete Current;
		Current = Next;
	}
}

inline bool HTNAtomList::operator==(const HTNAtomList& inOther) const
{
	if (mSize != inOther.mSize)
	{
		return false;
	}

	const HTNAtomNode* ThisCurrent = mHead;
	const HTNAtomNode* OtherCurrent = inOther.mHead;
	for (; ThisCurrent && OtherCurrent; ThisCurrent = ThisCurrent->GetNext(), OtherCurrent = OtherCurrent->GetNext())
	{
		if (ThisCurrent->GetData() != OtherCurrent->GetData())
		{
			return false;
		}
	}

	return true;
}

inline void HTNAtomList::Add(const HTNAtom& inData)
{
	HTNAtomNode* Node = new HTNAtomNode(inData);

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

inline const HTNAtom* HTNAtomList::Find(const int inIndex) const
{
	if (inIndex < 0 || inIndex >= mSize)
	{
		return nullptr;
	}

	const HTNAtomNode* Current = mHead;
	for (int i = 0; i < inIndex; ++i)
	{
		Current = Current->GetNext();
	}

	return &Current->GetData();
}

inline void HTNAtom::AddListElement(const HTNAtom& inElement)
{
	if (!IsSet())
	{
		mData = HTNAtomList();
	}

	if (!IsType<HTNAtomList>())
	{
		return;
	}
	
	HTNAtomList& List = GetValue<HTNAtomList>();
	List.Add(inElement);
}

inline const HTNAtom* HTNAtom::FindListElement(const int inElementIdx) const
{
	if (!IsSet())
	{
		return nullptr;
	}

	if (!IsType<HTNAtomList>())
	{
		return nullptr;
	}

	const HTNAtomList& List = GetValue<HTNAtomList>();
	return List.Find(inElementIdx);
}

inline int HTNAtom::GetListNumItems() const 
{
	if (!IsSet())
	{
		return -1;
	}

	if (!IsType<HTNAtomList>())
	{
		return -1;
	}

	const HTNAtomList& List = GetValue<HTNAtomList>();
	return List.GetSize();
}


inline void HTNAtom::UnBind()
{
	// TODO JOSE:

}