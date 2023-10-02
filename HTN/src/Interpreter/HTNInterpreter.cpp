#include "Interpreter/HTNInterpreter.h"

#include "Domain/AST/HTNAxiomNode.h"
#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNConstantNode.h"
#include "Domain/AST/HTNConstantsNode.h"
#include "Domain/AST/HTNDomainNode.h"
#include "Domain/AST/HTNMethodNode.h"
#include "Domain/AST/HTNTaskNode.h"
#include "Domain/AST/HTNValueNode.h"
#include "HTNLog.h"
#include "HTNMacros.h"
#include "Interpreter/HTNConditionQuery.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNEnvironment.h"
#include "Interpreter/HTNNodeScope.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Interpreter/HTNVariableScope.h"
#include "WorldState/HTNWorldState.h"

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

bool RemoveVariableIDPrefixes(std::string& ioString)
{
    static const std::string InputPrefix       = "inp_";
    static const std::string OutputPrefix      = "out_";
    static const std::string InputOutputPrefix = "io_";
    return RemovePrefix(InputPrefix, ioString) || RemovePrefix(OutputPrefix, ioString) || RemovePrefix(InputOutputPrefix, ioString);
}

bool IsNodeSuccessful(const HTNAtomList& inNodeValue)
{
    static constexpr size_t IsNodeSuccessfulIndex = 0;
    return inNodeValue.Find(IsNodeSuccessfulIndex)->GetValue<bool>();
}

bool HasNodeBoundArguments(const HTNAtomList& inNodeValue)
{
    static constexpr size_t HasNodeBoundArgumentsIndex = 1;
    return inNodeValue.Find(HasNodeBoundArgumentsIndex)->GetValue<bool>();
}
} // namespace

HTNInterpreter::HTNInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointName,
                               const HTNWorldState& inWorldState)
    : mDomainNode(inDomainNode), mEntryPointName(inEntryPointName), mDecompositionContext(inWorldState)
{
}

HTNInterpreter::~HTNInterpreter() = default;

