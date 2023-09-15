#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNTaskNode;
class HTNValueNode;

/**
 * Node representing a branch
 */
class HTNBranchNode final : public HTNNodeBase
{
public:
    explicit HTNBranchNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNConditionNodeBase>& inPreConditionNode,
                           const std::vector<std::shared_ptr<const HTNTaskNode>>& inTaskNodes);
    ~HTNBranchNode();

    void        Accept(const HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&             GetIDNode() const;
    const std::shared_ptr<const HTNConditionNodeBase>&     GetPreConditionNode() const;
    const std::vector<std::shared_ptr<const HTNTaskNode>>& GetTaskNodes() const;

private:
    // Node representing the ID of the branch
    // - Unique within its method
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Node representing the pre-condition of the branch
    std::shared_ptr<const HTNConditionNodeBase> mPreConditionNode;

    // Nodes representing the tasks of the branch
    std::vector<std::shared_ptr<const HTNTaskNode>> mTaskNodes;
};

inline const std::shared_ptr<const HTNValueNode>& HTNBranchNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNBranchNode::GetPreConditionNode() const
{
    return mPreConditionNode;
}

inline const std::vector<std::shared_ptr<const HTNTaskNode>>& HTNBranchNode::GetTaskNodes() const
{
    return mTaskNodes;
}
