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

    bool operator==(const HTNAtom& inOther) const;

    // Adds a new element to this list type
    void AddListElement(const HTNAtom& inElement);

    // Return the list element
    HTN_NODISCARD const HTNAtom& GetListElement(const uint32 inIndex) const;
    HTN_NODISCARD const HTNAtom* FindListElement(const uint32 inIndex) const;

    HTN_NODISCARD int32 GetListSize() const;
    HTN_NODISCARD bool  IsListEmpty() const;

    template<typename T>
    HTN_NODISCARD const T& GetValue() const;

    template<typename T>
    HTN_NODISCARD T& GetValueMutable();

    template<typename T>
    HTN_NODISCARD bool IsType() const;

    // Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
    // multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
    void Unbind();

    HTN_NODISCARD bool IsBound() const;

    // Returns a string optionally delimited by double quotes
    HTN_NODISCARD std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    std::optional<std::variant<bool, int32, float, std::string, HTNAtomList>> mData;
};

inline bool HTNAtom::operator==(const HTNAtom& inOther) const
{
    return mData == inOther.mData;
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