bool HTNInterpreter::Interpret(std::vector<HTNTaskResult>& outPlan)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    const bool IsDomainNodeSuccessful = GetNodeValue(*DomainNode).GetValue<bool>();
    if (!IsDomainNodeSuccessful)
    {
        return false;
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    outPlan                                            = CurrentDecomposition.GetPlan();

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string      DomainNodeID        = inDomainNode.GetID();
    const HTNNodeScope     DomainNodeScope     = HTNNodeScope(mDecompositionContext, DomainNodeID);
    const HTNVariableScope DomainVariableScope = HTNVariableScope(mDecompositionContext, DomainNodeID);

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root task node
    static constexpr bool                            IsIdentifier         = true;
    const std::shared_ptr<const HTNCompoundTaskNode> RootCompoundTaskNode = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(mEntryPointName, IsIdentifier), std::vector<std::shared_ptr<const HTNValueNodeBase>>());
    const std::string&    CurrentNodePath          = mDecompositionContext.GetCurrentNodePath();
    const std::string&    CurrentVariableScopePath = mDecompositionContext.GetCurrentVariableScopePath();
    const HTNTaskInstance RootCompoundTaskInstance =
        HTNTaskInstance(RootCompoundTaskNode, HTNEnvironment(), CurrentNodePath, CurrentVariableScopePath);
    CurrentDecomposition.PushPendingTaskInstance(RootCompoundTaskInstance);

    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance                         CurrentTaskInstance = CurrentDecomposition.PopPendingTaskInstance();
        const std::shared_ptr<const HTNTaskNodeBase>& CurrentTaskNode     = CurrentTaskInstance.GetTaskNode();
        CurrentDecomposition.SetCurrentTaskNode(CurrentTaskNode);
        CurrentDecomposition.SetEnvironment(CurrentTaskInstance.GetEnvironment());
        mDecompositionContext.SetCurrentNodePath(CurrentTaskInstance.GetNodePath());
        mDecompositionContext.SetCurrentVariableScopePath(CurrentTaskInstance.GetVariableScopePath());

        const bool IsTaskNodeSuccessful = GetNodeValue(*CurrentTaskNode).GetValue<bool>();
        if (!IsTaskNodeSuccessful)
        {
            return false;
        }
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNConstantNode& inConstantNode)
{
    const HTNNodeScope ConstantNodeScope = HTNNodeScope(mDecompositionContext, inConstantNode.GetID());

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNNodeScope AxiomNodeScope = HTNNodeScope(mDecompositionContext, inAxiomNode.GetID());

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
    const HTNNodeScope MethodNodeScope = HTNNodeScope(mDecompositionContext, inMethodNode.GetID());

    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const unsigned int                                       BranchIndex = Environment.AddIndex(CurrentNodePath);
    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    if (BranchIndex >= BranchNodes.size())
    {
        return false;
    }

    const std::string& CurrentVariableScopePath = mDecompositionContext.GetCurrentVariableScopePath();

    const std::shared_ptr<const HTNBranchNode>& BranchNode             = BranchNodes[BranchIndex];
    const bool                                  IsBranchNodeSuccessful = GetNodeValue(*BranchNode).GetValue<bool>();
    if (IsBranchNodeSuccessful)
    {
        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = BranchNode->GetTaskNodes();
        for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
        {
            const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
            const HTNTaskInstance TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentNodePath, CurrentVariableScopePath);
            CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
        }
    }
    else
    {
        if (BranchIndex < BranchNodes.size() - 1)
        {
            // Continue
            Environment.IncrementIndex(CurrentNodePath);
        }
        else // Last branch
        {
            // Restore state: unbound variables but updated indices
            mDecompositionContext.RestoreDecomposition();
        }

        const std::shared_ptr<const HTNTaskNodeBase>& CurrentTaskNode = CurrentDecomposition.GetCurrentTaskNode();
        const HTNTaskInstance CurrentTaskInstance = HTNTaskInstance(CurrentTaskNode, Environment, CurrentNodePath, CurrentVariableScopePath);
        CurrentDecomposition.PushPendingTaskInstance(CurrentTaskInstance);
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    const HTNNodeScope BranchNodeScope = HTNNodeScope(mDecompositionContext, inBranchNode.GetID());

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
    const HTNNodeScope ConditionNodeScope = HTNNodeScope(mDecompositionContext, inConditionNode.GetID());

    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
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
    for (unsigned int FactEntryIndex = Environment.AddOrIncrementIndex(CurrentNodePath); FactEntryIndex < FactEntriesCount;
         FactEntryIndex              = Environment.AddOrIncrementIndex(CurrentNodePath))
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
    const HTNNodeScope AxiomConditionNodeScope = HTNNodeScope(mDecompositionContext, inAxiomConditionNode.GetID());

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
        const HTNVariableScope AxiomVariableScope = HTNVariableScope(mDecompositionContext, AxiomNodeID);

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
    const HTNNodeScope AndConditionNodeScope = HTNNodeScope(mDecompositionContext, inAndConditionNode.GetID());

    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    // YES bindings
    // YES backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    for (int SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex     = Environment.GetIndex(CurrentNodePath))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> Variables = Environment.GetVariables();

        // Check condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (!IsNodeSuccessful(ConditionNodeValue))
        {
            // TODO salvarez Why this early out?
            // TODO salvarez Get function in conditions for index to backtrack using -1
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
            HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
            NewEnvironment.SetVariables(Variables);
            mDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        Environment.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to true
    static constexpr bool HasBoundArguments = false;
    return HTNAtomList({true, HasBoundArguments});
}

HTNAtom HTNInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const HTNNodeScope OrConditionNodeScope = HTNNodeScope(mDecompositionContext, inOrConditionNode.GetID());

    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    // YES bindings
    // NO backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    for (int SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex     = Environment.GetIndex(CurrentNodePath))
    {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (IsNodeSuccessful(ConditionNodeValue))
        {
            static constexpr bool HasBoundArguments = false;
            return HTNAtomList({true, HasBoundArguments});
        }

        // Continue
        Environment.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const HTNNodeScope AltConditionNodeScope = HTNNodeScope(mDecompositionContext, inAltConditionNode.GetID());

    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    // YES bindings
    // YES backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    for (int SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodes.size();
         SubConditionIndex     = Environment.GetIndex(CurrentNodePath))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> Variables = Environment.GetVariables();

        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = SubConditionNodes[SubConditionIndex];
        const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();
        if (IsNodeSuccessful(ConditionNodeValue))
        {
            // Record decomposition if it has bound any arguments
            if (HasNodeBoundArguments(ConditionNodeValue))
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
                NewEnvironment.SetVariables(Variables);
                mDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            static constexpr bool HasBoundArguments = false;
            return HTNAtomList({true, HasBoundArguments});
        }

        // Continue
        Environment.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    return HTNAtomList({false});
}

