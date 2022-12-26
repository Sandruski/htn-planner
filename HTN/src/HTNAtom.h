#pragma once

#include <optional>
#include <variant>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
	HTNAtomList() = default;
	HTNAtomList(const HTNAtomList& _Other);

	~HTNAtomList();

	void Add(const HTNAtom& inData);

	const HTNAtom* Find(int inIndex) const;

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

	// TODO: I think you need to implement operator==
	
	// Adds a new element to this list type
	void AddListElement(const HTNAtom& inElement);

	// Return the list element
	const HTNAtom* FindListElement(int inElementIdx) const;

	int GetListNumItems() const;

private:
	std::optional<std::variant<int, float, const char*, HTNAtomList>> mData;
};

class HTNAtomNode
{
public:
	HTNAtomNode(const HTNAtom& inData) : mData(inData)
	{
	}

	HTNAtom& GetData()
	{
		return mData;
	}

	const HTNAtom& GetData() const
	{
		return mData;
	}

	void SetNext(HTNAtomNode* inNext)
	{
		mNext = inNext;
	}

	HTNAtomNode* GetNext() const
	{
		return mNext;
	}

private:
	HTNAtom mData;
	HTNAtomNode* mNext = nullptr;
};

inline HTNAtomList::HTNAtomList(const HTNAtomList& _Other)
{
	for (HTNAtomNode* Current = _Other.mHead; Current; Current = Current->GetNext())
	{
		Add(Current->GetData());
	}
}

inline HTNAtomList::~HTNAtomList()
{
	for (HTNAtomNode* Current = mHead; Current;)
	{
		HTNAtomNode* Next = Current->GetNext();
		delete Current;
		Current = Next;
	}
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

inline const HTNAtom* HTNAtomList::Find(int inIndex) const
{
	if (inIndex < 0 || inIndex >= mSize)
	{
		return nullptr;
	}

	HTNAtomNode* Current = mHead;
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

inline const HTNAtom* HTNAtom::FindListElement(int inElementIdx) const
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
