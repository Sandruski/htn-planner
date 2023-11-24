// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNIdentifierExpressionNode;
class HTNVariableExpressionNode;

/**
 * Node representing an axiom
 */
class HTNAxiomNode final : public HTNNodeBase
{
public:
    explicit HTNAxiomNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                          const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                          const std::shared_ptr<const HTNConditionNodeBase>&                   inConditionNode);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    HTN_NODISCARD std::string GetID() const final;

    const std::shared_ptr<const HTNIdentifierExpressionNode>&            GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& GetParameterNodes() const;
    const std::shared_ptr<const HTNConditionNodeBase>&                   GetConditionNode() const;

private:
    // Node representing the ID of the axiom
    // - Unique within its domain
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the parameters of the axiom
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> mParameterNodes;

    // Node representing the condition of the axiom
    std::shared_ptr<const HTNConditionNodeBase> mConditionNode;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNAxiomNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& HTNAxiomNode::GetParameterNodes() const
{
    return mParameterNodes;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNAxiomNode::GetConditionNode() const
{
    return mConditionNode;
}
