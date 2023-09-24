#include "Interpreter/HTNInterpreter.h"

#include "Database/HTNWorldState.h"
#include "HTNLog.h"
#include "Interpreter/HTNConditionQuery.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNDecompositionScope.h"
#include "Interpreter/HTNEnvironment.h"
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

#include <cassert>

namespace
{
bool RemovePrefix(const std::string& inPrefix, std::string& ioString)
{
    const size_t Position = ioString.find(inPrefix);
    if (Position == std::string::npos)
    {
        return false;
    }

    ioString = ioString.erase(Position, inPrefix.size());

    return true;
}

const std::string InputPrefix       = "inp_";
const std::string OutputPrefix      = "out_";
const std::string InputOutputPrefix = "io_";

bool RemoveInputPrefix(std::string& ioString)
{
    return RemovePrefix(InputPrefix, ioString) || RemovePrefix(InputOutputPrefix, ioString);
}

bool RemoveOutputPrefix(std::string& ioString)
{
    return RemovePrefix(OutputPrefix, ioString) || RemovePrefix(InputOutputPrefix, ioString);
}

bool IsNodeSuccessful(const HTNAtomList& inNodeValue)
{
    static constexpr size_t IsNodeSuccessfulIndex = 0;
    return inNodeValue.Find(IsNodeSuccessfulIndex);
}

bool HasNodeBoundArguments(const HTNAtomList& inNodeValue)
{
    static constexpr size_t HasNodeBoundArgumentsIndex = 1;
    return inNodeValue.Find(HasNodeBoundArgumentsIndex);
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
    const bool                  IsDomainNodeSuccessful = GetNodeValue(*DomainNode).GetValue<bool>();
    if (!IsDomainNodeSuccessful)
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

        const HTNEnvironment&       Environment = TaskInstanceToProcess.GetEnvironment();
        const HTNDecompositionScope TaskScope   = HTNDecompositionScope(mDecompositionContext, Environment);

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstanceToProcess.GetTaskNode();
        const bool                                    IsTaskNodeSuccessful = GetNodeValue(*TaskNode).GetValue<bool>();
        if (!IsTaskNodeSuccessful)
        {
            return false;
        }
    }

    return true;
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNConstantsNode& inConstantsNode)
{
    // TODO salvarez Remove HTNConstantsNode
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit(const HTNConstantNode& inConstantNode)
{
    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        return true;
    }

    return GetNodeValue(*ConditionNode);
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

    const std::shared_ptr<const HTNBranchNode>& BranchNode           = BranchNodes[BranchIndex];
    const bool                                  IsBranchNodeSuccessful = GetNodeValue(*BranchNode).GetValue<bool>();
    if (IsBranchNodeSuccessful)
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

    const HTNAtomList ConditionNodeValue = GetNodeValue(*CurrentPreConditionNode).GetValue<HTNAtomList>();
    return IsNodeSuccessful(ConditionNodeValue);
}

