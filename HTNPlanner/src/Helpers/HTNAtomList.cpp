#include "Helpers/HTNAtomList.h"

#include "Helpers/HTNAtomNode.h"

HTNAtomList::HTNAtomList(const std::initializer_list<HTNAtom>& inElements)
{
    for (const HTNAtom& Element : inElements)
    {
        Add(Element);
    }
}

HTNAtomList::HTNAtomList(const HTNAtomList& inOther)
{
    for (const HTNAtomNode* Current = inOther.mHead; Current; Current = Current->GetNext())
    {
        Add(Current->GetData());
    }
}

HTNAtomList::~HTNAtomList()
{
    for (const HTNAtomNode* Current = mHead; Current;)
    {
        const HTNAtomNode* Next = Current->GetNext();
        delete Current;
        Current = Next;
    }
}

bool HTNAtomList::operator==(const HTNAtomList& inOther) const
{
    if (mSize != inOther.mSize)
    {
        return false;
    }

    const HTNAtomNode* ThisCurrent  = mHead;
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

void HTNAtomList::Add(const HTNAtom& inData)
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

const HTNAtom* HTNAtomList::Find(const uint32 inIndex) const
{
    if (inIndex >= mSize)
    {
        return nullptr;
    }

    const HTNAtomNode* Current = mHead;
    for (uint32 i = 0; i < inIndex; ++i)
    {
        Current = Current->GetNext();
    }

    return &Current->GetData();
}

std::string HTNAtomList::ToString(const bool inShouldDoubleQuoteString) const
{
    std::string String = "(";

    for (const HTNAtomNode* Current = mHead; Current; Current = Current->GetNext())
    {
        String.append(std::format("{} ", Current->GetData().ToString(inShouldDoubleQuoteString)));
    }

    // Remove last whitespace
    const size Index = String.find_last_of(" ");
    if (Index != std::string::npos)
    {
        String.erase(Index);
    }

    String.append(")");

    return String;
}
