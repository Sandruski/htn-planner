// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>

class HTNIdentifierExpressionNode;
class HTNLiteralExpressionNode;

/**
 * Node representing a constant
 */
class HTNConstantNode final : public HTNNodeBase
{
public:
    explicit HTNConstantNode(const std::shared_ptr<const HTNIdentifierExpressionNode>& inIDNode,
                             const std::shared_ptr<const HTNLiteralExpressionNode>& inValueNode);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    HTN_NODISCARD std::string GetID() const final;

    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;
    HTN_NODISCARD const std::shared_ptr<const HTNLiteralExpressionNode>& GetValueNode() const;

private:
    // Node representing the ID of the constant
    // - Unique within its group of constants
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Node representing the value of the constant
    std::shared_ptr<const HTNLiteralExpressionNode> mValueNode;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNConstantNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::shared_ptr<const HTNLiteralExpressionNode>& HTNConstantNode::GetValueNode() const
{
    return mValueNode;
}