HTNAtom HTNInterpreter::Visit(const HTNConditionNode& inConditionNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNWorldState*    WorldState           = mDecompositionContext.GetWorldState();

    // Gather arguments
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<HTNAtom*>                                       Arguments;
    std::vector<HTNAtom>                                        ArgumentsTemp; // TODO salvarez Delete ArgumentsTemp
    Arguments.reserve(ArgumentNodes.size());
    ArgumentsTemp.reserve(ArgumentNodes.size());
    bool HasBoundArguments = false;
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = GetNodeValue(*ArgumentNode);
        HasBoundArguments      = HasBoundArguments || !Argument.IsSet();
        Arguments.emplace_back(&ArgumentsTemp.emplace_back(Argument));
    }

    // Check bound arguments and bind unbound arguments
    const std::shared_ptr<const HTNValueNode>& ConditionNodeIDNode      = inConditionNode.GetIDNode();
    const std::string                          FactID                   = GetNodeValue(*ConditionNodeIDNode).GetValue<std::string>();
    const HTNConditionQueryWorldState          ConditionQueryWorldState = HTNConditionQueryWorldState(FactID, Arguments);
    const unsigned int                         ArgumentsCount           = static_cast<unsigned int>(Arguments.size());
    const unsigned int                         FactEntriesCount         = WorldState->GetNumFactEntries(FactID, ArgumentsCount);
    const std::string                          ConditionNodeID          = inConditionNode.GetID();
    for (unsigned int FactEntryIndex                       = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ConditionNodeID);
         FactEntryIndex < FactEntriesCount; FactEntryIndex = CurrentDecomposition.GetCurrentEnvironment().AddOrIncrementIndex(ConditionNodeID))
    {
        if (ConditionQueryWorldState.Check(*WorldState, FactEntryIndex))
        {
            for (size_t i = 0; i < ArgumentNodes.size(); ++i)
            {
                const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = ArgumentNodes[i];
                const HTNAtom*                                 Argument     = Arguments[i];
                SetNodeValue(*ArgumentNode, *Argument);
            }

            return HTNAtomList({true, HasBoundArguments});
        }
    }

    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const std::shared_ptr<const HTNValueNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                          AxiomNodeID              = GetNodeValue(*AxiomConditionNodeIDNode).GetValue<std::string>();
    const std::shared_ptr<const HTNAxiomNode>  AxiomNode                = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not found", AxiomNodeID);
        return HTNAtomList({false});
    }

    std::vector<HTNAtom>                                        AxiomConditionNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    AxiomConditionNodeArguments.reserve(AxiomConditionNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        const HTNAtom AxiomConditionNodeArgument = GetNodeValue(*AxiomConditionNodeArgumentNode);
        AxiomConditionNodeArguments.emplace_back(AxiomConditionNodeArgument);
    }

    std::vector<HTNAtom> AxiomNodeArguments;

    {
        HTNDecompositionScope AxiomScope = HTNDecompositionScope(mDecompositionContext);

        // Initialize the input arguments of the axiom node with the arguments of the condition node
        const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomNodeArgumentNodes = AxiomNode->GetArgumentNodes();
        for (size_t i = 0; i < AxiomNodeArgumentNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNValueNodeBase>& AxiomNodeArgumentNode      = AxiomNodeArgumentNodes[i];
            const HTNAtom&                                 AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
            SetNodeValue(*AxiomNodeArgumentNode, AxiomConditionNodeArgument);
        }

        const HTNAtomList AxiomNodeValue = GetNodeValue(*AxiomNode).GetValue<HTNAtomList>();
        if (!IsNodeSuccessful(AxiomNodeValue))
        {
            return HTNAtomList({false});
        }

        AxiomNodeArguments.reserve(AxiomNodeArgumentNodes.size());
        for (const std::shared_ptr<const HTNValueNodeBase>& AxiomNodeArgumentNode : AxiomNodeArgumentNodes)
        {
            const HTNAtom AxiomNodeArgument = GetNodeValue(*AxiomNodeArgumentNode);
            AxiomNodeArguments.emplace_back(AxiomNodeArgument);
        }
    }

    // Initialize the arguments of the condition node with the output arguments of the axiom node
    for (size_t i = 0; i < AxiomConditionNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueNodeBase>& AxiomConditionNodeArgumentNode = AxiomConditionNodeArgumentNodes[i];
        const HTNAtom&                                 AxiomNodeArgument              = AxiomNodeArguments[i];
        SetNodeValue(*AxiomConditionNodeArgumentNode, AxiomNodeArgument);
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
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (!IsNodeSuccessful(ConditionNodeValue))
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
        if (HasNodeBoundArguments(ConditionNodeValue))
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
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (IsNodeSuccessful(ConditionNodeValue))
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

        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (IsNodeSuccessful(ConditionNodeValue))
        {
            // Record decomposition if it has bound any arguments
            if (HasNodeBoundArguments(ConditionNodeValue))
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
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = inNotConditionNode.GetSubConditionNode();
    const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();

    // Reset decomposition history
    mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.GetCurrentEnvironment().SetVariables(Variables);

    // Invert result
    if (IsNodeSuccessful(ConditionNodeValue))
    {
        return HTNAtomList({false});
    }

    static constexpr bool HasBoundArguments = false;
    return HTNAtomList({true, HasBoundArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    // The ID of a compound task should match the ID of a method
    const std::string                          CompoundTaskNodeID = inCompoundTaskNode.GetID();
    const std::shared_ptr<const HTNMethodNode> MethodNode         = mDomainNode->FindMethodNodeByID(CompoundTaskNodeID);
    if (!MethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", CompoundTaskNodeID);
        return false;
    }

    std::vector<HTNAtom>                                        CompoundTaskNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    CompoundTaskNodeArguments.reserve(CompoundTaskNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        const HTNAtom CompoundTaskNodeArgument = GetNodeValue(*CompoundTaskNodeArgumentNode);
        CompoundTaskNodeArguments.emplace_back(CompoundTaskNodeArgument);
    }

    const HTNDecompositionScope MethodScope = HTNDecompositionScope(mDecompositionContext);

    // Initialize the input arguments of the method node with the arguments of the compound task node
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& MethodNodeArgumentNodes = MethodNode->GetArgumentNodes();
    for (size_t i = 0; i < MethodNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueNodeBase>& MethodNodeArgumentNode   = MethodNodeArgumentNodes[i];
        const HTNAtom&                                 CompoundTaskNodeArgument = CompoundTaskNodeArguments[i];
        SetNodeValue(*MethodNodeArgumentNode, CompoundTaskNodeArgument);
    }

    return GetNodeValue(*MethodNode);
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

void HTNInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveInputPrefix(VariableID);
    CurrentDecomposition.GetCurrentEnvironment().SetVariable(VariableID, inVariableValueNodeValue);
}

HTNAtom HTNInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveOutputPrefix(VariableID);
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

    return GetNodeValue(*ConstantNode);
}
