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

    const HTNDecompositionScope DomainScope = HTNDecompositionScope(mDecompositionContext);
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
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root task node
    const std::shared_ptr<const HTNCompoundTaskNode> RootCompoundTaskNode = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(mEntryPointName), std::vector<std::shared_ptr<const HTNValueNodeBase>>());
    CurrentDecomposition.PushTaskInstanceToProcess(RootCompoundTaskNode);

    while (CurrentDecomposition.HasTaskInstancesToProcess())
    {
        const HTNTaskInstance& TaskInstanceToProcess = CurrentDecomposition.PopTaskInstanceToProcess();
        CurrentDecomposition.SetCurrentTaskInstance(TaskInstanceToProcess);

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstanceToProcess.GetTaskNode();
        if (!EvaluateNode<bool>(*TaskNode))
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
    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        return true;
    }

    return EvaluateNode<HTNAtomList>(*ConditionNode);
}

HTNAtom HTNInterpreter::Visit(const HTNMethodNode& inMethodNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::string                                        MethodID    = inMethodNode.GetID();
    const unsigned int                                       BranchIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(MethodID);
    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    if (BranchIndex >= BranchNodes.size())
    {
        return false;
    }

    const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
    if (EvaluateNode<bool>(*BranchNode))
    {
        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = BranchNode->GetTaskNodes();
        for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
        {
            const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
            CurrentDecomposition.PushTaskInstanceToProcess(TaskNode);
        }
    }
    else
    {
        if (BranchIndex < BranchNodes.size() - 1)
        {
            // Continue
            CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(inMethodNode.GetID());
        }
        else // Last branch
        {
            // Restore state: unbound variables but updated indices
            mDecompositionContext.RestoreDecomposition();
        }

        const HTNTaskInstance&                        CurrentTaskInstance = CurrentDecomposition.GetCurrentTaskInstance();
        const std::shared_ptr<const HTNTaskNodeBase>& CurrentTaskNode     = CurrentTaskInstance.GetTaskNode();
        CurrentDecomposition.PushTaskInstanceToProcess(CurrentTaskNode);
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
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNWorldState*    WorldState           = mDecompositionContext.GetWorldState();

    // Gather arguments
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<HTNAtom*>                                       Arguments;
    std::vector<HTNAtom>                                        ArgumentsTemp;
    Arguments.reserve(ArgumentNodes.size());
    Arguments.reserve(ArgumentsTemp.size());
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = EvaluateNode(*ArgumentNode);
        Arguments.emplace_back(ArgumentsTemp.emplace_back(Argument));
    }

    bool HasBoundArguments = false; // TODO salvarez

    // Check bound arguments and bind unbound arguments
    const std::shared_ptr<const HTNValueNode>& ConditionNodeIDNode      = inConditionNode.GetIDNode();
    const std::string                          FactID                   = EvaluateNode<std::string>(*ConditionNodeIDNode);
    const HTNConditionQueryWorldState          ConditionQueryWorldState = HTNConditionQueryWorldState(FactID, Arguments);
    const unsigned int                         ArgumentsCount           = static_cast<unsigned int>(Arguments.size());
    const unsigned int                         FactEntriesCount         = WorldState->GetNumFactEntries(FactID, ArgumentsCount);
    const std::string                          ConditionNodeID          = inConditionNode.GetID();
    for (unsigned int FactEntryIndex                       = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ConditionNodeID);
         FactEntryIndex < FactEntriesCount; FactEntryIndex = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ConditionNodeID))
    {
        if (ConditionQueryWorldState.Check(*WorldState, FactEntryIndex))
        {
            return HTNAtomList({true, HasBoundArguments});
        }
    }

    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNValueNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                          AxiomNodeID              = EvaluateNode<std::string>(*AxiomConditionNodeIDNode);
    const std::shared_ptr<const HTNAxiomNode>  AxiomNode                = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not found", AxiomNodeID);
        return HTNAtomList({false});
    }

    HTNDecompositionScope AxiomScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the axiom with the arguments of the condition node

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomNodeArgumentNodes          = AxiomNode->GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsNoConst(mDomainNode, AxiomConditionNodeArgumentNodes, AxiomNodeArgumentNodes,
                                            CurrentDecomposition.GetPreviousEnvironment(), CurrentDecomposition.GetCurrentEnvironment(), {},
                                            HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from condition node [{}] to axiom node [{}]", inAxiomConditionNode.GetID(), AxiomNodeID);
        return HTNAtomList({false});
    }

    const HTNAtomList AxiomNodeResult = EvaluateNode<HTNAtomList>(*AxiomNode);
    if (!IsNodeSuccessful(AxiomNodeResult))
    {
        return HTNAtomList({false});
    }

    // Copy back the output arguments of the axiom node to the condition node
    if (!HTN::Helpers::CopyArgumentsConst(mDomainNode, AxiomNodeArgumentNodes, AxiomConditionNodeArgumentNodes,
                                          CurrentDecomposition.GetCurrentEnvironment(), CurrentDecomposition.GetPreviousEnvironment(),
                                          HTN::Helpers::OutputPrefixes, {}))
    {
        LOG_ERROR("Arguments could not be copied from axiom node [{}] to condition node [{}]", AxiomNodeID, inAxiomConditionNode.GetID());
        return HTNAtomList({false});
    }

    static constexpr bool HasBoundArguments = false;
    return HTNAtomList({true, HasBoundArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    // YES bindings
    // YES backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes  = inAndConditionNode.GetSubConditionNodes();
    const std::string                                               AndConditionNodeID = inAndConditionNode.GetID();
    for (unsigned int SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(AndConditionNodeID);
         SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(AndConditionNodeID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> Variables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        // Check condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode    = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult = EvaluateNode<HTNAtomList>(*SubConditionNode);
        if (!IsNodeSuccessful(ConditionNodeResult))
        {
            if (SubConditionIndex == 0) // First condition
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
            NewEnvironment.SetVariables(Variables);
            mDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(AndConditionNodeID);
    }

    // If empty, it evaluates to true
    static constexpr bool HasBoundArguments = false;
    return HTNAtomList({true, HasBoundArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    // YES bindings
    // NO backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    const std::string                                               OrConditionNodeID = inOrConditionNode.GetID();
    for (unsigned int SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(OrConditionNodeID);
         SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(OrConditionNodeID))
    {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode    = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult = EvaluateNode<HTNAtomList>(*SubConditionNode);
        if (IsNodeSuccessful(ConditionNodeResult))
        {
            static constexpr bool HasBoundArguments = false;
            return HTNAtomList({true, HasBoundArguments});
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(OrConditionNodeID);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    // YES bindings
    // YES backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes  = inAltConditionNode.GetSubConditionNodes();
    const std::string                                               AltConditionNodeID = inAltConditionNode.GetID();
    for (unsigned int SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(AltConditionNodeID);
         SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex = CurrentDecomposition.GetCurrentEnvironment().GetOrAddIndex(AltConditionNodeID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> Variables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode    = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeResult = EvaluateNode<HTNAtomList>(*SubConditionNode);
        if (IsNodeSuccessful(ConditionNodeResult))
        {
            // Record decomposition if it has bound any arguments
            if (HasNodeBoundArguments(ConditionNodeResult))
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                HTNEnvironment&        NewEnvironment   = NewDecomposition.GetCurrentEnvironment();
                NewEnvironment.SetVariables(Variables);
                mDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            static constexpr bool HasBoundArguments = false;
            return HTNAtomList({true, HasBoundArguments});
        }

        // Continue
        CurrentDecomposition.GetCurrentEnvironment().IncrementIndex(AltConditionNodeID);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    // NO bindings
    // NO backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Copy variables
    const std::unordered_map<std::string, HTNAtom> Variables = CurrentDecomposition.GetCurrentEnvironment().GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext.GetDecompositionHistory();

    // Check condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode    = inNotConditionNode.GetSubConditionNode();
    const HTNAtomList                                  ConditionNodeResult = EvaluateNode<HTNAtomList>(*SubConditionNode);

    // Reset decomposition history
    mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.GetCurrentEnvironment().SetVariables(Variables);

    // Invert result
    if (IsNodeSuccessful(ConditionNodeResult))
    {
        return HTNAtomList({false});
    }

    static constexpr bool HasBoundArguments = false;
    return HTNAtomList({true, HasBoundArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // The ID of a compound task should match the ID of a method
    const std::string                          CompoundTaskNodeID = inCompoundTaskNode.GetID();
    const std::shared_ptr<const HTNMethodNode> MethodNode         = mDomainNode->FindMethodNodeByID(CompoundTaskNodeID);
    if (!MethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", CompoundTaskNodeID);
        return false;
    }

    const HTNDecompositionScope MethodScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the method with the arguments of the compound task
    const HTNTaskInstance& CurrentTaskInstance = CurrentDecomposition.GetCurrentTaskInstance();
    assert(CurrentTaskInstance.GetTaskNode().get() == &inCompoundTaskNode);
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& MethodNodeArgumentNodes       = MethodNode->GetArgumentNodes();
    if (!HTN::Helpers::CopyArgumentsConst(mDomainNode, CompoundTaskNodeArgumentNodes, MethodNodeArgumentNodes, CurrentTaskInstance.GetEnvironment(),
                                          CurrentDecomposition.GetCurrentEnvironment(), {}, HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from compound task node [{}] to method node of the same name", CompoundTaskNodeID);
        return false;
    }

    return EvaluateNode<bool>(*MethodNode);
}

HTNAtom HTNInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const HTNTaskInstance& CurrentTaskInstance = CurrentDecomposition.GetCurrentTaskInstance();
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

    const std::string& VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    return CurrentDecomposition.GetCurrentEnvironment().GetOrAddVariable(VariableID);
}

HTNAtom HTNInterpreter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtomList({false});
    }

    return EvaluateNode<HTNAtom>(*ConstantNode);
}
