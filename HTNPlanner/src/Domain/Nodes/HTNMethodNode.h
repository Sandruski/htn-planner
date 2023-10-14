#pragma once

#include "Domain/Nodes/HTNNodeBase.h"

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
    explicit HTNMethodNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                           const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    std::string GetID() const final;

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  GetParameterNodes() const;
    const std::vector<std::shared_ptr<const HTNBranchNode>>&              GetBranchNodes() const;
    bool                                                                  IsTopLevel() const;

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

inline HTNMethodNode::HTNMethodNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                                    const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mParameterNodes(inParameterNodes), mBranchNodes(inBranchNodes), mIsTopLevel(inIsTopLevel)
{
}

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
