#pragma once

#include <iomanip>
#include <ios>
#include <optional>
#include <sstream>
#include <string>
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

	const HTNAtom* Find(const unsigned int inIndex) const;

	unsigned int GetSize() const
	{
		return mSize;
	}

	std::string ToString() const;

private:
	HTNAtomNode* mHead = nullptr;
	HTNAtomNode* mTail = nullptr;
	unsigned int mSize = 0;
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

	HTNAtom(const std::string& inValue)
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
	const HTNAtom* FindListElement(const unsigned int inElementIdx) const;

	int GetListNumItems() const;

	// Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
	// multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
	void UnBind();

	std::string ToString() const;

private:
	std::optional<std::variant<int, float, std::string, HTNAtomList>> mData;
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

	++mSize;
}

inline const HTNAtom* HTNAtomList::Find(const unsigned int inIndex) const
{
	if (inIndex >= mSize)
	{
		return nullptr;
	}

	const HTNAtomNode* Current = mHead;
	for (unsigned int i = 0; i < inIndex; ++i)
	{
		Current = Current->GetNext();
	}

	return &Current->GetData();
}

inline std::string HTNAtomList::ToString() const
{
	std::string String;

	for (const HTNAtomNode* Current = mHead; Current; Current = Current->GetNext())
	{
		String += Current->GetData().ToString();
		String += "->";
	}

	// Erase last "->"
	if (!String.empty())
	{
		String.erase(String.size() - 2);
	}

	return String;
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

inline const HTNAtom* HTNAtom::FindListElement(const unsigned int inElementIdx) const
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
	mData.reset();
}

inline std::string HTNAtom::ToString() const
{
	if (IsType<int>())
	{
		return std::to_string(GetValue<int>());
	}
	else if (IsType<float>())
	{
		std::ostringstream Buffer;
		Buffer << std::fixed;
		Buffer << std::setprecision(2);
		Buffer << GetValue<float>();
		return Buffer.str();
	}
	else if (IsType<std::string>())
	{
		return GetValue<std::string>();
	}
	else if (IsType<HTNAtomList>())
	{
		return GetValue<HTNAtomList>().ToString();
	}

	return "";
}
