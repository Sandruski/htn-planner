// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNIdentifierExpressionNode;
class HTNVariableExpressionNode;

/**
 * Node representing an axiom of a domain
 */
class HTNAxiomNode final : public HTNNodeBase
{
public:
    explicit HTNAxiomNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                          const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                          const std::shared_ptr<const HTNConditionNodeBase>&                   inConditionNode);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the ID of the node, which is the same as the ID of the axiom because it is already unique within its domain
    HTN_NODISCARD std::string GetID() const final;

    // Returns the node representing the ID of the axiom
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the parameters of the axiom
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& GetParameterNodes() const;

    // Returns the node representing the condition of the axiom
    HTN_NODISCARD const std::shared_ptr<const HTNConditionNodeBase>& GetConditionNode() const;

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
