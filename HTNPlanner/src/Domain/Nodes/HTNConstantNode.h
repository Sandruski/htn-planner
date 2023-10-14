#pragma once

#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>

class HTNValueNode;
class HTNValueNodeBase;

/**
 * Node representing a constant
 */
class HTNConstantNode final : public HTNNodeBase
{
public:
    explicit HTNConstantNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNValueNodeBase>& inValueNode);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    std::string GetID() const final;

    const std::shared_ptr<const HTNValueNode>&     GetIDNode() const;
    const std::shared_ptr<const HTNValueNodeBase>& GetValueNode() const;

private:
    // Node representing the ID of the constant
    // - Unique within its group of constants
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Node representing the value of the constant
    std::shared_ptr<const HTNValueNodeBase> mValueNode;
};

inline HTNConstantNode::HTNConstantNode(const std::shared_ptr<const HTNValueNode>&     inIDNode,
                                        const std::shared_ptr<const HTNValueNodeBase>& inValueNode)
    : mIDNode(inIDNode), mValueNode(inValueNode)
{
}

inline const std::shared_ptr<const HTNValueNode>& HTNConstantNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNValueNodeBase>& HTNConstantNode::GetValueNode() const
{
    return mValueNode;
}
