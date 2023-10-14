#include "Domain/Parser/HTNDomainValidator.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionVariableScopeNodeScope.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNMacros.h"

bool HTNDomainValidator::Validate()
{
    // TODO salvarez
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    // return GetNodeValue(*DomainNode).GetValue<bool>();
    return true;
}

HTNAtom HTNDomainValidator::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string                            DomainNodeID = inDomainNode.GetID();
    const HTNDecompositionVariableScopeNodeScope DomainVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(mDecompositionContext, DomainNodeID);

    // Dummy top-level task node
    static const std::string&                        EntryPointID             = HTNDecompositionHelpers::kDefaultTopLevelMethodID;
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    return GetNodeValue(*TopLevelCompoundTaskNode).GetValue<bool>();
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNConstantNode& inConstantNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAxiomNode& inAxiomNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNMethodNode& inMethodNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNBranchNode& inBranchNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNConditionNode& inConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAxiomConditionNode& inAxiomConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAndConditionNode& inAndConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNOrConditionNode& inOrConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAltConditionNode& inAltConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNNotConditionNode& inNotConditionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNCompoundTaskNode& inCompoundTaskNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNIdentifierExpressionNode& inIdentifierExpressionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNVariableExpressionNode& inVariableExpressionNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNConstantExpressionNode& inConstantExpressionNode)
{
    return true;
}
