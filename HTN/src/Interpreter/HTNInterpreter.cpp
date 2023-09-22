#include "Interpreter/HTNInterpreter.h"

#include "Database/HTNWorldState.h"
#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/HTNConditionQuery.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNDecompositionScope.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNConstantsNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"
#include "Parser/HTNToken.h"

#include <cassert>

namespace
{
bool IsNodeSuccessful(const HTNAtomList& inNodeResult)
{
    static constexpr size_t IsNodeSuccessfulIndex = 0;
    return inNodeResult.Find(IsNodeSuccessfulIndex);
}

bool HasNodeBoundArguments(const HTNAtomList& inNodeResult)
{
    static constexpr size_t HasNodeBoundArgumentsIndex = 1;
    return inNodeResult.Find(HasNodeBoundArgumentsIndex);
}
} // namespace

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

    const HTNDecompositionScope CurrentDomainScope = HTNDecompositionScope(mDecompositionContext);
    if (!EvaluateNode<bool>(*DomainNode))
    {
        return false;
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    outPlan                                            = CurrentDecomposition.GetPlan();

    return true;
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNDomainNode& inDomainNode)
{
    // Dummy root task node
    HTNDecompositionRecord&                          CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNCompoundTaskNode> RootTaskNode         = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(mEntryPointName), std::vector<std::shared_ptr<const HTNValueNodeBase>>());
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

        if (!EvaluateNode<bool>(*CurrentTaskNode))
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

HTNAtom HTNInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const std::shared_ptr<const HTNConditionNodeBase>& CurrentConditionNode = inAxiomNode.GetConditionNode();
    if (!CurrentConditionNode)
    {
        // If empty, it evaluates to true
        return true;
    }

    return EvaluateNode<HTNAtomList>(*CurrentConditionNode);
}

