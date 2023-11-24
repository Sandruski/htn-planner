// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <initializer_list>
#include <string>

class HTNAtom;
class HTNAtomNode;

/*
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

    void PushBack(const HTNAtom& inValue);

    HTN_NODISCARD const HTNAtom& Get(const uint32 inIndex) const;
    HTN_NODISCARD const HTNAtom* Find(const uint32 inIndex) const;

    HTN_NODISCARD uint32 GetSize() const;
    HTN_NODISCARD bool   IsEmpty() const;

    HTN_NODISCARD std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    HTNAtomNode* mHeadNode = nullptr;
};
