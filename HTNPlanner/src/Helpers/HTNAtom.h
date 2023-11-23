// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Helpers/HTNAtomList.h"
#include "HTNPlannerMinimal.h"

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
    const HTNAtom& GetListElement(const uint32 inIndex) const;
    const HTNAtom* FindListElement(const uint32 inIndex) const;

    int32  GetListNumItems() const;
    bool IsListEmpty() const;

    // Unbinds this HtnAtom, this can be used in the context of multiresult queries where we might want to reuse the same HtnAtom
    // multiple times because the backtracking mechanism (we will talk about this later) is making us reevaluate the planner.
    void UnBind();

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
