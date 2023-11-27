// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNIdentifierExpressionNode;
class HTNLiteralExpressionNode;

/**
 * Node representing a constant of a group of constants
 */
class HTNConstantNode final : public HTNNodeBase
{
public:
    explicit HTNConstantNode(const std::shared_ptr<const HTNIdentifierExpressionNode>& inIDNode,
                             const std::shared_ptr<const HTNLiteralExpressionNode>&    inValueNode);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the ID of the node, which is the same as the ID of the constant because it is already unique within its group of constants
    HTN_NODISCARD std::string GetID() const final;

    // Returns the node representing the ID of the constant
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the node representing the value of the constant
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
