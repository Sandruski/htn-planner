// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNConditionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNConditionNodeBase::HTNConditionNodeBase() : mID(GenerateGUID())
{
}

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

uint32 HTNConditionNodeBase::GenerateGUID() const
{
    static uint32 mGUIDGenerator = 0;
    return ++mGUIDGenerator;
}

HTNConditionNode::HTNConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNAxiomConditionNode::HTNAxiomConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNAxiomConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNAndConditionNode::HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAndConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNOrConditionNode::HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNOrConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNAltConditionNode::HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAltConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNNotConditionNode::HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inSubConditionNode)
    : mSubConditionNode(inSubConditionNode)
{
}

HTNAtom HTNNotConditionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}
