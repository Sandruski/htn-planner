// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNConditionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNConditionNodeBase::HTNConditionNodeBase() : mID(GenerateID())
{
}

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNConditionNode::HTNConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                          const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNAxiomConditionNode::HTNAxiomConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                                    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNAxiomConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNAndConditionNode::HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAndConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNOrConditionNode::HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNOrConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNAltConditionNode::HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAltConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNNotConditionNode::HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inSubConditionNode)
    : mSubConditionNode(inSubConditionNode)
{
}

HTNAtom HTNNotConditionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}
