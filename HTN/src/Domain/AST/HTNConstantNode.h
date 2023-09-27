#pragma once

#include "Domain/AST/HTNNodeBase.h"

#include <memory>

class HTNValueNode;
class HTNValueNodeBase;

/**
 * Node representing a constant
 */
class HTNConstantNode final : public HTNNodeBase
{
public:
    explicit HTNConstantNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNValueNodeBase>& inArgumentNode);
    ~HTNConstantNode();

    HTNAtom     Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    std::string GetID() const final;

    const std::shared_ptr<const HTNValueNode>& GetIDNode() const;
    const std::shared_ptr<const HTNValueNodeBase>& GetArgumentNode() const;

private:
    // Node representing the ID of the constant
    // - Unique within its group of constants
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Node representing the argument of the constant
    std::shared_ptr<const HTNValueNodeBase> mArgumentNode;
};

inline const std::shared_ptr<const HTNValueNode>& HTNConstantNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNValueNodeBase>& HTNConstantNode::GetArgumentNode() const
{
    return mArgumentNode;
}
