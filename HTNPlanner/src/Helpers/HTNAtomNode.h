#pragma once

#include "Helpers/HTNAtom.h"
#include "HTNPlannerMinimal.h"

class HTNAtomNode
{
public:
    explicit HTNAtomNode(const HTNAtom& inData);

    const HTNAtom& GetData() const;

    void SetNext(const HTNAtomNode* inNext);

    const HTNAtomNode* GetNext() const;

private:
    HTNAtom            mData;
    const HTNAtomNode* mNext = nullptr;
};

inline const HTNAtom& HTNAtomNode::GetData() const
{
    return mData;
}

inline void HTNAtomNode::SetNext(const HTNAtomNode* inNext)
{
    mNext = inNext;
}

inline const HTNAtomNode* HTNAtomNode::GetNext() const
{
    return mNext;
}
