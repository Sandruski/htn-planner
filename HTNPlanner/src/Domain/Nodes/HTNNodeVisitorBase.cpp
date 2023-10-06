#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNAtom.h"
#include "HTNMacros.h"

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNDomainNode& inDomainNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantsNode& inConstantsNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantNode& inConstantNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAxiomNode& inAxiomNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNMethodNode& inMethodNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNBranchNode& inBranchNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConditionNode& inConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAxiomConditionNode& inAxiomConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAndConditionNode& inAndConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNOrConditionNode& inOrConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAltConditionNode& inAltConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNNotConditionNode& inNotConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNCompoundTaskNode& inCompoundTaskNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNValueNode& inValueNode)
{
    return HTNAtom();
}

void HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNVariableValueNode& inVariableValueNode, MAYBE_UNUSED const HTNAtom& inVariableValueNodeValue)
{
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNVariableValueNode& inVariableValueNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantValueNode& inConstantValueNode)
{
    return HTNAtom();
}

void HTNNodeVisitorBase::SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue)
{
    inNode.Accept(*this, inNodeValue);
}

HTNAtom HTNNodeVisitorBase::GetNodeValue(const HTNNodeBase& inNode)
{
    return inNode.Accept(*this);
}
