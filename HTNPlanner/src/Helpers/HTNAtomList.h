#pragma once

#include <initializer_list>
#include <string>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
    HTNAtomList() = default;
    explicit HTNAtomList(const std::initializer_list<HTNAtom>& inElements);
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

inline unsigned int HTNAtomList::GetSize() const
{
    return mSize;
}

inline bool HTNAtomList::IsEmpty() const
{
    return (0 == mSize);
}
