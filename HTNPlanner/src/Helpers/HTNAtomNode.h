#pragma once

#include "HTNPlannerMinimal.h"
#include "Helpers/HTNAtom.h"

class HTNAtomNode
{
public:
    explicit HTNAtomNode(const HTNAtom& inData);

    const HTNAtom& GetData() const;

    void               SetNextNode(HTNAtomNode* inNode);
    const HTNAtomNode* GetNextNode() const;
    HTNAtomNode*       GetNextNodeMutable();
    bool               HasNextNode() const;

private:
    HTNAtom      mData;
    HTNAtomNode* mNextNode = nullptr;
};

inline const HTNAtom& HTNAtomNode::GetData() const
{
    return mData;
}

inline void HTNAtomNode::SetNextNode(HTNAtomNode* inNode)
{
    mNextNode = inNode;
}

inline const HTNAtomNode* HTNAtomNode::GetNextNode() const
{
    return mNextNode;
}

inline HTNAtomNode* HTNAtomNode::GetNextNodeMutable()
{
    return mNextNode;
}

inline bool HTNAtomNode::HasNextNode() const
{
    return (nullptr != mNextNode);
}
