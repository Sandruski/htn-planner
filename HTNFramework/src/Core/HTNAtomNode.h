// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "HTNCoreMinimal.h"

/**
 * Node of a list of atoms
 */
class HTNAtomNode
{
public:
    explicit HTNAtomNode(const HTNAtom& inValue);

    // Returns the value of the node
    HTN_NODISCARD const HTNAtom& GetData() const;

    // Sets the next node to the given one
    void SetNextNode(HTNAtomNode* inNode);

    // Returns the next node
    HTN_NODISCARD const HTNAtomNode* GetNextNode() const;

    // Returns the next node
    HTN_NODISCARD HTNAtomNode* GetNextNodeMutable();

    // Returns whether it has a next node
    HTN_NODISCARD bool HasNextNode() const;

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
