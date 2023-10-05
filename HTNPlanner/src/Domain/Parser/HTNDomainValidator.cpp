#include "Domain/Parser/HTNDomainValidator.h"

#include "Domain/AST/HTNAxiomNode.h"
#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNConstantNode.h"
#include "Domain/AST/HTNConstantsNode.h"
#include "Domain/AST/HTNDomainNode.h"
#include "Domain/AST/HTNMethodNode.h"
#include "Domain/AST/HTNTaskNode.h"
#include "Domain/AST/HTNValueNode.h"
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNVariableScope.h"
#include "HTNLog.h"
#include "HTNMacros.h"

HTNDomainValidator::HTNDomainValidator(const std::shared_ptr<const HTNDomainNode>& inDomainNode) : mDomainNode(inDomainNode)
{
}

HTNDomainValidator::~HTNDomainValidator() = default;

bool HTNDomainValidator::Validate()
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode).GetValue<bool>();
}

HTNAtom HTNDomainValidator::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string      DomainNodeID        = inDomainNode.GetID();
    const HTNVariableScope DomainVariableScope = HTNVariableScope(mDecompositionContext, DomainNodeID);

    // Dummy top-level task node
    static const std::string&                        EntryPointName           = HTNDomainHelpers::kDefaultTopLevelMethodName;
    static constexpr bool                            IsIdentifier             = true;
    static constexpr bool                            IsTopLevel               = true;
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(EntryPointName, IsIdentifier), std::vector<std::shared_ptr<const HTNValueNodeBase>>(), IsTopLevel);
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

// YES bindings
// YES backtracking
HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAndConditionNode& inAndConditionNode)
{
    return true;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNOrConditionNode& inOrConditionNode)
{
    return true;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNAltConditionNode& inAltConditionNode)
{
    return true;
}

// NO bindings
// NO backtracking
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

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNValueNode& inValueNode)
{
    return true;
}

void HTNDomainValidator::Visit(MAYBE_UNUSED const HTNVariableValueNode& inVariableValueNode, MAYBE_UNUSED const HTNAtom& inVariableValueNodeValue)
{
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNVariableValueNode& inVariableValueNode)
{
    return true;
}

HTNAtom HTNDomainValidator::Visit(MAYBE_UNUSED const HTNConstantValueNode& inConstantValueNode)
{
    return true;
}
