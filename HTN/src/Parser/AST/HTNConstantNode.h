#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>

class HTNValueNode;

/**
 * Node representing a constant
 */
class HTNConstantNode final : public HTNNodeBase
{
public:
    explicit HTNConstantNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNValueNode>& inValueNode);
    ~HTNConstantNode();

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>& GetIDNode() const;
    const std::shared_ptr<const HTNValueNode>& GetValueNode() const;

private:
    // Node representing the ID of the constant
    // - Unique within its group of constants
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Node representing the value of the constant
    std::shared_ptr<const HTNValueNode> mValueNode;
};

inline const std::shared_ptr<const HTNValueNode>& HTNConstantNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNValueNode>& HTNConstantNode::GetValueNode() const
{
    return mValueNode;
}
