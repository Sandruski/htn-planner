#include "Helpers/HTNAtom.h"

#include <iomanip>
#include <ios>
#include <sstream>

HTNAtom::HTNAtom(const bool inValue) : mData(inValue)
{
}

HTNAtom::HTNAtom(const int32 inValue) : mData(inValue)
{
}

HTNAtom::HTNAtom(const float inValue) : mData(inValue)
{
}

HTNAtom::HTNAtom(const std::string& inValue) : mData(inValue)
{
}

HTNAtom::HTNAtom(const HTNAtomList& inValue) : mData(inValue)
{
}

HTNAtom::~HTNAtom()
{
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

const HTNAtom* HTNAtom::FindListElement(const uint32 inElementIdx) const
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

int32 HTNAtom::GetListNumItems() const
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
    else if (IsType<int32>())
    {
        return std::to_string(GetValue<int32>());
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