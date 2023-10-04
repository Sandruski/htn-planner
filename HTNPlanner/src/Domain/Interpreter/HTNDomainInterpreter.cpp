#include "Domain/Interpreter/HTNDomainInterpreter.h"

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
#include "Domain/Interpreter/HTNConditionQuery.h"
#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNNodeScope.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNVariableScope.h"
#include "WorldState/HTNWorldState.h"

namespace
{
bool RemovePrefix(const std::string& inPrefix, std::string& ioString)
{
    const std::size_t Position = ioString.find(inPrefix);
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
} // namespace

HTNDomainInterpreter::HTNDomainInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointName,
                               const HTNWorldState& inWorldState)
    : mDomainNode(inDomainNode), mEntryPointName(inEntryPointName), mDecompositionContext(inWorldState)
{
}

HTNDomainInterpreter::~HTNDomainInterpreter() = default;

bool HTNDomainInterpreter::Interpret(std::vector<HTNTaskResult>& outPlan)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    if (!DomainNode->IsTopLevel())
    {
        LOG_ERROR("Domain node [{}] is not top-level", DomainNode->GetID());
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

HTNAtom HTNDomainInterpreter::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string      DomainNodeID        = inDomainNode.GetID();
    const HTNNodeScope     DomainNodeScope     = HTNNodeScope(mDecompositionContext, DomainNodeID);
    const HTNVariableScope DomainVariableScope = HTNVariableScope(mDecompositionContext, DomainNodeID);

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root task node
    static constexpr bool                            IsIdentifier         = true;
    static constexpr bool                            IsTopLevel           = true;
    const std::shared_ptr<const HTNCompoundTaskNode> RootCompoundTaskNode = std::make_shared<HTNCompoundTaskNode>(
        std::make_shared<const HTNValueNode>(mEntryPointName, IsIdentifier), std::vector<std::shared_ptr<const HTNValueNodeBase>>(), IsTopLevel);
    const std::string&    CurrentVariableScopePath = mDecompositionContext.GetCurrentVariableScopePath();
    const HTNTaskInstance RootCompoundTaskInstance = HTNTaskInstance(RootCompoundTaskNode, HTNEnvironment(), CurrentVariableScopePath);
    CurrentDecomposition.PushPendingTaskInstance(RootCompoundTaskInstance);

    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance TaskInstance = CurrentDecomposition.PopPendingTaskInstance();

        // Initialize the state with that of the task instance
        CurrentDecomposition.SetEnvironment(TaskInstance.GetEnvironment());
        mDecompositionContext.SetCurrentVariableScopePath(TaskInstance.GetVariableScopePath());

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstance.GetTaskNode();
        if (!GetNodeValue(*TaskNode).GetValue<bool>())
        {
            return false;
        }
    }

    return true;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantNode& inConstantNode)
{
    const HTNNodeScope ConstantNodeScope = HTNNodeScope(mDecompositionContext, inConstantNode.GetID());

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNNodeScope AxiomNodeScope = HTNNodeScope(mDecompositionContext, inAxiomNode.GetID());

    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        return true;
    }

    // Check condition
    return GetNodeValue(*ConditionNode);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodNodeScope = HTNNodeScope(mDecompositionContext, inMethodNode.GetID());
    const std::string& CurrentNodePath = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes     = inMethodNode.GetBranchNodes();
    const std::size_t                                        BranchNodesSize = BranchNodes.size();
    for (std::size_t BranchIndex = Environment.AddIndex(CurrentNodePath); BranchIndex >= 0 && BranchIndex < BranchNodesSize;
         BranchIndex             = Environment.GetIndex(CurrentNodePath))
    {
        // Check branch
        const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
        if (GetNodeValue(*BranchNode).GetValue<bool>())
        {
            const std::string& CurrentVariableScopePath = mDecompositionContext.GetCurrentVariableScopePath();

            const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = BranchNode->GetTaskNodes();
            for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
            {
                const std::shared_ptr<const HTNTaskNodeBase>& TaskNode     = TaskNodes[i];
                const HTNTaskInstance                         TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentVariableScopePath);
                CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
            }

            return true;
        }

        if (BranchIndex == BranchNodesSize - 1) // Last branch
        {
            // Restore state: unbound variables but updated indices
            mDecompositionContext.RestoreDecomposition();
        }
        else
        {
            // Continue
            Environment.IncrementIndex(CurrentNodePath);
        }
    }

    return false;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    const HTNNodeScope BranchNodeScope = HTNNodeScope(mDecompositionContext, inBranchNode.GetID());

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (!CurrentPreConditionNode)
    {
        return true;
    }

    return GetNodeValue(*CurrentPreConditionNode).GetValue<bool>();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConditionNode& inConditionNode)
{
    const HTNNodeScope ConditionNodeScope = HTNNodeScope(mDecompositionContext, inConditionNode.GetID());
    const std::string& CurrentNodePath    = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    const HTNWorldState*    WorldState           = mDecompositionContext.GetWorldState();

    // Gather fact arguments
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<HTNAtom>                                        FactArguments;
    FactArguments.reserve(ArgumentNodes.size());
    bool ShouldBindFactArguments = false;
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument  = GetNodeValue(*ArgumentNode);
        ShouldBindFactArguments = ShouldBindFactArguments || !Argument.IsSet();
        FactArguments.emplace_back(Argument);
    }

    // Check fact
    const std::shared_ptr<const HTNValueNode>& ConditionNodeIDNode = inConditionNode.GetIDNode();
    const std::string                          FactID              = GetNodeValue(*ConditionNodeIDNode).GetValue<std::string>();
    const std::size_t                          FactArgumentsSize   = WorldState->GetFactArgumentsSize(FactID, FactArguments.size());
    for (std::size_t FactArgumentsIndex = Environment.AddOrIncrementIndex(CurrentNodePath); FactArgumentsIndex < FactArgumentsSize;
         FactArgumentsIndex             = Environment.AddOrIncrementIndex(CurrentNodePath))
    {
        if (!HTNConditionQueryWorldState::Check(*WorldState, FactID, FactArgumentsIndex, FactArguments))
        {
            continue;
        }

        if (!ShouldBindFactArguments)
        {
            return true;
        }

        // Copy variables before they change
        const std::unordered_map<std::string, HTNAtom> Variables = Environment.GetVariables();

        // Update variables
        for (std::size_t i = 0; i < ArgumentNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = ArgumentNodes[i];
            const HTNAtom                                  FactArgument = FactArguments[i];
            SetNodeValue(*ArgumentNode, FactArgument);
        }

        const bool IsLastFactEntry = (FactArgumentsIndex == FactArgumentsSize - 1);
        if (IsLastFactEntry)
        {
            return true;
        }

        // Copy unchanged variables to decomposition
        HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
        HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
        NewEnvironment.SetVariables(Variables);

        // Record decomposition
        mDecompositionContext.RecordDecomposition(NewDecomposition);

        return true;
    }

    return false;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const HTNNodeScope AxiomConditionNodeScope = HTNNodeScope(mDecompositionContext, inAxiomConditionNode.GetID());

    const std::shared_ptr<const HTNValueNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                          AxiomNodeID              = GetNodeValue(*AxiomConditionNodeIDNode).GetValue<std::string>();
    const std::shared_ptr<const HTNAxiomNode>  AxiomNode                = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not found", AxiomNodeID);
        return false;
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
        for (std::size_t i = 0; i < AxiomNodeArgumentNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNValueNodeBase>& AxiomNodeArgumentNode      = AxiomNodeArgumentNodes[i];
            const HTNAtom&                                 AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
            SetNodeValue(*AxiomNodeArgumentNode, AxiomConditionNodeArgument);
        }

        if (!GetNodeValue(*AxiomNode).GetValue<bool>())
        {
            return false;
        }

        AxiomNodeArguments.reserve(AxiomNodeArgumentNodes.size());
        for (const std::shared_ptr<const HTNValueNodeBase>& AxiomNodeArgumentNode : AxiomNodeArgumentNodes)
        {
            const HTNAtom AxiomNodeArgument = GetNodeValue(*AxiomNodeArgumentNode);
            AxiomNodeArguments.emplace_back(AxiomNodeArgument);
        }
    }

    // Initialize the arguments of the condition node with the output arguments of the axiom node
    for (std::size_t i = 0; i < AxiomConditionNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueNodeBase>& AxiomConditionNodeArgumentNode = AxiomConditionNodeArgumentNodes[i];
        const HTNAtom&                                 AxiomNodeArgument              = AxiomNodeArguments[i];
        SetNodeValue(*AxiomConditionNodeArgumentNode, AxiomNodeArgument);
    }

    return true;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const HTNNodeScope AndConditionNodeScope = HTNNodeScope(mDecompositionContext, inAndConditionNode.GetID());
    const std::string& CurrentNodePath       = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAndConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Environment.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        if (!GetNodeValue(*SubConditionNode).GetValue<bool>())
        {
            if (SubConditionIndex == 0) // First sub-condition
            {
                return false;
            }
            else
            {
                // Restore state: unbound variables but updated indices
                mDecompositionContext.RestoreDecomposition();
                continue;
            }
        }

        // Continue
        Environment.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to true
    return true;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const HTNNodeScope OrConditionNodeScope = HTNNodeScope(mDecompositionContext, inOrConditionNode.GetID());
    const std::string& CurrentNodePath      = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inOrConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Environment.GetIndex(CurrentNodePath))
    {
        // Copy decomposition history
        const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext.GetDecompositionHistory();

        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode      = SubConditionNodes[SubConditionIndex];
        const bool                                         SubConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<bool>();

        // Reset decomposition history
        mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

        if (SubConditionNodeValue)
        {
            return true;
        }

        // Continue
        Environment.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    return false;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const HTNNodeScope AltConditionNodeScope = HTNNodeScope(mDecompositionContext, inAltConditionNode.GetID());
    const std::string& CurrentNodePath       = mDecompositionContext.GetCurrentNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAltConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Environment.AddIndex(CurrentNodePath); SubConditionIndex >= 0 && SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Environment.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        if (GetNodeValue(*SubConditionNode).GetValue<bool>())
        {
            return true;
        }

        if (SubConditionIndex == SubConditionNodesSize - 1) // Last sub-condition
        {
            // Restore state: unbound variables but updated indices
            mDecompositionContext.RestoreDecomposition();
        }
        else
        {
            // Continue
            Environment.IncrementIndex(CurrentNodePath);
        }
    }

    // If empty, it evaluates to false
    return false;
}

