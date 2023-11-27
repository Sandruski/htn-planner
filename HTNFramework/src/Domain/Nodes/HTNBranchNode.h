// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNIdentifierExpressionNode;
class HTNTaskNodeBase;

/**
 * Node representing a branch of a method
 */
class HTNBranchNode final : public HTNNodeBase
{
public:
    explicit HTNBranchNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                           const std::shared_ptr<const HTNConditionNodeBase>&         inPreConditionNode,
                           const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& inTaskNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the ID of the node, which is the same as the ID of the branch because it is already unique within its method
    HTN_NODISCARD std::string GetID() const final;

    // Returns the node representing the ID of the branch
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the node representing the pre-condition of the branch
    HTN_NODISCARD const std::shared_ptr<const HTNConditionNodeBase>& GetPreConditionNode() const;

    // Returns the nodes representing the tasks of the branch
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& GetTaskNodes() const;

private:
    // Node representing the ID of the branch
    // - Unique within its method
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Node representing the pre-condition of the branch
    std::shared_ptr<const HTNConditionNodeBase> mPreConditionNode;

    // Nodes representing the tasks of the branch
    std::vector<std::shared_ptr<const HTNTaskNodeBase>> mTaskNodes;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNBranchNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNBranchNode::GetPreConditionNode() const
{
    return mPreConditionNode;
}

inline const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& HTNBranchNode::GetTaskNodes() const
{
    return mTaskNodes;
}
