#include "Parser/AST/HTNNodeVisitorBase.h"

#include "HTNAtom.h"
#include "Parser/AST/HTNNodeBase.h"

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNDomainNode& inDomainNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNConstantsNode& inConstantsNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNConstantNode& inConstantNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNAxiomNode& inAxiomNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNMethodNode& inMethodNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNBranchNode& inBranchNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNConditionNode& inConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNAxiomConditionNode& inAxiomConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNAndConditionNode& inAndConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNOrConditionNode& inOrConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNAltConditionNode& inAltConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNNotConditionNode& inNotConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNCompoundTaskNode& inCompoundTaskNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNValueNode& inValueNode)
{
    return HTNAtom();
}

void HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNVariableValueNode& inVariableValueNode,
                               [[maybe_unused]] const HTNAtom&              inVariableValueNodeValue)
{
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNVariableValueNode& inVariableValueNode)
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit([[maybe_unused]] const HTNConstantValueNode& inConstantValueNode)
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
