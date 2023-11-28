// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtomList.h"
#include "HTNCoreMinimal.h"

#include <optional>
#include <string>
#include <variant>

/**
 * Holds a value of one of several types (bool, int32, float, std::string, or a list of atoms)
 */
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

    // Returns the value of the atom
    template<typename T>
    HTN_NODISCARD const T& GetValue() const;

    // Returns the value of the atom
    template<typename T>
    HTN_NODISCARD T& GetValueMutable();

    // Returns the type of the value of the atom
    template<typename T>
    HTN_NODISCARD bool IsType() const;

    // Resets the value of the atom
    void Unbind();

    // Returns whether the atom has a value
    HTN_NODISCARD bool IsBound() const;

    // Pushes the given element to the back of the list
    void PushBackElementToList(const HTNAtom& inValue);

    // Return the element of the list at the given index
    HTN_NODISCARD const HTNAtom& GetListElement(const uint32 inIndex) const;

    // Return the element of the list at the given index
    HTN_NODISCARD const HTNAtom* FindListElement(const uint32 inIndex) const;

    // Returns the number of elements in the list
    HTN_NODISCARD int32 GetListSize() const;

    // Returns whether the list has no elements
    HTN_NODISCARD bool IsListEmpty() const;

    // Returns a string representation of the value, optionally delimited by double quotes
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
