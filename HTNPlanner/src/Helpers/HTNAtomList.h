// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"

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

    void PushBack(const HTNAtom& inData);

    const HTNAtom& Get(const uint32 inIndex) const;
    const HTNAtom* Find(const uint32 inIndex) const;

    uint32 GetSize() const;
    bool   IsEmpty() const;

    std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    HTNAtomNode* mHeadNode = nullptr;
};