HTNAtom HTNInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const HTNNodeScope NotConditionNodeScope = HTNNodeScope(mDecompositionContext, inNotConditionNode.GetID());

    // NO bindings
    // NO backtracking

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const std::unordered_map<std::string, HTNAtom> Variables = Environment.GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext.GetDecompositionHistory();

    // Check condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = inNotConditionNode.GetSubConditionNode();
    const HTNAtomList                                  ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<HTNAtomList>();

    // Reset decomposition history
    mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    // TODO salvarez Make sure that this works
    Environment.SetVariables(Variables);

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
    const std::string  CompoundTaskNodeID    = inCompoundTaskNode.GetID();
    const HTNNodeScope CompoundTaskNodeScope = HTNNodeScope(mDecompositionContext, CompoundTaskNodeID);

    // The ID of a compound task should match the ID of a method
    const std::shared_ptr<const HTNMethodNode> MethodNode = mDomainNode->FindMethodNodeByID(CompoundTaskNodeID);
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

    const std::string      MethodNodeID        = MethodNode->GetID();
    const HTNVariableScope MethodVariableScope = HTNVariableScope(mDecompositionContext, MethodNodeID);

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
    const HTNNodeScope PrimitiveTaskNodeScope = HTNNodeScope(mDecompositionContext, inPrimitiveTaskNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNValueNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    const std::string                          ID     = GetNodeValue(*IDNode).GetValue<std::string>();

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    std::vector<HTNAtom>                                        Arguments;
    Arguments.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = GetNodeValue(*ArgumentNode);
        Arguments.emplace_back(Argument);
    }

    const HTNTaskResult TaskResult = HTNTaskResult(ID, Arguments);
    CurrentDecomposition.AddTaskResultToPlan(TaskResult);

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNValueNode& inValueNode)
{
    const HTNNodeScope ValueNodeScope = HTNNodeScope(mDecompositionContext, inValueNode.GetID());

    return inValueNode.GetValue();
}

void HTNInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveVariableIDPrefixes(VariableID);
    const std::string CurrentVariablePath = mDecompositionContext.MakeCurrentVariablePath(VariableID);
    Environment.SetVariable(CurrentVariablePath, inVariableValueNodeValue);
}

HTNAtom HTNInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveVariableIDPrefixes(VariableID);
    const std::string CurrentVariablePath = mDecompositionContext.MakeCurrentVariablePath(VariableID);
    return Environment.GetOrAddVariable(CurrentVariablePath);
}

HTNAtom HTNInterpreter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNNodeScope ConstantValueNodeScope = HTNNodeScope(mDecompositionContext, inConstantValueNode.GetID());

    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtomList({false});
    }

    return GetNodeValue(*ConstantNode);
}
