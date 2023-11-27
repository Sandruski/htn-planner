// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <initializer_list>
#include <string>

class HTNAtom;
class HTNAtomNode;

/**
 * Singly linked list of atoms
 */
class HTNAtomList
{
public:
    HTNAtomList() = default;
    explicit HTNAtomList(const std::initializer_list<HTNAtom>& inElements);
    HTNAtomList(const HTNAtomList& inOther);
    ~HTNAtomList();

    bool operator==(const HTNAtomList& inOther) const;

    // Pushes the given element to the back of the list
    void PushBack(const HTNAtom& inValue);

    // Return the element of the list at the given index
    HTN_NODISCARD const HTNAtom& Get(const uint32 inIndex) const;

    // Return the element of the list at the given index
    HTN_NODISCARD const HTNAtom* Find(const uint32 inIndex) const;

    // Returns the number of elements in the list
    HTN_NODISCARD uint32 GetSize() const;

    // Returns whether the list has no elements
    HTN_NODISCARD bool IsEmpty() const;

    // Returns a string representation of the values, optionally delimited by double quotes
    HTN_NODISCARD std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    HTNAtomNode* mHeadNode = nullptr;
};
