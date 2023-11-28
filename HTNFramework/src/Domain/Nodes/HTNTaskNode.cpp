// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNTaskNode.h"

#include "Core/HTNGuidGenerator.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mID(HTNGuidGenerator::GenerateGUID())
{
}

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mID(inID)
{
}

std::string HTNTaskNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes,
                                         const bool                                                            inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID,
                                         const bool inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes, inID), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNCompoundTaskNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNPrimitiveTaskNode::HTNPrimitiveTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                           const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes)
{
}

HTNAtom HTNPrimitiveTaskNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}
