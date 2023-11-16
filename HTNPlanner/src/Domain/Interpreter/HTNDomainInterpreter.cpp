#include "Domain/Interpreter/HTNDomainInterpreter.h"

#include "Domain/Interpreter/HTNConditionQuery.h"
#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionNodeScope.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
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
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "WorldState/HTNWorldState.h"

namespace
{
HTNDecompositionContext& GetDecompositionContext(HTNNodeVisitorContextBase& ioContext)
{
    return static_cast<HTNDecompositionContext&>(ioContext);
}

#ifdef HTN_DEBUG
void RecordCurrentNodeSnapshot(const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint,
                               HTNDecompositionContext& ioDecompositionContext)
{
    const std::string& CurrentNodePath = ioDecompositionContext.GetCurrentNodePath().GetNodePath();
    ioDecompositionContext.RecordNodeSnapshot(CurrentNodePath, inResult, inNodeStep, inIsChoicePoint);
}

void RecordCurrentNodeSnapshot(const HTNNodeStep inNodeStep, const bool inIsChoicePoint, HTNDecompositionContext& ioDecompositionContext)
{
    const std::string& CurrentNodePath = ioDecompositionContext.GetCurrentNodePath().GetNodePath();
    ioDecompositionContext.RecordNodeSnapshot(CurrentNodePath, inNodeStep, inIsChoicePoint);
}
#endif
} // namespace

bool HTNDomainInterpreter::Interpret(HTNDecompositionContext& ioDecompositionContext) const
{
    OPTICK_EVENT("InterpretDomain");

    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDecompositionContext.GetDomainNode();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode, ioDecompositionContext).GetValue<bool>();
}

/*
 * Depth first search on a graph
 * Domain is a graph that may contain cycles because of backtracking
 */
