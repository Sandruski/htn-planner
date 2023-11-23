// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtomList.h"
#include "HTNCoreMinimal.h"

#include <optional>
#include <string>
#include <variant>

class HTNAtom
{
public:
    HTNAtom() = default;
    HTNAtom(const bool inValue);
    HTNAtom(const int32 inValue);
    HTNAtom(const float inValue);
    HTNAtom(const std::string& inValue);
    HTNAtom(const HTNAtomList& inValue);
    ~HTNAtom();

    bool operator==(const HTNAtom& inOther) const;

    // Adds a new element to this list type
    void AddListElement(const HTNAtom& inElement);

    // Return the list element
    const HTNAtom& GetListElement(const uint32 inIndex) const;
    const HTNAtom* FindListElement(const uint32 inIndex) const;

    int32 GetListSize() const;
    bool  IsListEmpty() const;

    template<typename T>
    const T& GetValue() const;

    template<typename T>
    T& GetValueMutable();

    template<typename T>
    bool IsType() const;

    // Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
    // multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
    void Unbind();

    bool IsBound() const;

    // Returns a string optionally delimited by double quotes
    std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    std::optional<std::variant<bool, int32, float, std::string, HTNAtomList>> mData;
};

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
inline T& HTNAtom::GetValueMutable()
{
    return std::get<T>(mData.value());
}

template<typename T>
inline bool HTNAtom::IsType() const
{
    return std::holds_alternative<T>(mData.value());
}

inline void HTNAtom::Unbind()
{
    mData.reset();
}

inline bool HTNAtom::IsBound() const
{
    return mData.has_value();
}
