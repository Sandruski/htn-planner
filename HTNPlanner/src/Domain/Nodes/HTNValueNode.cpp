#include "Domain/Nodes/HTNValueNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

std::string HTNValueNodeBase::ToString() const
{
    static constexpr bool ShouldDoubleQuoteString = false;
    return mValue.ToString(ShouldDoubleQuoteString);
}

HTNAtom HTNValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

void HTNVariableValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const
{
    return ioNodeVisitor.Visit(*this, inNodeValue);
}

HTNAtom HTNVariableValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNConstantValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

const std::string& HTNConstantValueNode::GetConstantNodeID() const
{
    return mValue.GetValue<std::string>();
}
