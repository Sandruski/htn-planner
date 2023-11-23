// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Core/HTNAtom.h"

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

void HTNAtom::AddListElement(const HTNAtom& inElement)
{
    if (!IsBound())
    {
        mData = HTNAtomList();
    }

    if (!IsType<HTNAtomList>())
    {
        return;
    }

    HTNAtomList& List = GetValueMutable<HTNAtomList>();
    List.PushBack(inElement);
}

const HTNAtom& HTNAtom::GetListElement(const uint32 inIndex) const
{
    const HTNAtomList& List = GetValue<HTNAtomList>();
    return List.Get(inIndex);
}

const HTNAtom* HTNAtom::FindListElement(const uint32 inIndex) const
{
    if (!IsBound())
    {
        return nullptr;
    }

    if (!IsType<HTNAtomList>())
    {
        return nullptr;
    }

    const HTNAtomList& List = GetValue<HTNAtomList>();
    return List.Find(inIndex);
}

int32 HTNAtom::GetListSize() const
{
    if (!IsBound())
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
    if (!IsBound())
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
    if (!IsBound())
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
