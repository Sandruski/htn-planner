#include "Interpreter/HTNInterpreter.h"

#include "Database/HTNWorldState.h"
#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/HTNConditionQuery.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNDecompositionScope.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"
#include "Parser/HTNToken.h"

#include <cassert>

HTNInterpreter::HTNInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointName,
                               const HTNWorldState& inWorldState)
    : mDomainNode(inDomainNode), mEntryPointName(inEntryPointName), mDecompositionContext(inWorldState)
{
}

HTNInterpreter::~HTNInterpreter() = default;

bool HTNInterpreter::Interpret(std::vector<HTNTaskInstance>& outPlan)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    const bool Result = EvaluateNode<bool>(*DomainNode);
    if (!Result)
    {
        return false;
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    outPlan                                            = CurrentDecomposition.GetPlan();

    return true;
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNDomainNode& inDomainNode)
{
    const HTNDecompositionScope CurrentDomainScope = HTNDecompositionScope(mDecompositionContext);

    // Dummy root task node
    HTNDecompositionRecord&                          CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNCompoundTaskNode> RootTaskNode         = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(HTNAtom(mEntryPointName), HTNValueType::SYMBOL), std::vector<std::shared_ptr<const HTNValueNode>>());
    CurrentDecomposition.PushTaskInstanceToProcess(RootTaskNode);

    while (CurrentDecomposition.HasTaskInstancesToProcess())
    {
        const HTNTaskInstance& CurrentTaskInstance = CurrentDecomposition.PopTaskInstanceToProcess();
        CurrentDecomposition.SetCurrentTaskInstance(CurrentTaskInstance);

        const std::shared_ptr<const HTNTaskNodeBase>& CurrentTaskNode = CurrentTaskInstance.GetTaskNode();
        if (!CurrentTaskNode)
        {
            return false;
        }

        const bool Result = EvaluateNode<bool>(*CurrentTaskNode);
        if (!Result)
        {
            return false;
        }
    }

    return true;
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNConstantsNode& inConstantsNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNConstantNode& inConstantNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNAxiomNode& inAxiomNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNDecompositionScope CurrentMethodScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the method with the arguments of the compound task
    HTNDecompositionRecord&                                 CurrentDecomposition          = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNTaskInstance&                                  CurrentTaskInstance           = CurrentDecomposition.GetCurrentTaskInstance();
    const std::shared_ptr<const HTNTaskNodeBase>&           CurrentTaskNode               = CurrentTaskInstance.GetTaskNode();
    const std::vector<std::shared_ptr<const HTNValueNode>>& CurentTaskNodeArgumentNodes   = CurrentTaskNode->GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNValueNode>>& CurentMethodNodeArgumentNodes = inMethodNode.GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsConst(mDecompositionContext, CurentTaskNodeArgumentNodes, CurentMethodNodeArgumentNodes,
                                          CurrentTaskInstance.GetEnvironment(), CurrentDecomposition.GetCurrentEnvironment(), {},
                                          HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from compound task [{}] to method [{}]", CurrentTaskNode->GetID(), inMethodNode.GetID());
        return false;
    }

    const std::string  CurrentMethodID    = inMethodNode.GetID();
    const unsigned int CurrentBranchIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(CurrentMethodID);
    const std::vector<std::shared_ptr<const HTNBranchNode>>& CurrentBranchNodes = inMethodNode.GetBranchNodes();
    if (CurrentBranchIndex >= CurrentBranchNodes.size())
    {
        return false;
    }

    const std::shared_ptr<const HTNBranchNode>& CurrentBranchNode = CurrentBranchNodes[CurrentBranchIndex];
    const bool                                  Result            = EvaluateNode<bool>(*CurrentBranchNode);
    if (Result)
    {
        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
        for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
        {
            const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
            CurrentDecomposition.PushTaskInstanceToProcess(TaskNode);
        }
    }
    else
    {
        if (CurrentBranchIndex < CurrentBranchNodes.size() - 1)
        {
            // Continue
            CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(inMethodNode.GetID());
        }
        else // Last branch
        {
            // Restore state: unbound variables but updated indices
            mDecompositionContext.RestoreDecomposition();
        }

        const HTNTaskInstance& CurrentTaskInstance = CurrentDecomposition.GetCurrentTaskInstance();
        CurrentDecomposition.PushTaskInstanceToProcess(CurrentTaskInstance.GetTaskNode());
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (!CurrentPreConditionNode)
    {
        return true;
    }

    return EvaluateNode<bool>(*CurrentPreConditionNode);
}

// TODO salvarez Check whether variables have been bound or not
HTNAtom HTNInterpreter::Visit(const HTNConditionNode& inConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
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

HTNAtom HTNInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    // The ID of a compound task should match the ID of a method
    const std::string                          CurrentCompoundTaskID = inCompoundTaskNode.GetID();
    const std::shared_ptr<const HTNMethodNode> CurrentMethodNode     = mDomainNode->FindMethodNodeByID(CurrentCompoundTaskID);
    if (!CurrentMethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", CurrentCompoundTaskID);
        return false;
    }

    return EvaluateNode<bool>(*CurrentMethodNode);
}

HTNAtom HTNInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNTaskInstance&  CurrentTaskInstance  = CurrentDecomposition.GetCurrentTaskInstance();
    assert(CurrentTaskInstance.GetTaskNode().get() == &inPrimitiveTaskNode);
    CurrentDecomposition.AddTaskToPlan(CurrentTaskInstance);
    return true;
}

// TODO salvarez Do subclasses of HTNValueNode for constants, axioms, etc.
HTNAtom HTNInterpreter::Visit(const HTNValueNode& inValueNode)
{
    return inValueNode.GetValue();
}
