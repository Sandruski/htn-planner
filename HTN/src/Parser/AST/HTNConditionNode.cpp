#include "Parser/AST/HTNConditionNode.h"

#include "Database/HTNWorldState.h"
#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/HTNConditionQuery.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNDecompositionScope.h"
#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <cassert>
#include <format>

HTNAtom HTNConditionNodeBase::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNConditionNode::HTNConditionNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

std::string HTNConditionNode::ToString() const
{
    std::string Description = GetIDNode()->ToString();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
    {
        Description.append(std::format("{} ", ArgumentNode->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}

bool HTNConditionNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    const std::string       ID                   = GetID();
    const std::string       FactID               = GetIDNode()->ToString();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const HTNWorldState*    WorldState           = ioDecompositionContext.GetWorldState();

    // Gather arguments
    std::vector<HTNAtom*> Arguments;
    std::vector<HTNAtom>  Literals;
    bool                  ShouldBindArguments = false;

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
    {
        const HTNAtom* ArgumentValue        = &ArgumentNode->GetValue();
        HTNAtom*       ArgumentValueMutable = nullptr;

        // TODO salvarez Merge with HTN::Helpers::CopyArguments somehow
        // Arguments can be variables, constants, or literals
        switch (ArgumentNode->GetType())
        {
        case HTNValueType::VARIABLE: {
            // If argument is a variable, get or add a variable for it in its scope
            std::string ArgumentValueString = ArgumentValue->GetValue<std::string>();
            ArgumentValueMutable            = &CurrentDecomposition.GetCurrentEnvironment().GetOrAddVariable(ArgumentValueString);
            ShouldBindArguments             = ShouldBindArguments || !ArgumentValueMutable->IsSet();
            break;
        }
        case HTNValueType::CONSTANT: {
            const std::shared_ptr<const HTNDomainNode>& CurrentDomainNode   = ioDecompositionContext.GetCurrentDomainNode();
            std::string                                 ArgumentValueString = ArgumentValue->GetValue<std::string>();
            std::shared_ptr<const HTNConstantNode>      ConstantNode        = CurrentDomainNode->FindConstantNodeByID(ArgumentValueString);
            if (!ConstantNode)
            {
                LOG_ERROR("Constant node [{}] not found", ArgumentValueString);
                return false;
            }

            ArgumentValue = &ConstantNode->GetValueNode()->GetValue();
        }
        case HTNValueType::LITERAL: {
            // If argument is a literal or a constant, create a temporary copy for it
            ArgumentValueMutable = &Literals.emplace_back(*ArgumentValue);
            break;
        }
        case HTNValueType::SYMBOL:
        case HTNValueType::AXIOM:
        default:
            assert(false);
        }

        Arguments.emplace_back(ArgumentValueMutable);
    }

    // Check bound arguments and bind unbound arguments
    const HTNConditionQueryWorldState Condition        = HTNConditionQueryWorldState(FactID, Arguments);
    const unsigned int                ArgumentsCount   = static_cast<unsigned int>(Arguments.size());
    const unsigned int                FactEntriesCount = WorldState->GetNumFactEntries(FactID, ArgumentsCount);
    for (unsigned int CurrentFactEntryIndex                              = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ID);
         CurrentFactEntryIndex < FactEntriesCount; CurrentFactEntryIndex = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ID))
    {
        if (Condition.Check(*WorldState, CurrentFactEntryIndex))
        {
            outHasBoundArguments = ShouldBindArguments;

            return true;
        }
    }

    outHasBoundArguments = false;

    return false;
}

HTNConditionAxiomNode::HTNConditionAxiomNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                             const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

