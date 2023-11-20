#pragma once

#include "HTNPlannerMinimal.h"

#include <initializer_list>
#include <string>

class HTNAtom;
class HTNAtomNode;

class HTNAtomList
{
public:
    HTNAtomList() = default;
    explicit HTNAtomList(const std::initializer_list<HTNAtom>& inElements);
    HTNAtomList(const HTNAtomList& inOther);
    ~HTNAtomList();

    bool operator==(const HTNAtomList& inOther) const;
    
    void PushBack(const HTNAtom& inData);

    const HTNAtom* Find(const uint32 inIndex) const;

    uint32 GetSize() const;
    bool   IsEmpty() const;

    std::string ToString(const bool inShouldDoubleQuoteString) const;

private:
    HTNAtomNode* mHeadNode = nullptr;
};
