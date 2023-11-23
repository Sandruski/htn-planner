// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNBranchNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNBranchNode::HTNBranchNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                                    const std::shared_ptr<const HTNConditionNodeBase>&         inPreConditionNode,
                                    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& inTaskNodes)
    : mIDNode(inIDNode), mPreConditionNode(inPreConditionNode), mTaskNodes(inTaskNodes)
{
}

HTNAtom HTNBranchNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

std::string HTNBranchNode::GetID() const
{
    static constexpr bool ShoudDoubleQuoteString = false;
    return mIDNode->GetValue().ToString(ShoudDoubleQuoteString);
}
