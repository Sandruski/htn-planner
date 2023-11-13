#include "Domain/Interpreter/HTNDomainInterpreter.h"

#include "Domain/Interpreter/HTNConditionQuery.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
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
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "WorldState/HTNWorldState.h"

bool HTNDomainInterpreter::Interpret(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     HTNDecompositionContext& ioDecompositionContext)
{
    OPTICK_EVENT("InterpretDomain");

    Reset(inDomainNode, inEntryPointID, ioDecompositionContext);

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

    if (mEntryPointID.empty())
    {
        LOG_ERROR("Entry point ID is empty");
        return false;
    }

    return GetNodeValue(*DomainNode).GetValue<bool>();
}

/*
 * Depth first search on a graph
 * Domain is a graph that may contain cycles because of backtracking
 */
HTNAtom HTNDomainInterpreter::Visit(const HTNDomainNode& inDomainNode)
{
    OPTICK_EVENT("GetDomainNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const std::string                            DomainNodeID    = inDomainNode.GetID();
    const HTNDecompositionNodeScope              DomainNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, DomainNodeID);
    const HTNDecompositionVariableScopeNodeScope DomainVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(*mDecompositionContext, DomainNodeID);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();

    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    const HTNNodePath&                               CurrentNodePath          = mDecompositionContext->GetCurrentNodePath();
    const HTNNodePath&                               CurrentVariableScopeNodePath = mDecompositionContext->GetCurrentVariableScopeNodePath();
    const HTNTaskInstance                            TopLevelCompoundTaskInstance =
        HTNTaskInstance(TopLevelCompoundTaskNode, HTNEnvironment(), CurrentNodePath, CurrentVariableScopeNodePath);
    CurrentDecomposition.PushPendingTaskInstance(TopLevelCompoundTaskInstance);

    // Perform depth-first search on the domain
    // The domain is a graph
    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance TaskInstance = CurrentDecomposition.PopPendingTaskInstance();

        // Initialize the state with that of the task instance
        mDecompositionContext->SetCurrentNodePath(TaskInstance.GetNodePath());
        mDecompositionContext->SetCurrentVariableScopeNodePath(TaskInstance.GetVariableScopeNodePath());

        // Also set the environment because it is synced to the current node and variable scope paths
        CurrentDecomposition.SetEnvironment(TaskInstance.GetEnvironment());

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstance.GetTaskNode();
        const bool                                    Result   = GetNodeValue(*TaskNode).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantNode& inConstantNode)
{
    OPTICK_EVENT("GetConstantNodeValue");

    const HTNDecompositionNodeScope ConstantNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inConstantNode.GetID());

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomNode& inAxiomNode)
{
    OPTICK_EVENT("GetAxiomNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AxiomNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inAxiomNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        constexpr bool Result = true;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
        return Result;
    }

    // Check condition
    const bool Result = GetNodeValue(*ConditionNode).GetValue<bool>();
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNMethodNode& inMethodNode)
{
    OPTICK_EVENT("GetMethodNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope MethodNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inMethodNode.GetID());
    const std::string&              CurrentNodePath = mDecompositionContext->GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes     = inMethodNode.GetBranchNodes();
    const std::size_t                                        BranchNodesSize = BranchNodes.size();
    for (std::size_t BranchIndex = Indices.AddIndex(CurrentNodePath); BranchIndex < BranchNodesSize; BranchIndex = Indices.GetIndex(CurrentNodePath))
    {
        // Check branch
        const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
        const bool                                  Result     = GetNodeValue(*BranchNode).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        if (BranchIndex == BranchNodesSize - 1) // Last branch
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            // Restore state: unbound variables but updated indices
            if (!mDecompositionContext->RestoreDecomposition())
            {
                break;
            }
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint);
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
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    OPTICK_EVENT("GetBranchNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope BranchNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inBranchNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (CurrentPreConditionNode)
    {
        const bool Result = GetNodeValue(*CurrentPreConditionNode).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }
    }

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const HTNNodePath& CurrentNodePath              = mDecompositionContext->GetCurrentNodePath();
    const HTNNodePath& CurrentVariableScopeNodePath = mDecompositionContext->GetCurrentVariableScopeNodePath();

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
    {
        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
        const HTNTaskInstance TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentNodePath, CurrentVariableScopeNodePath);
        CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConditionNode& inConditionNode)
{
    OPTICK_EVENT("GetConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
#endif

    const HTNDecompositionNodeScope ConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inConditionNode.GetID());
    const std::string&              CurrentNodePath    = mDecompositionContext->GetCurrentNodePath().GetNodePath();

    // Gather fact arguments
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<HTNAtom>                                                  FactArguments;
    FactArguments.reserve(ArgumentNodes.size());
    bool ShouldBindFactArguments = false;
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument  = GetNodeValue(*ArgumentNode);
        ShouldBindFactArguments = ShouldBindFactArguments || !Argument.IsSet();
        FactArguments.emplace_back(Argument);
    }

#ifdef HTN_DEBUG
    const bool IsChoicePoint = ShouldBindFactArguments;
#endif

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const HTNWorldState* WorldState = mDecompositionContext->GetWorldState();

    // Check fact
    const std::shared_ptr<const HTNIdentifierExpressionNode>& ConditionNodeIDNode = inConditionNode.GetIDNode();
    const std::string                                         FactID              = GetNodeValue(*ConditionNodeIDNode).GetValue<std::string>();
    const std::size_t                                         FactArgumentsSize   = WorldState->GetFactArgumentsSize(FactID, FactArguments.size());
    for (std::size_t FactArgumentsIndex = Indices.AddOrIncrementIndex(CurrentNodePath); FactArgumentsIndex < FactArgumentsSize;
         FactArgumentsIndex             = Indices.AddOrIncrementIndex(CurrentNodePath))
    {
        if (!HTNConditionQueryWorldState::Check(*WorldState, FactID, FactArgumentsIndex, FactArguments))
        {
            continue;
        }

        // TODO salvarez Detect whether it is a choice point in the domain validator and store the data in the condition node
        if (!ShouldBindFactArguments)
        {
            constexpr bool Result = true;
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        // Copy variables before they change
        const HTNVariables Variables = Environment.GetVariables();

        // Update variables
        for (std::size_t i = 0; i < ArgumentNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode = ArgumentNodes[i];
            const HTNAtom                                            FactArgument = FactArguments[i];
            SetNodeValue(*ArgumentNode, FactArgument);
        }

        const bool IsLastFactEntry = (FactArgumentsIndex == FactArgumentsSize - 1);
        if (IsLastFactEntry)
        {
            constexpr bool Result = true;
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        // Copy unchanged variables to decomposition
        HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
        HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
        NewEnvironment.SetVariables(Variables);

        // Record decomposition
        mDecompositionContext->RecordDecomposition(NewDecomposition);

        constexpr bool Result = true;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
        return Result;
    }

    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    OPTICK_EVENT("GetAxiomConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AxiomConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inAxiomConditionNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    const std::shared_ptr<const HTNIdentifierExpressionNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                                         AxiomNodeID = GetNodeValue(*AxiomConditionNodeIDNode).GetValue<std::string>();
    const std::shared_ptr<const HTNAxiomNode>                 AxiomNode   = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        LOG_ERROR("Axiom node [{}] could not be found", AxiomNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
        return Result;
    }

    std::vector<HTNAtom>                                                  AxiomConditionNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    AxiomConditionNodeArguments.reserve(AxiomConditionNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        const HTNAtom AxiomConditionNodeArgument = GetNodeValue(*AxiomConditionNodeArgumentNode);
        AxiomConditionNodeArguments.emplace_back(AxiomConditionNodeArgument);
    }

    std::vector<HTNAtom> AxiomNodeParameters;
    {
        const HTNDecompositionVariableScopeNodeScope AxiomVariableScopeNodeScope =
            HTNDecompositionVariableScopeNodeScope(*mDecompositionContext, AxiomNodeID);

        // Initialize the input parameters of the axiom node with the arguments of the condition node
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& AxiomNodeParameterNodes = AxiomNode->GetParameterNodes();
        for (std::size_t i = 0; i < AxiomNodeParameterNodes.size(); ++i)
        {
            const std::shared_ptr<const HTNVariableExpressionNode>& AxiomNodeParameterNode     = AxiomNodeParameterNodes[i];
            const HTNAtom&                                          AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
            SetNodeValue(*AxiomNodeParameterNode, AxiomConditionNodeArgument);
        }

        const bool Result = GetNodeValue(*AxiomNode).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        AxiomNodeParameters.reserve(AxiomNodeParameterNodes.size());
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomNodeParameterNode : AxiomNodeParameterNodes)
        {
            const HTNAtom AxiomNodeParameter = GetNodeValue(*AxiomNodeParameterNode);
            AxiomNodeParameters.emplace_back(AxiomNodeParameter);
        }
    }

    // Initialize the arguments of the condition node with the output parameters of the axiom node
    for (std::size_t i = 0; i < AxiomConditionNodeArgumentNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode = AxiomConditionNodeArgumentNodes[i];
        const HTNAtom&                                           AxiomNodeArgument              = AxiomNodeParameters[i];
        SetNodeValue(*AxiomConditionNodeArgumentNode, AxiomNodeArgument);
    }

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    OPTICK_EVENT("GetAndConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AndConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inAndConditionNode.GetID());
    const std::string&              CurrentNodePath       = mDecompositionContext->GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAndConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode).GetValue<bool>();
        if (!Result)
        {
            if (SubConditionIndex == 0) // First sub-condition
            {
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
                return Result;
            }
            else
            {
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
                // Restore state: unbound variables but updated indices
                if (!mDecompositionContext->RestoreDecomposition())
                {
                    break;
                }
#ifdef HTN_DEBUG
                RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint);
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
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    OPTICK_EVENT("GetOrConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope OrConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inOrConditionNode.GetID());
    const std::string&              CurrentNodePath      = mDecompositionContext->GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inOrConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Copy decomposition history
        const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext->GetDecompositionHistory();

        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode).GetValue<bool>();

        // Reset decomposition history
        mDecompositionContext->SetDecompositionHistory(DecompositionHistory);

        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        // Continue
        Indices.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    constexpr bool Result = false;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    OPTICK_EVENT("GetAltConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope AltConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inAltConditionNode.GetID());
    const std::string&              CurrentNodePath       = mDecompositionContext->GetCurrentNodePath().GetNodePath();

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndices&             Indices              = Environment.GetIndicesMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAltConditionNode.GetSubConditionNodes();
    const std::size_t                                               SubConditionNodesSize = SubConditionNodes.size();
    for (std::size_t SubConditionIndex = Indices.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex             = Indices.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result           = GetNodeValue(*SubConditionNode).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            return Result;
        }

        if (SubConditionIndex == SubConditionNodesSize - 1) // Last sub-condition
        {
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
            // Restore state: unbound variables but updated indices
            if (!mDecompositionContext->RestoreDecomposition())
            {
                break;
            }
#ifdef HTN_DEBUG
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint);
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
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

// NO bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    OPTICK_EVENT("GetNotConditionNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope NotConditionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inNotConditionNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const HTNVariables Variables = Environment.GetVariables();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = mDecompositionContext->GetDecompositionHistory();

    // Check sub-condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    const bool                                         Result           = !GetNodeValue(*SubConditionNode).GetValue<bool>();

    // Reset variables
    Environment.SetVariables(Variables);

    // Reset decomposition history
    mDecompositionContext->SetDecompositionHistory(DecompositionHistory);

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    OPTICK_EVENT("GetCompoundTaskNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope CompoundTaskNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inCompoundTaskNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    const std::shared_ptr<const HTNIdentifierExpressionNode>& CompoundTaskNodeIDNode = inCompoundTaskNode.GetIDNode();
    const std::string                                         MethodNodeID           = GetNodeValue(*CompoundTaskNodeIDNode).GetValue<std::string>();
    const std::shared_ptr<const HTNMethodNode>                MethodNode             = mDomainNode->FindMethodNodeByID(MethodNodeID);
    if (!MethodNode)
    {
        LOG_ERROR("Method node [{}] could not be found", MethodNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
        return Result;
    }

    if (inCompoundTaskNode.IsTopLevel() && !MethodNode->IsTopLevel())
    {
        LOG_ERROR("Method node [{}] is not top-level", MethodNodeID);
        constexpr bool Result = false;
#ifdef HTN_DEBUG
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
        return Result;
    }

    std::vector<HTNAtom>                                                  CompoundTaskNodeArguments;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    CompoundTaskNodeArguments.reserve(CompoundTaskNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        const HTNAtom CompoundTaskNodeArgument = GetNodeValue(*CompoundTaskNodeArgumentNode);
        CompoundTaskNodeArguments.emplace_back(CompoundTaskNodeArgument);
    }

    const HTNDecompositionVariableScopeNodeScope MethodVariableScopeNodeScope =
        HTNDecompositionVariableScopeNodeScope(*mDecompositionContext, MethodNodeID);

    // Initialize the input parameters of the method node with the arguments of the compound task node
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& MethodNodeParameterNodes = MethodNode->GetParameterNodes();
    for (std::size_t i = 0; i < MethodNodeParameterNodes.size(); ++i)
    {
        const std::shared_ptr<const HTNVariableExpressionNode>& MethodNodeParameterNode  = MethodNodeParameterNodes[i];
        const HTNAtom&                                          CompoundTaskNodeArgument = CompoundTaskNodeArguments[i];
        SetNodeValue(*MethodNodeParameterNode, CompoundTaskNodeArgument);
    }

    const bool Result = GetNodeValue(*MethodNode).GetValue<bool>();
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    OPTICK_EVENT("GetPrimitiveTaskNodeValue");

#ifdef HTN_DEBUG
    constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    constexpr bool        IsChoicePoint = false;
#endif

    const HTNDecompositionNodeScope PrimitiveTaskNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inPrimitiveTaskNode.GetID());

#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint);
#endif

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    const std::string                                         ID     = GetNodeValue(*IDNode).GetValue<std::string>();

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    std::vector<HTNAtom>                                                  Arguments;
    Arguments.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = GetNodeValue(*ArgumentNode);
        Arguments.emplace_back(Argument);
    }

    const HTNTaskResult TaskResult = HTNTaskResult(ID, Arguments);
    CurrentDecomposition.AddTaskResultToPlan(TaskResult);

    constexpr bool Result = true;
#ifdef HTN_DEBUG
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode)
{
    OPTICK_EVENT("GetIdentifierExpressionNodeValue");

    const HTNDecompositionNodeScope IdentifierExpressionNodeScope =
        HTNDecompositionNodeScope(*mDecompositionContext, inIdentifierExpressionNode.GetID());

    return inIdentifierExpressionNode.GetValue();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    OPTICK_EVENT("GetLiteralExpressionNodeValue");

    const HTNDecompositionNodeScope LiteralExpressionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inLiteralExpressionNode.GetID());

    return inLiteralExpressionNode.GetValue();
}

void HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue)
{
    OPTICK_EVENT("SetVariableExpressionNodeValue");

    const HTNDecompositionNodeScope VariableExpressionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inVariableExpressionNode.GetID());

    const std::string  VariableID                   = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = mDecompositionContext->GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return;
    }

    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNVariables&           Variables            = Environment.GetVariablesMutable();
    Variables.SetVariable(CurrentVariablePath, inVariableExpressionNodeValue);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    OPTICK_EVENT("GetVariableExpressionNodeValue");

    const HTNDecompositionNodeScope VariableExpressionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inVariableExpressionNode.GetID());

    const std::string  VariableID                   = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& CurrentVariableScopeNodePath = mDecompositionContext->GetCurrentVariableScopeNodePath().GetNodePath();
    std::string        CurrentVariablePath;
    if (!HTNDecompositionHelpers::MakeVariablePath(VariableID, CurrentVariableScopeNodePath, CurrentVariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return HTNAtom();
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext->GetCurrentDecomposition();
    const HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironment();
    const HTNVariables&           Variables            = Environment.GetVariables();
    return Variables.FindVariable(CurrentVariablePath);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    OPTICK_EVENT("GetConstantExpressionNodeValue");

    const HTNDecompositionNodeScope ConstantExpressionNodeScope = HTNDecompositionNodeScope(*mDecompositionContext, inConstantExpressionNode.GetID());

    const std::string&                     ConstantNodeID = inConstantExpressionNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtom();
    }

    return GetNodeValue(*ConstantNode);
}

void HTNDomainInterpreter::Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                 HTNDecompositionContext& ioDecompositionContext)
{
    mDomainNode           = inDomainNode;
    mEntryPointID         = inEntryPointID;
    mDecompositionContext = &ioDecompositionContext;
}

#ifdef HTN_DEBUG
void HTNDomainInterpreter::RecordCurrentNodeSnapshot(const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    const std::string& CurrentNodePath = mDecompositionContext->GetCurrentNodePath().GetNodePath();
    mDecompositionContext->RecordNodeSnapshot(CurrentNodePath, inResult, inNodeStep, inIsChoicePoint);
}

void HTNDomainInterpreter::RecordCurrentNodeSnapshot(const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    const std::string& CurrentNodePath = mDecompositionContext->GetCurrentNodePath().GetNodePath();
    mDecompositionContext->RecordNodeSnapshot(CurrentNodePath, inNodeStep, inIsChoicePoint);
}
#endif