// NO bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const HTNNodeScope NotConditionNodeScope = HTNNodeScope(mDecompositionContext, inNotConditionNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const std::unordered_map<std::string, HTNAtom> Variables = Environment.GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext.GetDecompositionHistory();

    // Check sub-condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode   = inNotConditionNode.GetSubConditionNode();
    const bool                                         ConditionNodeValue = GetNodeValue(*SubConditionNode).GetValue<bool>();

    // Reset variables
    Environment.SetVariables(Variables);

    // Reset decomposition history
    mDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Invert result
    return !ConditionNodeValue;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
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

    if (inCompoundTaskNode.IsTopLevel() && !MethodNode->IsTopLevel())
    {
        LOG_ERROR("Method node [{}] is not top-level", CompoundTaskNodeID);
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
    for (std::size_t i = 0; i < MethodNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueNodeBase>& MethodNodeArgumentNode   = MethodNodeArgumentNodes[i];
        const HTNAtom&                                 CompoundTaskNodeArgument = CompoundTaskNodeArguments[i];
        SetNodeValue(*MethodNodeArgumentNode, CompoundTaskNodeArgument);
    }

    return GetNodeValue(*MethodNode);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
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

HTNAtom HTNDomainInterpreter::Visit(const HTNValueNode& inValueNode)
{
    const HTNNodeScope ValueNodeScope = HTNNodeScope(mDecompositionContext, inValueNode.GetID());

    return inValueNode.GetValue();
}

void HTNDomainInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveVariableIDPrefixes(VariableID);
    const std::string CurrentVariablePath = mDecompositionContext.MakeCurrentVariablePath(VariableID);
    Environment.SetVariable(CurrentVariablePath, inVariableValueNodeValue);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    const HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironment();

    std::string VariableID = inVariableValueNode.GetValue().GetValue<std::string>();
    RemoveVariableIDPrefixes(VariableID);
    const std::string CurrentVariablePath = mDecompositionContext.MakeCurrentVariablePath(VariableID);
    return Environment.GetVariable(CurrentVariablePath);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNNodeScope ConstantValueNodeScope = HTNNodeScope(mDecompositionContext, inConstantValueNode.GetID());

    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return false;
    }

    return GetNodeValue(*ConstantNode);
}
