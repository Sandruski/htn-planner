#pragma once

#include <optional>
#include <string>
#include <variant>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
    HTNAtomList() = default;
    HTNAtomList(const std::initializer_list<HTNAtom>& inElements);
    HTNAtomList(const HTNAtomList& inOther);
    ~HTNAtomList();

    bool operator==(const HTNAtomList& inOther) const;

    void Add(const HTNAtom& inData);

    const HTNAtom* Find(const unsigned int inIndex) const;

    unsigned int GetSize() const;
    bool         IsEmpty() const;

    std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    HTNAtomNode* mHead = nullptr;
    HTNAtomNode* mTail = nullptr;
    unsigned int mSize = 0;
};

class HTNAtom
{
public:
    HTNAtom() = default;
    HTNAtom(const bool inValue);
    HTNAtom(const int inValue);
    HTNAtom(const float inValue);
    HTNAtom(const std::string& inValue);
    HTNAtom(const HTNAtomList& inValue);

    bool operator==(const HTNAtom& inOther) const;

    template<typename T>
    const T& GetValue() const;

    template<typename T>
    T& GetValue();

    template<typename T>
    bool IsType() const;

    bool IsSet() const;

    // Adds a new element to this list type
    void AddListElement(const HTNAtom& inElement);

    // Return the list element
    const HTNAtom* FindListElement(const unsigned int inElementIdx) const;

    int  GetListNumItems() const;
    bool IsListEmpty() const;

    // Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
    // multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
    void UnBind();

    // Returns a string optionally delimited by double quotes
    std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    std::optional<std::variant<bool, int, float, std::string, HTNAtomList>> mData;
};

class HTNAtomNode
{
public:
    explicit HTNAtomNode(const HTNAtom& inData);

    const HTNAtom& GetData() const;

    void SetNext(const HTNAtomNode* inNext);

    const HTNAtomNode* GetNext() const;

private:
    HTNAtom            mData;
    const HTNAtomNode* mNext = nullptr;
};

inline unsigned int HTNAtomList::GetSize() const
{
    return mSize;
}

inline bool HTNAtomList::IsEmpty() const
{
    return mSize == 0;
}

inline HTNAtom::HTNAtom(const bool inValue) : mData(inValue)
{
}

inline HTNAtom::HTNAtom(const int inValue) : mData(inValue)
{
}

inline HTNAtom::HTNAtom(const float inValue) : mData(inValue)
{
}

inline HTNAtom::HTNAtom(const std::string& inValue) : mData(inValue)
{
}

inline HTNAtom::HTNAtom(const HTNAtomList& inValue) : mData(inValue)
{
}

inline bool HTNAtom::operator==(const HTNAtom& inOther) const
{
    return (mData == inOther.mData);
}

template<typename T>
inline const T& HTNAtom::GetValue() const
{
    return std::get<T>(mData.value());
}

template<typename T>
inline T& HTNAtom::GetValue()
{
    return std::get<T>(mData.value());
}

template<typename T>
inline bool HTNAtom::IsType() const
{
    return std::holds_alternative<T>(mData.value());
}

inline bool HTNAtom::IsSet() const
{
    return mData.has_value();
}

inline void HTNAtom::UnBind()
{
    mData.reset();
}

inline HTNAtomNode::HTNAtomNode(const HTNAtom& inData) : mData(inData)
{
}

inline const HTNAtom& HTNAtomNode::GetData() const
{
    return mData;
}

inline void HTNAtomNode::SetNext(const HTNAtomNode* inNext)
{
    mNext = inNext;
}

inline const HTNAtomNode* HTNAtomNode::GetNext() const
{
    return mNext;
}