HTNAtom HTNDomainInterpreter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetDomainNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const std::string                            DomainNodeID    = inDomainNode.GetID();
    const HTNDecompositionNodeScope              DomainNodeScope = HTNDecompositionNodeScope(DomainNodeID, DecompositionContext);
    const HTNDecompositionVariableScopeNodeScope DomainVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(DomainNodeID, DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    if (!inDomainNode.IsTopLevel())
    {
        LOG_ERROR("Domain node [{}] is not top-level", inDomainNode.GetID());
        return false;
    }

    // Top-level compound task node
    const std::string& EntryPointID = DecompositionContext.GetEntryPointID();
    if (EntryPointID.empty())
    {
        LOG_ERROR("Entry point ID is empty");
        return false;
    }

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    const HTNNodePath&                               CurrentNodePath          = DecompositionContext.GetCurrentNodePath();
    const HTNNodePath&                               CurrentVariableScopeNodePath = DecompositionContext.GetCurrentVariableScopeNodePath();
    const HTNTaskInstance                            TopLevelCompoundTaskInstance =
        HTNTaskInstance(TopLevelCompoundTaskNode, HTNEnvironment(), CurrentNodePath, CurrentVariableScopeNodePath);
    CurrentDecomposition.PushPendingTaskInstance(TopLevelCompoundTaskInstance);

    // Perform depth-first search on the domain
    // The domain is a graph
    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance TaskInstance = CurrentDecomposition.PopPendingTaskInstance();

        // Initialize the state with that of the task instance
        DecompositionContext.SetCurrentNodePath(TaskInstance.GetNodePath());
        DecompositionContext.SetCurrentVariableScopeNodePath(TaskInstance.GetVariableScopeNodePath());

        // Also set the environment because it is synced to the current node and variable scope paths
        CurrentDecomposition.SetEnvironment(TaskInstance.GetEnvironment());

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstance.GetTaskNode();
        const bool                                    Result   = GetNodeValue(*TaskNode, ioContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetConstantNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope ConstantNodeScope = HTNDecompositionNodeScope(inConstantNode.GetID(), DecompositionContext);

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode, ioContext);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetAxiomNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AxiomNodeScope = HTNDecompositionNodeScope(inAxiomNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        constexpr bool Result = true;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    // Check condition
    const bool Result = GetNodeValue(*ConditionNode, ioContext).GetValue<bool>();
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetMethodNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope MethodNodeScope = HTNDecompositionNodeScope(inMethodNode.GetID(), DecompositionContext);
    const std::string&              CurrentNodePath = DecompositionContext.GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes     = inMethodNode.GetBranchNodes();
    const size                                        BranchNodesSize = BranchNodes.size();
    for (size BranchIndex = Indices.AddIndex(CurrentNodePath); BranchIndex < BranchNodesSize; BranchIndex = Indices.GetIndex(CurrentNodePath))
    {
        // Check branch
        const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
        const bool                                  Result     = GetNodeValue(*BranchNode, ioContext).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        if (BranchIndex == BranchNodesSize - 1) // Last branch
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            // Restore state: unbound variables but updated indices
            if (!DecompositionContext.RestoreDecomposition())
            {
                break;
            }
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
        }
        else
        {
            // Continue
            Indices.IncrementIndex(CurrentNodePath);
        }
    }

    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetBranchNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope BranchNodeScope = HTNDecompositionNodeScope(inBranchNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (CurrentPreConditionNode)
    {
        const bool Result = GetNodeValue(*CurrentPreConditionNode, ioContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const HTNNodePath& CurrentNodePath              = DecompositionContext.GetCurrentNodePath();
    const HTNNodePath& CurrentVariableScopeNodePath = DecompositionContext.GetCurrentVariableScopeNodePath();

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (int32 i = static_cast<int32>(TaskNodes.size()) - 1; i >= 0; --i)
    {
        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
        const HTNTaskInstance TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentNodePath, CurrentVariableScopeNodePath);
        CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
#endif

    const HTNDecompositionNodeScope ConditionNodeScope = HTNDecompositionNodeScope(inConditionNode.GetID(), DecompositionContext);
    const std::string&              CurrentNodePath    = DecompositionContext.GetCurrentNodePath().GetNodePath();

    // Gather fact arguments
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<HTNAtom>                                                  FactArguments;
    FactArguments.reserve(ArgumentNodes.size());
    bool ShouldBindFactArguments = false;
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument  = GetNodeValue(*ArgumentNode, ioContext);
        ShouldBindFactArguments = ShouldBindFactArguments || !Argument.IsSet();
        FactArguments.emplace_back(Argument);
    }

#ifdef HTN_DEBUG
    const bool IsChoicePoint = ShouldBindFactArguments;
#endif

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const HTNWorldState& WorldState = DecompositionContext.GetWorldState();

    // Check fact
    const std::shared_ptr<const HTNIdentifierExpressionNode>& ConditionNodeIDNode = inConditionNode.GetIDNode();
    const std::string                                         FactID = GetNodeValue(*ConditionNodeIDNode, ioContext).GetValue<std::string>();
    const size                                         FactArgumentsSize = WorldState.GetFactArgumentsSize(FactID, FactArguments.size());
    for (size FactArgumentsIndex = Indices.AddOrIncrementIndex(CurrentNodePath); FactArgumentsIndex < FactArgumentsSize;
         FactArgumentsIndex             = Indices.AddOrIncrementIndex(CurrentNodePath))
    {
        if (!HTNConditionQueryWorldState::Check(WorldState, FactID, FactArgumentsIndex, FactArguments))
        {
            continue;
        }

        // TODO salvarez Detect whether it is a choice point in the domain validator and store the data in the condition node
        if (!ShouldBindFactArguments)
        {
            constexpr bool Result = true;
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Copy variables before they change
        const HTNVariables Variables = Environment.GetVariables();

        // Update variables
        for (size i = 0; i < ArgumentNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode = ArgumentNodes[i];
            const HTNAtom                                            FactArgument = FactArguments[i];
            SetNodeValue(*ArgumentNode, FactArgument, ioContext);
        }

        const bool IsLastFactEntry = (FactArgumentsIndex == FactArgumentsSize - 1);
        if (IsLastFactEntry)
        {
            constexpr bool Result = true;
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Copy unchanged variables to decomposition
        HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
        HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
        NewEnvironment.SetVariables(Variables);

        // Record decomposition
        DecompositionContext.RecordDecomposition(NewDecomposition);

        constexpr bool Result = true;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetAxiomConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AxiomConditionNodeScope = HTNDecompositionNodeScope(inAxiomConditionNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNDomainNode>&               DomainNode               = DecompositionContext.GetDomainNode();
    const std::shared_ptr<const HTNIdentifierExpressionNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                         AxiomNodeID = GetNodeValue(*AxiomConditionNodeIDNode, ioContext).GetValue<std::string>();
    const std::shared_ptr<const HTNAxiomNode> AxiomNode   = DomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not be found", AxiomNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    std::vector<HTNAtom>                                                  AxiomConditionNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    AxiomConditionNodeArguments.reserve(AxiomConditionNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        const HTNAtom AxiomConditionNodeArgument = GetNodeValue(*AxiomConditionNodeArgumentNode, ioContext);
        AxiomConditionNodeArguments.emplace_back(AxiomConditionNodeArgument);
    }

    std::vector<HTNAtom> AxiomNodeParameters;
    {
        const HTNDecompositionVariableScopeNodeScope AxiomVariableScopeNodeScope =
            HTNDecompositionVariableScopeNodeScope(AxiomNodeID, DecompositionContext);

        // Initialize the input parameters of the axiom node with the arguments of the condition node
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& AxiomNodeParameterNodes = AxiomNode->GetParameterNodes();
        for (size i = 0; i < AxiomNodeParameterNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNVariableExpressionNode>& AxiomNodeParameterNode     = AxiomNodeParameterNodes[i];
            const HTNAtom&                                          AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
            SetNodeValue(*AxiomNodeParameterNode, AxiomConditionNodeArgument, ioContext);
        }

        const bool Result = GetNodeValue(*AxiomNode, ioContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        AxiomNodeParameters.reserve(AxiomNodeParameterNodes.size());
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomNodeParameterNode : AxiomNodeParameterNodes)
        {
            const HTNAtom AxiomNodeParameter = GetNodeValue(*AxiomNodeParameterNode, ioContext);
            AxiomNodeParameters.emplace_back(AxiomNodeParameter);
        }
    }

    // Initialize the arguments of the condition node with the output parameters of the axiom node
    for (size i = 0; i < AxiomConditionNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode = AxiomConditionNodeArgumentNodes[i];
        const HTNAtom&                                           AxiomNodeArgument              = AxiomNodeParameters[i];
        SetNodeValue(*AxiomConditionNodeArgumentNode, AxiomNodeArgument, ioContext);
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetAndConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AndConditionNodeScope = HTNDecompositionNodeScope(inAndConditionNode.GetID(), DecompositionContext);
    const std::string&              CurrentNodePath       = DecompositionContext.GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAndConditionNode.GetSubConditionNodes();
    const size                                               SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode, ioContext).GetValue<bool>();
        if (!Result)
        {
            if (SubConditionIndex == 0) // First sub-condition
            {
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                return Result;
            }
            else
            {
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                // Restore state: unbound variables but updated indices
                if (!DecompositionContext.RestoreDecomposition())
                {
                    break;
                }
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
                continue;
            }
        }

        // Continue
        Indices.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to true
    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetOrConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope OrConditionNodeScope = HTNDecompositionNodeScope(inOrConditionNode.GetID(), DecompositionContext);
    const std::string&              CurrentNodePath      = DecompositionContext.GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inOrConditionNode.GetSubConditionNodes();
    const size                                               SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Copy decomposition history
        const std::vector<HTNDecompositionRecord> DecompositionHistory = DecompositionContext.GetDecompositionHistory();

        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode, ioContext).GetValue<bool>();

        // Reset decomposition history
        DecompositionContext.SetDecompositionHistory(DecompositionHistory);

        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Continue
        Indices.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetAltConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AltConditionNodeScope = HTNDecompositionNodeScope(inAltConditionNode.GetID(), DecompositionContext);
    const std::string&              CurrentNodePath       = DecompositionContext.GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAltConditionNode.GetSubConditionNodes();
    const size                                               SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode, ioContext).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        if (SubConditionIndex == SubConditionNodesSize - 1) // Last sub-condition
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            // Restore state: unbound variables but updated indices
            if (!DecompositionContext.RestoreDecomposition())
            {
                break;
            }
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
        }
        else
        {
            // Continue
            Indices.IncrementIndex(CurrentNodePath);
        }
    }

    // If empty, it evaluates to false
    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// NO bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetNotConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope NotConditionNodeScope = HTNDecompositionNodeScope(inNotConditionNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const HTNVariables Variables = Environment.GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = DecompositionContext.GetDecompositionHistory();

    // Check sub-condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    const bool                                         Result           = !GetNodeValue(*SubConditionNode, ioContext).GetValue<bool>();

    // Reset variables
    Environment.SetVariables(Variables);

    // Reset decomposition history
    DecompositionContext.SetDecompositionHistory(DecompositionHistory);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetCompoundTaskNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope CompoundTaskNodeScope = HTNDecompositionNodeScope(inCompoundTaskNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNDomainNode>&               DomainNode             = DecompositionContext.GetDomainNode();
    const std::shared_ptr<const HTNIdentifierExpressionNode>& CompoundTaskNodeIDNode = inCompoundTaskNode.GetIDNode();
    const std::string                                         MethodNodeID = GetNodeValue(*CompoundTaskNodeIDNode, ioContext).GetValue<std::string>();
    const std::shared_ptr<const HTNMethodNode>                MethodNode   = DomainNode->FindMethodNodeByID(MethodNodeID);
    if (!MethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", MethodNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    if (inCompoundTaskNode.IsTopLevel() && !MethodNode->IsTopLevel())
    {
        LOG_ERROR("Method node [{}] is not top-level", MethodNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    std::vector<HTNAtom>                                                  CompoundTaskNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    CompoundTaskNodeArguments.reserve(CompoundTaskNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        const HTNAtom CompoundTaskNodeArgument = GetNodeValue(*CompoundTaskNodeArgumentNode, ioContext);
        CompoundTaskNodeArguments.emplace_back(CompoundTaskNodeArgument);
    }

    const HTNDecompositionVariableScopeNodeScope MethodVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(MethodNodeID, DecompositionContext);

    // Initialize the input parameters of the method node with the arguments of the compound task node
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& MethodNodeParameterNodes = MethodNode->GetParameterNodes();
    for (size i = 0; i < MethodNodeParameterNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNVariableExpressionNode>& MethodNodeParameterNode  = MethodNodeParameterNodes[i];
        const HTNAtom&                                          CompoundTaskNodeArgument = CompoundTaskNodeArguments[i];
        SetNodeValue(*MethodNodeParameterNode, CompoundTaskNodeArgument, ioContext);
    }

    const bool Result = GetNodeValue(*MethodNode, ioContext).GetValue<bool>();
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetPrimitiveTaskNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope PrimitiveTaskNodeScope = HTNDecompositionNodeScope(inPrimitiveTaskNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    const std::string                                         ID     = GetNodeValue(*IDNode, ioContext).GetValue<std::string>();

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    std::vector<HTNAtom>                                                  Arguments;
    Arguments.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = GetNodeValue(*ArgumentNode, ioContext);
        Arguments.emplace_back(Argument);
    }

    const HTNTaskResult TaskResult = HTNTaskResult(ID, Arguments);
    CurrentDecomposition.AddTaskResultToPlan(TaskResult);

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetIdentifierExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope IdentifierExpressionNodeScope =
        HTNDecompositionNodeScope(inIdentifierExpressionNode.GetID(), DecompositionContext);

    return inIdentifierExpressionNode.GetValue();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetLiteralExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope LiteralExpressionNodeScope = HTNDecompositionNodeScope(inLiteralExpressionNode.GetID(), DecompositionContext);

    return inLiteralExpressionNode.GetValue();
}

void HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                                 HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("SetVariableExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope VariableExpressionNodeScope = HTNDecompositionNodeScope(inVariableExpressionNode.GetID(), DecompositionContext);

    const std::string  VariableID                   = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = DecompositionContext.GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return;
    }

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNVariables&           Variables            = Environment.GetVariablesMutable();
    Variables.SetVariable(CurrentVariablePath, inVariableExpressionNodeValue);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetVariableExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope VariableExpressionNodeScope = HTNDecompositionNodeScope(inVariableExpressionNode.GetID(), DecompositionContext);

    const std::string  VariableID                   = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = DecompositionContext.GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return HTNAtom();
    }

    const HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecomposition();
    const HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironment();
    const HTNVariables&           Variables            = Environment.GetVariables();
    return Variables.FindVariable(CurrentVariablePath);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    OPTICK_EVENT("GetConstantExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioContext);

    const HTNDecompositionNodeScope ConstantExpressionNodeScope = HTNDecompositionNodeScope(inConstantExpressionNode.GetID(), DecompositionContext);

    const std::shared_ptr<const HTNDomainNode>& DomainNode     = DecompositionContext.GetDomainNode();
    const std::string&                          ConstantNodeID = inConstantExpressionNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode>      ConstantNode   = DomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtom();
    }

    return GetNodeValue(*ConstantNode, ioContext);
}
