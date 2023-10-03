#include "HTNAtom.h"

#include <format>
#include <iomanip>
#include <ios>
#include <sstream>

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

const HTNAtom* HTNAtomList::Find(const unsigned int inIndex) const
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

std::string HTNAtomList::ToString(const bool inShouldDoubleQuoteString) const
{
    std::string String = "(";

    for (const HTNAtomNode* Current = mHead; Current; Current = Current->GetNext())
    {
        String.append(std::format("{} ", Current->GetData().ToString(inShouldDoubleQuoteString)));
    }

    // Remove last whitespace
    const std::size_t Index = String.find_last_of(" ");
    if (Index != std::string::npos)
    {
        String.erase(Index);
    }

    String.append(")");

    return String;
}

void HTNAtom::AddListElement(const HTNAtom& inElement)
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

const HTNAtom* HTNAtom::FindListElement(const unsigned int inElementIdx) const
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

int HTNAtom::GetListNumItems() const
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

bool HTNAtom::IsListEmpty() const
{
    if (!IsSet())
    {
        return true;
    }

    if (!IsType<HTNAtomList>())
    {
        return true;
    }

    const HTNAtomList& List = GetValue<HTNAtomList>();
    return List.IsEmpty();
}

std::string HTNAtom::ToString(const bool inShouldDoubleQuoteString) const
{
    if (!IsSet())
    {
        return "";
    }

    if (IsType<bool>())
    {
        return std::format("{}", GetValue<bool>());
    }
    else if (IsType<int>())
    {
        return std::to_string(GetValue<int>());
    }
    else if (IsType<float>())
    {
        std::ostringstream Buffer;
        Buffer << std::fixed;
        Buffer << std::setprecision(1);
        Buffer << GetValue<float>();
        return Buffer.str();
    }
    else if (IsType<std::string>())
    {
        return inShouldDoubleQuoteString ? std::format("\"{}\"", GetValue<std::string>()) : GetValue<std::string>();
    }
    else if (IsType<HTNAtomList>())
    {
        return GetValue<HTNAtomList>().ToString(inShouldDoubleQuoteString);
    }

    return "";
}
