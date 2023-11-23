// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNMethodNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNMethodNode::HTNMethodNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                                    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                                    const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mParameterNodes(inParameterNodes), mBranchNodes(inBranchNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNMethodNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

std::string HTNMethodNode::GetID() const
{
    static constexpr bool ShoudDoubleQuoteString = false;
    return mIDNode->GetValue().ToString(ShoudDoubleQuoteString);
}