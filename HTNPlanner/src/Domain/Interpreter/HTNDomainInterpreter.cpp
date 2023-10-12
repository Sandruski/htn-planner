#include "Domain/Interpreter/HTNDomainInterpreter.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNConditionQuery.h"
#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNDecompositionNodeScope.h"
#include "Domain/Interpreter/HTNDecompositionVariableScopeNodeScope.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "WorldState/HTNWorldState.h"

bool HTNDomainInterpreter::Interpret()
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

    return GetNodeValue(*DomainNode).GetValue<bool>();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string                            DomainNodeID    = inDomainNode.GetID();
    const HTNDecompositionNodeScope              DomainNodeScope = HTNDecompositionNodeScope(mDecompositionContext, DomainNodeID);
    const HTNDecompositionVariableScopeNodeScope DomainVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(mDecompositionContext, DomainNodeID);

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    const HTNNodePath&                               CurrentNodePath              = mDecompositionContext.GetCurrentNodePath();
    const HTNNodePath&                               CurrentVariableScopeNodePath = mDecompositionContext.GetCurrentVariableScopeNodePath();
    const HTNTaskInstance                            TopLevelCompoundTaskInstance =
        HTNTaskInstance(TopLevelCompoundTaskNode, HTNEnvironment(), CurrentNodePath, CurrentVariableScopeNodePath);
    CurrentDecomposition.PushPendingTaskInstance(TopLevelCompoundTaskInstance);

    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance TaskInstance = CurrentDecomposition.PopPendingTaskInstance();

        // Initialize the state with that of the task instance
        mDecompositionContext.SetCurrentNodePath(TaskInstance.GetNodePath());
        mDecompositionContext.SetCurrentVariableScopeNodePath(TaskInstance.GetVariableScopeNodePath());

        // Also set the environment because it is synced to the current node and variable scope paths
        CurrentDecomposition.SetEnvironment(TaskInstance.GetEnvironment());

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
    const HTNDecompositionNodeScope ConstantNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inConstantNode.GetID());

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNDecompositionNodeScope AxiomNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inAxiomNode.GetID());

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
    const HTNDecompositionNodeScope MethodNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inMethodNode.GetID());
    const std::string&              CurrentNodePath = mDecompositionContext.GetCurrentNodePath().GetNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes     = inMethodNode.GetBranchNodes();
    const std::size_t                                        BranchNodesSize = BranchNodes.size();
    for (std::size_t BranchIndex = Indices.AddIndex(CurrentNodePath); BranchIndex < BranchNodesSize; BranchIndex = Indices.GetIndex(CurrentNodePath))
    {
        // Check branch
        const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
        if (GetNodeValue(*BranchNode).GetValue<bool>())
        {
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
            Indices.IncrementIndex(CurrentNodePath);
        }
    }

    return false;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    const HTNDecompositionNodeScope BranchNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inBranchNode.GetID());

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (CurrentPreConditionNode)
    {
        if (!GetNodeValue(*CurrentPreConditionNode).GetValue<bool>())
        {
            return false;
        }
    }

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const HTNNodePath& CurrentNodePath          = mDecompositionContext.GetCurrentNodePath();
    const HTNNodePath& CurrentVariableScopeNodePath = mDecompositionContext.GetCurrentVariableScopeNodePath();

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
    {
        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
        const HTNTaskInstance TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentNodePath, CurrentVariableScopeNodePath);
        CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
    }

    return true;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConditionNode& inConditionNode)
{
    const HTNDecompositionNodeScope ConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inConditionNode.GetID());
    const std::string&              CurrentNodePath    = mDecompositionContext.GetCurrentNodePath().GetNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const HTNWorldState* WorldState = mDecompositionContext.GetWorldState();

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
    for (std::size_t FactArgumentsIndex = Indices.AddOrIncrementIndex(CurrentNodePath); FactArgumentsIndex < FactArgumentsSize;
         FactArgumentsIndex             = Indices.AddOrIncrementIndex(CurrentNodePath))
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
        const HTNVariables Variables = Environment.GetVariables();

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
    const HTNDecompositionNodeScope AxiomConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inAxiomConditionNode.GetID());

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
        const HTNDecompositionVariableScopeNodeScope AxiomVariableScopeNodeScope =
            HTNDecompositionVariableScopeNodeScope(mDecompositionContext, AxiomNodeID);

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
    const HTNDecompositionNodeScope AndConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inAndConditionNode.GetID());
    const std::string&              CurrentNodePath       = mDecompositionContext.GetCurrentNodePath().GetNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAndConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
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
        Indices.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to true
    return true;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const HTNDecompositionNodeScope OrConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inOrConditionNode.GetID());
    const std::string&              CurrentNodePath      = mDecompositionContext.GetCurrentNodePath().GetNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inOrConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
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
        Indices.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    return false;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const HTNDecompositionNodeScope AltConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inAltConditionNode.GetID());
    const std::string&              CurrentNodePath       = mDecompositionContext.GetCurrentNodePath().GetNodePath();

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAltConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
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
            Indices.IncrementIndex(CurrentNodePath);
        }
    }

    // If empty, it evaluates to false
    return false;
}

// NO bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const HTNDecompositionNodeScope NotConditionNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inNotConditionNode.GetID());

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const HTNVariables Variables = Environment.GetVariables();

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
    const HTNDecompositionNodeScope CompoundTaskNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inCompoundTaskNode.GetID());

    const std::shared_ptr<const HTNValueNode>& CompoundTaskNodeIDNode = inCompoundTaskNode.GetIDNode();
    const std::string                          MethodNodeID           = GetNodeValue(*CompoundTaskNodeIDNode).GetValue<std::string>();
    const std::shared_ptr<const HTNMethodNode> MethodNode             = mDomainNode->FindMethodNodeByID(MethodNodeID);
    if (!MethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", MethodNodeID);
        return false;
    }

    if (inCompoundTaskNode.IsTopLevel() && !MethodNode->IsTopLevel())
    {
        LOG_ERROR("Method node [{}] is not top-level", MethodNodeID);
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

    const HTNDecompositionVariableScopeNodeScope MethodVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(mDecompositionContext, MethodNodeID);

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
    const HTNDecompositionNodeScope PrimitiveTaskNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inPrimitiveTaskNode.GetID());

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
    const HTNDecompositionNodeScope ValueNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inValueNode.GetID());

    return inValueNode.GetValue();
}

void HTNDomainInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue)
{
    const HTNDecompositionNodeScope VariableValueNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    const std::string  VariableID                   = inVariableValueNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = mDecompositionContext.GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return;
    }

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNVariables&           Variables            = Environment.GetVariablesMutable();
    Variables.SetVariable(CurrentVariablePath, inVariableValueNodeValue);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const HTNDecompositionNodeScope VariableValueNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    const std::string  VariableID                   = inVariableValueNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = mDecompositionContext.GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return HTNAtom();
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    const HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironment();
    const HTNVariables&           Variables            = Environment.GetVariables();
    return Variables.GetVariable(CurrentVariablePath);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNDecompositionNodeScope ConstantValueNodeScope = HTNDecompositionNodeScope(mDecompositionContext, inConstantValueNode.GetID());

    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtom();
    }

    return GetNodeValue(*ConstantNode);
}