std::string HTNConditionAxiomNode::ToString() const
{
    std::string Description = GetIDNode()->ToString();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
    {
        Description.append(std::format("{} ", ArgumentNode->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}

bool HTNConditionAxiomNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    outHasBoundArguments = false;

    const std::string                           AxiomID           = GetIDNode()->ToString();
    const std::shared_ptr<const HTNDomainNode>& CurrentDomainNode = ioDecompositionContext.GetCurrentDomainNode();
    const std::shared_ptr<const HTNAxiomNode>   CurrentAxiomNode  = CurrentDomainNode->FindAxiomNodeByID(AxiomID);
    if (!CurrentAxiomNode)
    {
        LOG_ERROR("Axiom node [{}] not found", AxiomID);
        return false;
    }

    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    HTNDecompositionScope   CurrentAxiomScope    = HTNDecompositionScope(ioDecompositionContext);

    // Initialize the input arguments of the axiom with the arguments of the condition
    const std::vector<std::shared_ptr<const HTNValueNode>>& CurrentAxiomArgumentNodes = CurrentAxiomNode->GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsNoConst(ioDecompositionContext, mArgumentNodes, CurrentAxiomArgumentNodes,
                                            CurrentDecomposition.GetPreviousEnvironment(), CurrentDecomposition.GetCurrentEnvironment(), {},
                                            HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from condition to axiom [{}]", AxiomID);
        return false;
    }

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentConditionNode = CurrentAxiomNode->GetConditionNode();
    if (!CurrentConditionNode)
    {
        // If empty, it evaluates to true
        return true;
    }

    bool       HasBoundArguments = false;
    const bool Result            = CurrentConditionNode->Check(ioDecompositionContext, HasBoundArguments);
    if (!Result)
    {
        return false;
    }

    // Copy back the output arguments of the axiom
    if (!HTN::Helpers::CopyArgumentsConst(ioDecompositionContext, CurrentAxiomArgumentNodes, mArgumentNodes,
                                          CurrentDecomposition.GetCurrentEnvironment(), CurrentDecomposition.GetPreviousEnvironment(),
                                          HTN::Helpers::OutputPrefixes, {}))
    {
        LOG_ERROR("Arguments could not be copied from axiom to condition [{}]", AxiomID);
        return false;
    }

    return true;
}

HTNConditionAndNode::HTNConditionAndNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

std::string HTNConditionAndNode::ToString() const
{
    std::string Description = "and";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

bool HTNConditionAndNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex                                    = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentConditionIndex < mConditionNodes.size(); CurrentConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        // Check condition
        const std::shared_ptr<const HTNConditionNodeBase>& CurrentConditionNode = mConditionNodes[CurrentConditionIndex];
        bool                                               HasBoundArguments    = false;
        if (!CurrentConditionNode->Check(ioDecompositionContext, HasBoundArguments))
        {
            if (CurrentConditionIndex == 0) // First condition
            {
                return false;
            }
            else
            {
                // Restore state: unbound variables but updated indices
                ioDecompositionContext.RestoreDecomposition();
                continue;
            }
        }

        // Record decomposition if it has bound any arguments
        if (HasBoundArguments)
        {
            HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
            HTNEnvironment&        NewEnvironment   = NewDecomposition.GetCurrentEnvironment();
            NewEnvironment.SetVariables(CurrentVariables);
            ioDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to true
    return true;
}

HTNConditionOrNode::HTNConditionOrNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

std::string HTNConditionOrNode::ToString() const
{
    std::string Description = "or";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

bool HTNConditionOrNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // NO backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex                                    = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentConditionIndex < mConditionNodes.size(); CurrentConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        const std::shared_ptr<const HTNConditionNodeBase>& CurrentConditionNode = mConditionNodes[CurrentConditionIndex];
        bool                                               HasBoundArguments    = false;
        if (CurrentConditionNode->Check(ioDecompositionContext, HasBoundArguments))
        {
            return true;
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return false;
}

HTNConditionAltNode::HTNConditionAltNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

std::string HTNConditionAltNode::ToString() const
{
    std::string Description = "alt";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

bool HTNConditionAltNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex                                    = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentConditionIndex < mConditionNodes.size(); CurrentConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        const std::shared_ptr<const HTNConditionNodeBase>& CurrentConditionNode = mConditionNodes[CurrentConditionIndex];
        bool                                               HasBoundArguments    = false;
        if (CurrentConditionNode->Check(ioDecompositionContext, HasBoundArguments))
        {
            // Record decomposition if it has bound any arguments
            if (HasBoundArguments)
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                HTNEnvironment&        NewEnvironment   = NewDecomposition.GetCurrentEnvironment();
                NewEnvironment.SetVariables(CurrentVariables);
                ioDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            return true;
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return false;
}

HTNConditionNotNode::HTNConditionNotNode(const std::shared_ptr<const HTNConditionNodeBase>& inConditionNode) : mConditionNode(inConditionNode)
{
}

std::string HTNConditionNotNode::ToString() const
{
    return std::format("not {}", mConditionNode->ToString());
}

bool HTNConditionNotNode::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // NO bindings
    // NO backtracking

    outHasBoundArguments = false;

    // Copy variables
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = ioDecompositionContext.GetDecompositionHistory();

    // Check condition
    bool       HasBoundArguments = false;
    const bool Result            = mConditionNode->Check(ioDecompositionContext, HasBoundArguments);

    // Reset decomposition history
    ioDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.GetCurrentEnvironment().SetVariables(CurrentVariables);

    // Invert result
    return !Result;
}