HTNAtom HTNInterpreter::Visit(const HTNMethodNode& inMethodNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const std::string       CurrentMethodID      = inMethodNode.GetID();
    const unsigned int      CurrentBranchIndex   = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(CurrentMethodID);
    const std::vector<std::shared_ptr<const HTNBranchNode>>& CurrentBranchNodes = inMethodNode.GetBranchNodes();
    if (CurrentBranchIndex >= CurrentBranchNodes.size())
    {
        return false;
    }

    const std::shared_ptr<const HTNBranchNode>& CurrentBranchNode = CurrentBranchNodes[CurrentBranchIndex];
    if (EvaluateNode<bool>(*CurrentBranchNode))
    {
        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = CurrentBranchNode->GetTaskNodes();
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
    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (!CurrentPreConditionNode)
    {
        return true;
    }

    const HTNAtomList ConditionNodeResult = EvaluateNode<HTNAtomList>(*CurrentPreConditionNode);
    return IsNodeSuccessful(ConditionNodeResult);
}

HTNAtom HTNInterpreter::Visit(const HTNConditionNode& inConditionNode)
{
    const std::string       ID                   = inConditionNode.GetID();
    const std::string       FactID               = inConditionNode.GetIDNode()->ToString();
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNWorldState*    WorldState           = mDecompositionContext.GetWorldState();

    // Gather arguments
    std::vector<HTNAtom*> Arguments;
    std::vector<HTNAtom>  Literals;
    bool                  ShouldBindArguments = false;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom* ArgumentValue        = &ArgumentNode->GetValue();
        HTNAtom*       ArgumentValueMutable = nullptr;

        // TODO salvarez Merge with HTN::Helpers::CopyArguments somehow
        // Arguments can be variables, constants, or literals
        switch (ArgumentNode->GetType())
        {
        case HTNValueType::VARIABLE: {
            // If argument is a variable, get or add a variable for it in its scope

            break;
        }
        case HTNValueType::CONSTANT: {
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
            return HTNAtomList({true, ShouldBindArguments});
        }
    }

    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const std::shared_ptr<const HTNValueNode>& CurrentAxiomNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                          CurrentAxiomNodeID     = EvaluateNode<std::string>(*CurrentAxiomNodeIDNode);
    const std::shared_ptr<const HTNAxiomNode>  CurrentAxiomNode       = mDomainNode->FindAxiomNodeByID(CurrentAxiomNodeID);
    if (!CurrentAxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not found", CurrentAxiomNodeID);
        return HTNAtomList({false});
    }

    HTNDecompositionScope CurrentAxiomScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the axiom with the arguments of the condition node
    HTNDecompositionRecord&                                     CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CurrentAxiomConditionArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CurrentAxiomArgumentNodes          = CurrentAxiomNode->GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsNoConst(mDomainNode, CurrentAxiomConditionArgumentNodes, CurrentAxiomArgumentNodes,
                                            CurrentDecomposition.GetPreviousEnvironment(), CurrentDecomposition.GetCurrentEnvironment(), {},
                                            HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from condition node [{}] to axiom node [{}]", inAxiomConditionNode.GetID(), CurrentAxiomNodeID);
        return HTNAtomList({false});
    }

    const HTNAtomList AxiomNodeResult = EvaluateNode<HTNAtomList>(*CurrentAxiomNode);
    if (!IsNodeSuccessful(AxiomNodeResult))
    {
        return HTNAtomList({false});
    }

    // Copy back the output arguments of the axiom node to the condition node
    if (!HTN::Helpers::CopyArgumentsConst(mDomainNode, CurrentAxiomArgumentNodes, CurrentAxiomConditionArgumentNodes,
                                          CurrentDecomposition.GetCurrentEnvironment(), CurrentDecomposition.GetPreviousEnvironment(),
                                          HTN::Helpers::OutputPrefixes, {}))
    {
        LOG_ERROR("Arguments could not be copied from axiom node [{}] to condition node [{}]", CurrentAxiomNodeID, inAxiomConditionNode.GetID());
        return HTNAtomList({false});
    }

    static constexpr bool CanBindArguments = false;
    return HTNAtomList({true, CanBindArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    // YES bindings
    // YES backtracking

    const std::string       ID                   = inAndConditionNode.GetID();
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    for (unsigned int CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentSubConditionIndex < SubConditionNodes.size();
         CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        // Check condition
        const std::shared_ptr<const HTNConditionNodeBase>& CurrentSubConditionNode = SubConditionNodes[CurrentSubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult     = EvaluateNode<HTNAtomList>(*CurrentSubConditionNode);
        if (!IsNodeSuccessful(ConditionNodeResult))
        {
            if (CurrentSubConditionIndex == 0) // First condition
            {
                return HTNAtomList({false});
            }
            else
            {
                // Restore state: unbound variables but updated indices
                mDecompositionContext.RestoreDecomposition();
                continue;
            }
        }

        // Record decomposition if it has bound any arguments
        if (HasNodeBoundArguments(ConditionNodeResult))
        {
            HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
            HTNEnvironment&        NewEnvironment   = NewDecomposition.GetCurrentEnvironment();
            NewEnvironment.SetVariables(CurrentVariables);
            mDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to true
    static constexpr bool CanBindArguments = false;
    return HTNAtomList({true, CanBindArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    // YES bindings
    // NO backtracking

    const std::string       ID                   = inOrConditionNode.GetID();
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    for (unsigned int CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentSubConditionIndex < SubConditionNodes.size();
         CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        const std::shared_ptr<const HTNConditionNodeBase>& CurrentSubConditionNode = SubConditionNodes[CurrentSubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult     = EvaluateNode<HTNAtomList>(*CurrentSubConditionNode);
        if (IsNodeSuccessful(ConditionNodeResult))
        {
            static constexpr bool CanBindArguments = false;
            return HTNAtomList({true, CanBindArguments});
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    // YES bindings
    // YES backtracking

    const std::string       ID                   = inAltConditionNode.GetID();
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    for (unsigned int CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID);
         CurrentSubConditionIndex < SubConditionNodes.size();
         CurrentSubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        const std::shared_ptr<const HTNConditionNodeBase>& CurrentSubConditionNode = SubConditionNodes[CurrentSubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult     = EvaluateNode<HTNAtomList>(*CurrentSubConditionNode);
        if (IsNodeSuccessful(ConditionNodeResult))
        {
            // Record decomposition if it has bound any arguments
            if (HasNodeBoundArguments(ConditionNodeResult))
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                HTNEnvironment&        NewEnvironment   = NewDecomposition.GetCurrentEnvironment();
                NewEnvironment.SetVariables(CurrentVariables);
                mDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            static constexpr bool CanBindArguments = false;
            return HTNAtomList({true, CanBindArguments});
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    // NO bindings
    // NO backtracking

    // Copy variables
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext.GetDecompositionHistory();

    // Check condition
    const std::shared_ptr<const HTNConditionNodeBase>& CurrentSubConditionNode = inNotConditionNode.GetSubConditionNode();
    const HTNAtomList                                  SubConditionNodeResult  = EvaluateNode<HTNAtomList>(*CurrentSubConditionNode);

    // Reset decomposition history
    mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.GetCurrentEnvironment().SetVariables(CurrentVariables);

    // Invert result
    if (IsNodeSuccessful(SubConditionNodeResult))
    {
        return HTNAtomList({false});
    }

    static constexpr bool CanBindArguments = false;
    return HTNAtomList({true, CanBindArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    // The ID of a compound task should match the ID of a method
    const std::string                          CurrentCompoundTaskNodeID = inCompoundTaskNode.GetID();
    const std::shared_ptr<const HTNMethodNode> CurrentMethodNode         = mDomainNode->FindMethodNodeByID(CurrentCompoundTaskNodeID);
    if (!CurrentMethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", CurrentCompoundTaskNodeID);
        return false;
    }

    const HTNDecompositionScope CurrentMethodScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the method with the arguments of the compound task
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNTaskInstance&  CurrentTaskInstance  = CurrentDecomposition.GetCurrentTaskInstance();
    assert(CurrentTaskInstance.GetTaskNode().get() == &inCompoundTaskNode);
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CurrentTaskNodeArgumentNodes   = inCompoundTaskNode.GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CurrentMethodNodeArgumentNodes = CurrentMethodNode->GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsConst(mDomainNode, CurrentTaskNodeArgumentNodes, CurrentMethodNodeArgumentNodes,
                                          CurrentTaskInstance.GetEnvironment(), CurrentDecomposition.GetCurrentEnvironment(), {},
                                          HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from compound task node [{}] to method node of the same name", CurrentCompoundTaskNodeID);
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

HTNAtom HTNInterpreter::Visit(const HTNValueNode& inValueNode)
{
    return inValueNode.GetValue();
}

HTNAtom HTNInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         CurrentEnvironment   = CurrentDecomposition.GetCurrentEnvironment();
    const std::string&      VariableID           = inVariableValueNode.GetValue().GetValue<std::string>();
    return CurrentEnvironment.GetOrAddVariable(VariableID);
}

HTNAtom HTNInterpreter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string&                     CurrentConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> CurrentConstantNode   = mDomainNode->FindConstantNodeByID(CurrentConstantNodeID);
    if (!CurrentConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", CurrentConstantNodeID);
        return HTNAtomList({false});
    }

    return EvaluateNode<HTNAtom>(*CurrentConstantNode);
}
