// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Core/HTNAtom.h"

class HTNAtomNode
{
public:
    explicit HTNAtomNode(const HTNAtom& inValue);

    HTN_NODISCARD const HTNAtom& GetData() const;

    void               SetNextNode(HTNAtomNode* inNode);
    HTN_NODISCARD const HTNAtomNode* GetNextNode() const;
    HTN_NODISCARD HTNAtomNode*       GetNextNodeMutable();
    HTN_NODISCARD bool               HasNextNode() const;

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
    return nullptr != mNextNode;
}
