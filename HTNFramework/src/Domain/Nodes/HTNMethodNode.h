// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNBranchNode;
class HTNIdentifierExpressionNode;
class HTNVariableExpressionNode;

/**
 * Node representing a method
 */
class HTNMethodNode final : public HTNNodeBase
{
public:
    explicit HTNMethodNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                           const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    HTN_NODISCARD std::string GetID() const final;

    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& GetParameterNodes() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNBranchNode>>& GetBranchNodes() const;
    HTN_NODISCARD bool                                                     IsTopLevel() const;

private:
    // Node representing the ID of the method
    // - Unique within its domain
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the parameters of the method
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> mParameterNodes;

    // Nodes representing the branches of the method
    std::vector<std::shared_ptr<const HTNBranchNode>> mBranchNodes;

    // Whether the method serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNMethodNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& HTNMethodNode::GetParameterNodes() const
{
    return mParameterNodes;
}

inline const std::vector<std::shared_ptr<const HTNBranchNode>>& HTNMethodNode::GetBranchNodes() const
{
    return mBranchNodes;
}

inline bool HTNMethodNode::IsTopLevel() const
{
    return mIsTopLevel;
}
