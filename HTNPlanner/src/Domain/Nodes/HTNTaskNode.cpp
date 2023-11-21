// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNTaskNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mID(GenerateID())
{
}

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes,
                                        const uint32                                                    inID)
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
                                                const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes,
                                                const uint32 inID, const bool inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes, inID), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNCompoundTaskNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNPrimitiveTaskNode::HTNPrimitiveTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes)
{
}


HTNAtom HTNPrimitiveTaskNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}
