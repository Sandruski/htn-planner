// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNDomainInterpreter.h"

#include "Core/HTNPathManager.h"
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNConditionQuery.h"
#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/Scope/HTNNodeScope.h"
#include "Domain/Interpreter/Scope/HTNVariablesScope.h"
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
HTN_NODISCARD HTNDecompositionContext& GetDecompositionContext(HTNNodeVisitorContextBase& ioDecompositionContext)
{
    return static_cast<HTNDecompositionContext&>(ioDecompositionContext);
}

#ifdef HTN_DEBUG_DECOMPOSITION
void RecordCurrentNodeSnapshot(const HTNNodeStep inNodeStep, const bool inIsChoicePoint, HTNDecompositionContext& ioDecompositionContext)
{
    OPTICK_EVENT("RecordNodeResult");

    const std::string& CurrentNodePath = ioDecompositionContext.GetCurrentNodePathManager().GetPath();
    ioDecompositionContext.RecordNodeSnapshot(CurrentNodePath, inNodeStep, inIsChoicePoint);
}

void RecordCurrentNodeSnapshot(const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint,
                               HTNDecompositionContext& ioDecompositionContext)
{
    OPTICK_EVENT("RecordNodeResult");

    const std::string& CurrentNodePath = ioDecompositionContext.GetCurrentNodePathManager().GetPath();
    ioDecompositionContext.RecordNodeSnapshot(CurrentNodePath, inResult, inNodeStep, inIsChoicePoint);
}
#endif
} // namespace

bool HTNDomainInterpreter::Interpret(HTNDecompositionContext& ioDecompositionContext) const
{
    OPTICK_EVENT("InterpretDomain");

    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDecompositionContext.GetDomainNode();
    if (!DomainNode)
    {
        HTN_LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode, ioDecompositionContext).GetValue<bool>();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetDomainNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const std::string       DomainNodeID         = inDomainNode.GetID();
    const HTNNodeScope      DomainNodeScope      = HTNNodeScope(DomainNodeID, DecompositionContext);
    const HTNVariablesScope DomainVariablesScope = HTNVariablesScope(DomainNodeID, DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    if (!inDomainNode.IsTopLevel())
    {
        HTN_LOG_ERROR("Domain node [{}] is not top-level", inDomainNode.GetID());
        return false;
    }

    // Top-level compound task node
    const std::string& EntryPointID = DecompositionContext.GetEntryPointID();
    if (EntryPointID.empty())
    {
        HTN_LOG_ERROR("Entry point ID is empty");
        return false;
    }

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode    = HTNDomainHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    const HTNPathManager&                            CurrentNodePathManager      = DecompositionContext.GetCurrentNodePathManager();
    const HTNPathManager&                            CurrentVariablesPathManager = DecompositionContext.GetCurrentVariablesPathManager();
    const HTNTaskInstance                            TopLevelCompoundTaskInstance =
        HTNTaskInstance(TopLevelCompoundTaskNode, HTNEnvironment(), CurrentNodePathManager, CurrentVariablesPathManager);
    CurrentDecomposition.PushPendingTaskInstance(TopLevelCompoundTaskInstance);

    // Perform a depth-first search (DFS) on the domain graph
    while (CurrentDecomposition.HasPendingTaskInstances())
    {
        const HTNTaskInstance TaskInstance = CurrentDecomposition.PopPendingTaskInstance();

        // Initialize the state with that of the task instance
        DecompositionContext.SetCurrentNodePathManager(TaskInstance.GetNodePathManager());
        DecompositionContext.SetCurrentVariablesPathManager(TaskInstance.GetVariablesPathManager());

        // Also set the environment because it is synced to the current node and variable scope paths
        CurrentDecomposition.SetEnvironment(TaskInstance.GetEnvironment());

        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskInstance.GetTaskNode();
        const bool                                    Result   = GetNodeValue(*TaskNode, ioDecompositionContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }

    static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetConstantNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

    const HTNNodeScope ConstantNodeScope = HTNNodeScope(inConstantNode.GetID(), DecompositionContext);

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode, ioDecompositionContext);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetAxiomNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope AxiomNodeScope = HTNNodeScope(inAxiomNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode();
    if (!ConditionNode)
    {
        // If empty, it evaluates to true
        static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    // Check condition
    const bool Result = GetNodeValue(*ConditionNode, ioDecompositionContext).GetValue<bool>();
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetMethodNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope MethodNodeScope = HTNNodeScope(inMethodNode.GetID(), DecompositionContext);
    const std::string& CurrentNodePath = DecompositionContext.GetCurrentNodePathManager().GetPath();

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndicesManager&      IndicesManager       = Environment.GetIndicesManagerMutable();

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes     = inMethodNode.GetBranchNodes();
    const size                                               BranchNodesSize = BranchNodes.size();
    for (size BranchIndex = IndicesManager.AddIndex(CurrentNodePath); BranchIndex < BranchNodesSize;
         BranchIndex      = IndicesManager.GetIndex(CurrentNodePath))
    {
        // Check branch
        const std::shared_ptr<const HTNBranchNode>& BranchNode = BranchNodes[BranchIndex];
        const bool                                  Result     = GetNodeValue(*BranchNode, ioDecompositionContext).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        if (BranchIndex == BranchNodesSize - 1) // Last branch
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            // Restore state: unbound variables but updated indices
            if (!DecompositionContext.RestoreDecomposition())
            {
                return Result;
            }
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
        }
        else
        {
            // Continue
            IndicesManager.IncrementIndex(CurrentNodePath);
        }
    }

    static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetBranchNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope BranchNodeScope = HTNNodeScope(inBranchNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    if (CurrentPreConditionNode)
    {
        const bool Result = GetNodeValue(*CurrentPreConditionNode, ioDecompositionContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    const HTNPathManager& CurrentNodePathManager      = DecompositionContext.GetCurrentNodePathManager();
    const HTNPathManager& CurrentVariablesPathManager = DecompositionContext.GetCurrentVariablesPathManager();

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (int32 i = static_cast<int32>(TaskNodes.size()) - 1; i >= 0; --i)
    {
        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
        const HTNTaskInstance TaskInstance = HTNTaskInstance(TaskNode, Environment, CurrentNodePathManager, CurrentVariablesPathManager);
        CurrentDecomposition.PushPendingTaskInstance(TaskInstance);
    }

    static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
#endif

    const HTNNodeScope ConditionNodeScope = HTNNodeScope(inConditionNode.GetID(), DecompositionContext);
    const std::string& CurrentNodePath    = DecompositionContext.GetCurrentNodePathManager().GetPath();

    // Gather fact arguments
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes     = inConditionNode.GetArgumentNodes();
    const size                                                            ArgumentNodesSize = ArgumentNodes.size();
    std::vector<HTNAtom>                                                  FactArguments;
    FactArguments.reserve(ArgumentNodesSize);

    bool ShouldBindFactArguments = false;
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument  = GetNodeValue(*ArgumentNode, ioDecompositionContext);
        ShouldBindFactArguments = ShouldBindFactArguments || !Argument.IsBound();
        FactArguments.emplace_back(Argument);
    }

#ifdef HTN_DEBUG_DECOMPOSITION
    // Detect whether this condition node is a choice point
    // TODO salvarez Move to a validation step after parsing
    const bool IsChoicePoint = ShouldBindFactArguments;
#endif

#ifdef HTN_VALIDATE_DOMAIN
    {
        OPTICK_EVENT("ValidateNode");

        // Check that if an argument is any it is unbound
        // TODO salvarez Move to a validation step after parsing
        for (size i = 0; i < ArgumentNodesSize; ++i)
        {
            const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode = ArgumentNodes[i];

            // Variable IDs can only be strings
            const bool IsVariableID = ArgumentNode->GetValue().IsType<std::string>();
            if (!IsVariableID)
            {
                continue;
            }

            const std::string& VariableID    = ArgumentNode->GetValue().GetValue<std::string>();
            const bool         IsAnyArgument = HTNDomainHelpers::IsAnyArgument(VariableID);
            if (!IsAnyArgument)
            {
                continue;
            }

            const HTNAtom& Argument = FactArguments[i];
            if (!Argument.IsBound())
            {
                continue;
            }

            HTN_LOG_ERROR("Condition node's argument [{}] is any and is bound", VariableID);
            static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }
#endif

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndicesManager&      IndicesManager       = Environment.GetIndicesManagerMutable();

    const HTNWorldState& WorldState = DecompositionContext.GetWorldState();

    // Check fact
    const std::shared_ptr<const HTNIdentifierExpressionNode>& ConditionNodeIDNode = inConditionNode.GetIDNode();
    const std::string FactID            = GetNodeValue(*ConditionNodeIDNode, ioDecompositionContext).GetValue<std::string>();
    const size        FactArgumentsSize = WorldState.GetFactArgumentsSize(FactID, FactArguments.size());
    for (size FactArgumentsIndex = IndicesManager.AddOrIncrementIndex(CurrentNodePath); FactArgumentsIndex < FactArgumentsSize;
         FactArgumentsIndex      = IndicesManager.AddOrIncrementIndex(CurrentNodePath))
    {
        static const HTNConditionWorldStateQuery ConditionWorldStateQuery;
        if (!ConditionWorldStateQuery.Check(WorldState, FactID, FactArgumentsIndex, FactArguments))
        {
            continue;
        }

        if (!ShouldBindFactArguments)
        {
            static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Copy variables before they change
        const HTNVariablesManager VariablesManager = Environment.GetVariablesManager();

        // Update variables
        for (size i = 0; i < ArgumentNodesSize; ++i)
        {
            const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode = ArgumentNodes[i];
            const HTNAtom                                            FactArgument = FactArguments[i];
            SetNodeValue(*ArgumentNode, FactArgument, ioDecompositionContext);
        }

        const bool IsLastFactEntry = FactArgumentsIndex == (FactArgumentsSize - 1);
        if (IsLastFactEntry)
        {
            static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Copy unchanged variables to decomposition
        HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
        HTNEnvironment&        NewEnvironment   = NewDecomposition.GetEnvironmentMutable();
        NewEnvironment.SetVariablesManager(VariablesManager);

        // Record decomposition
        DecompositionContext.RecordDecomposition(NewDecomposition);

        static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetAxiomConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope AxiomConditionNodeScope = HTNNodeScope(inAxiomConditionNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNDomainNode>&               DomainNode               = DecompositionContext.GetDomainNode();
    const std::shared_ptr<const HTNIdentifierExpressionNode>& AxiomConditionNodeIDNode = inAxiomConditionNode.GetIDNode();
    const std::string                         AxiomNodeID = GetNodeValue(*AxiomConditionNodeIDNode, ioDecompositionContext).GetValue<std::string>();
    const std::shared_ptr<const HTNAxiomNode> AxiomNode   = DomainNode->FindAxiomNodeByID(AxiomNodeID);
    if (!AxiomNode)
    {
        HTN_LOG_ERROR("Axiom node [{}] could not be found", AxiomNodeID);
        static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    const size                                                           AxiomConditionNodeArgumentNodesSize = AxiomConditionNodeArgumentNodes.size();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& AxiomNodeParameterNodes             = AxiomNode->GetParameterNodes();
    const size                                                           AxiomNodeParameterNodesSize         = AxiomNodeParameterNodes.size();
#ifdef HTN_VALIDATE_DOMAIN
    {
        OPTICK_EVENT("ValidateNode");

        // Check the number of arguments of an axiom condition node is the same as the number of parameters of the axiom node that it calls
        // TODO salvarez Move to a validation step after parsing
        if (AxiomConditionNodeArgumentNodesSize != AxiomNodeParameterNodesSize)
        {
            HTN_LOG_ERROR("Axiom node of [{}] parameter(s) cannot be called by axiom condition node of [{}] argument(s)", AxiomNodeParameterNodesSize,
                          AxiomConditionNodeArgumentNodesSize);
            static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
    }
#endif

    std::vector<HTNAtom> AxiomConditionNodeArguments;
    AxiomConditionNodeArguments.reserve(AxiomConditionNodeArgumentNodesSize);
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        const HTNAtom AxiomConditionNodeArgument = GetNodeValue(*AxiomConditionNodeArgumentNode, ioDecompositionContext);
        AxiomConditionNodeArguments.emplace_back(AxiomConditionNodeArgument);
    }

    std::vector<HTNAtom> AxiomNodeParameters;
    {
        const HTNVariablesScope AxiomVariablesScope = HTNVariablesScope(AxiomNodeID, DecompositionContext);

        // Initialize the input parameters of the axiom node with the arguments of the axiom condition node
        for (size i = 0; i < AxiomNodeParameterNodesSize; ++i)
        {
            const std::shared_ptr<const HTNVariableExpressionNode>& AxiomNodeParameterNode = AxiomNodeParameterNodes[i];
            const std::string&                                      VariableID       = AxiomNodeParameterNode->GetValue().GetValue<std::string>();
            const bool                                              IsInputParameter = HTNDomainHelpers::IsInputParameter(VariableID);
            const bool                                              IsInputOutputParameter = HTNDomainHelpers::IsInputOutputParameter(VariableID);
            if (!IsInputParameter && !IsInputOutputParameter)
            {
#ifdef HTN_VALIDATE_DOMAIN
                {
                    OPTICK_EVENT("ValidateNode");

                    // Check that the parameter is input, output, or input/output
                    const bool IsOutputParameter = HTNDomainHelpers::IsOutputParameter(VariableID);
                    if (!IsOutputParameter)
                    {
                        HTN_LOG_ERROR("Axiom node's parameter [{}] is not input, output, or input/output", VariableID);
                        static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
                        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                        return Result;
                    }
#endif
                }

                continue;
            }

            const HTNAtom& AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
#ifdef HTN_VALIDATE_DOMAIN
            {
                OPTICK_EVENT("ValidateNode");

                // Check that the argument for the input parameter is bound
                // TODO salvarez Move to a validation step after parsing
                if (IsInputParameter)
                {
                    if (!AxiomConditionNodeArgument.IsBound())
                    {
#ifdef HTN_DEBUG_DECOMPOSITION
                        static constexpr bool    ShouldDoubleQuoteString = false;
                        static const std::string ParameterType           = IsInputParameter ? "input" : "input/output";
#endif
                        HTN_LOG_ERROR(
                            "Axiom node's {} parameter [{}] cannot be initialized with axiom condition node's argument [{}] because it is unbound",
                            ParameterType, VariableID, AxiomConditionNodeArgumentNodes[i]->GetValue().ToString(ShouldDoubleQuoteString));
                        static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
                        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                        return Result;
                    }
                }
#endif
            }

            SetNodeValue(*AxiomNodeParameterNode, AxiomConditionNodeArgument, ioDecompositionContext);
        }

        const bool Result = GetNodeValue(*AxiomNode, ioDecompositionContext).GetValue<bool>();
        if (!Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        AxiomNodeParameters.reserve(AxiomNodeParameterNodesSize);
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomNodeParameterNode : AxiomNodeParameterNodes)
        {
            const HTNAtom AxiomNodeParameter = GetNodeValue(*AxiomNodeParameterNode, ioDecompositionContext);
            AxiomNodeParameters.emplace_back(AxiomNodeParameter);
        }
    }

    // Initialize the arguments of the axiom condition node with the output parameters of the axiom node
    for (size i = 0; i < AxiomConditionNodeArgumentNodesSize; ++i)
    {
        const std::shared_ptr<const HTNVariableExpressionNode>& AxiomNodeParameterNode = AxiomNodeParameterNodes[i];
        const std::string&                                      VariableID             = AxiomNodeParameterNode->GetValue().GetValue<std::string>();
        const bool                                              IsOutputParameter      = HTNDomainHelpers::IsOutputParameter(VariableID);
        const bool                                              IsInputOutputParameter = HTNDomainHelpers::IsInputOutputParameter(VariableID);
        if (!IsOutputParameter && !IsInputOutputParameter)
        {
            continue;
        }

        const std::shared_ptr<const HTNValueExpressionNodeBase>& AxiomConditionNodeArgumentNode = AxiomConditionNodeArgumentNodes[i];

#ifdef HTN_VALIDATE_DOMAIN
        {
            OPTICK_EVENT("ValidateNode");

            // Check that the argument for the output parameter is unbound
            // TODO salvarez Move to a validation step after parsing
            if (IsOutputParameter)
            {
                const HTNAtom& AxiomConditionNodeArgument = AxiomConditionNodeArguments[i];
                if (AxiomConditionNodeArgument.IsBound())
                {
#ifdef HTN_DEBUG_DECOMPOSITION
                    static constexpr bool    ShouldDoubleQuoteString = false;
                    static const std::string ParameterType           = IsOutputParameter ? "output" : "input/output";
#endif
                    HTN_LOG_ERROR(
                        "Axiom condition node's argument [{}] cannot be initialized with axiom node's {} parameter [{}] because it is already bound",
                        AxiomConditionNodeArgumentNode->GetValue().ToString(ShouldDoubleQuoteString), ParameterType, VariableID);
                    static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
                    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                    return Result;
                }
            }
#endif
        }

        const HTNAtom& AxiomNodeParameter = AxiomNodeParameters[i];
        SetNodeValue(*AxiomConditionNodeArgumentNode, AxiomNodeParameter, ioDecompositionContext);
    }

    static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetAndConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope AndConditionNodeScope = HTNNodeScope(inAndConditionNode.GetID(), DecompositionContext);
    const std::string& CurrentNodePath       = DecompositionContext.GetCurrentNodePathManager().GetPath();

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndicesManager&      IndicesManager       = Environment.GetIndicesManagerMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAndConditionNode.GetSubConditionNodes();
    const size                                                      SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = IndicesManager.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex      = IndicesManager.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result = GetNodeValue(*SubConditionNode, ioDecompositionContext).GetValue<bool>();
        if (!Result)
        {
            if (SubConditionIndex == 0) // First sub-condition
            {
#ifdef HTN_DEBUG_DECOMPOSITION
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                return Result;
            }
            else
            {
#ifdef HTN_DEBUG_DECOMPOSITION
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                // Restore state: unbound variables but updated indices
                if (!DecompositionContext.RestoreDecomposition())
                {
                    return Result;
                }
#ifdef HTN_DEBUG_DECOMPOSITION
                RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
                continue;
            }
        }

        // Continue
        IndicesManager.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to true
    static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetOrConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope OrConditionNodeScope = HTNNodeScope(inOrConditionNode.GetID(), DecompositionContext);
    const std::string& CurrentNodePath      = DecompositionContext.GetCurrentNodePathManager().GetPath();

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndicesManager&      IndicesManager       = Environment.GetIndicesManagerMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inOrConditionNode.GetSubConditionNodes();
    const size                                                      SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = IndicesManager.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex      = IndicesManager.GetIndex(CurrentNodePath))
    {
        // Copy decomposition record history
        const HTNDecompositionRecordHistory DecompositionRecordHistory = DecompositionContext.GetDecompositionRecordHistory();

        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result = GetNodeValue(*SubConditionNode, ioDecompositionContext).GetValue<bool>();

        // Reset decomposition record history
        DecompositionContext.SetDecompositionRecordHistory(DecompositionRecordHistory);

        if (Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        // Continue
        IndicesManager.IncrementIndex(CurrentNodePath);
    }

    // If empty, it evaluates to false
    static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// YES bindings
// YES backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetAltConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope AltConditionNodeScope = HTNNodeScope(inAltConditionNode.GetID(), DecompositionContext);
    const std::string& CurrentNodePath       = DecompositionContext.GetCurrentNodePathManager().GetPath();

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNIndicesManager&      IndicesManager       = Environment.GetIndicesManagerMutable();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes     = inAltConditionNode.GetSubConditionNodes();
    const size                                                      SubConditionNodesSize = SubConditionNodes.size();
    for (size SubConditionIndex = IndicesManager.AddIndex(CurrentNodePath); SubConditionIndex < SubConditionNodesSize;
         SubConditionIndex      = IndicesManager.GetIndex(CurrentNodePath))
    {
        // Check sub-condition
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = SubConditionNodes[SubConditionIndex];
        const bool                                         Result = GetNodeValue(*SubConditionNode, ioDecompositionContext).GetValue<bool>();
        if (Result)
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }

        if (SubConditionIndex == SubConditionNodesSize - 1) // Last sub-condition
        {
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            // Restore state: unbound variables but updated indices
            if (!DecompositionContext.RestoreDecomposition())
            {
                return Result;
            }
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, StartNodeStep, IsChoicePoint, DecompositionContext);
#endif
        }
        else
        {
            // Continue
            IndicesManager.IncrementIndex(CurrentNodePath);
        }
    }

    // If empty, it evaluates to false
    static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

// NO bindings
// NO backtracking
HTNAtom HTNDomainInterpreter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetNotConditionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope NotConditionNodeScope = HTNNodeScope(inNotConditionNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();

    // Copy variables
    const HTNVariablesManager VariablesManager = Environment.GetVariablesManager();

    // Copy decomposition history
    const HTNDecompositionRecordHistory DecompositionRecordHistory = DecompositionContext.GetDecompositionRecordHistory();

    // Check sub-condition
    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    const bool                                         Result           = !GetNodeValue(*SubConditionNode, ioDecompositionContext).GetValue<bool>();

    // Reset variables
    Environment.SetVariablesManager(VariablesManager);

    // Reset decomposition history
    DecompositionContext.SetDecompositionRecordHistory(DecompositionRecordHistory);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetCompoundTaskNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope CompoundTaskNodeScope = HTNNodeScope(inCompoundTaskNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    const std::shared_ptr<const HTNDomainNode>&               DomainNode             = DecompositionContext.GetDomainNode();
    const std::shared_ptr<const HTNIdentifierExpressionNode>& CompoundTaskNodeIDNode = inCompoundTaskNode.GetIDNode();
    const std::string                          MethodNodeID = GetNodeValue(*CompoundTaskNodeIDNode, ioDecompositionContext).GetValue<std::string>();
    const std::shared_ptr<const HTNMethodNode> MethodNode   = DomainNode->FindMethodNodeByID(MethodNodeID);
    if (!MethodNode)
    {
        HTN_LOG_ERROR("Method node [{}] could not be found", MethodNodeID);
        static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    if (inCompoundTaskNode.IsTopLevel() && !MethodNode->IsTopLevel())
    {
        HTN_LOG_ERROR("Method node [{}] is not top-level", MethodNodeID);
        static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
        RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
        return Result;
    }

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  MethodNodeParameterNodes      = MethodNode->GetParameterNodes();
    const size                                                            MethodNodeParameterNodesSize  = MethodNodeParameterNodes.size();
#ifdef HTN_VALIDATE_DOMAIN
    {
        OPTICK_EVENT("ValidateNode");

        // Check the number of arguments of a compound task node is the same as the number of parameters of the method node that it calls
        // TODO salvarez Move to a validation step after parsing
        const size CompoundTaskNodeArgumentNodesSize = CompoundTaskNodeArgumentNodes.size();
        if (CompoundTaskNodeArgumentNodesSize != MethodNodeParameterNodesSize)
        {
            HTN_LOG_ERROR("Method node of [{}] parameter(s) cannot be called by compound task node of [{}] argument(s)", MethodNodeParameterNodesSize,
                          CompoundTaskNodeArgumentNodesSize);
            static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
            RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
            return Result;
        }
#endif
    }

    std::vector<HTNAtom> CompoundTaskNodeArguments;
    CompoundTaskNodeArguments.reserve(CompoundTaskNodeArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        const HTNAtom CompoundTaskNodeArgument = GetNodeValue(*CompoundTaskNodeArgumentNode, ioDecompositionContext);
        CompoundTaskNodeArguments.emplace_back(CompoundTaskNodeArgument);
    }

    const HTNVariablesScope MethodVariablesScope = HTNVariablesScope(MethodNodeID, DecompositionContext);

    // Initialize the input parameters of the method node with the arguments of the compound task node
    for (size i = 0; i < MethodNodeParameterNodesSize; ++i)
    {
        const std::shared_ptr<const HTNVariableExpressionNode>& MethodNodeParameterNode = MethodNodeParameterNodes[i];
        const std::string&                                      VariableID              = MethodNodeParameterNode->GetValue().GetValue<std::string>();
        const bool                                              IsInputParameter        = HTNDomainHelpers::IsInputParameter(VariableID);
        if (!IsInputParameter)
        {
#ifdef HTN_VALIDATE_DOMAIN
            {
                OPTICK_EVENT("ValidateNode");

                // Check that the parameter is input
                HTN_LOG_ERROR("Method node's parameter [{}] is not input", VariableID);
                static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                return Result;
#endif
            }

            continue;
        }

        const HTNAtom& CompoundTaskNodeArgument = CompoundTaskNodeArguments[i];
#ifdef HTN_VALIDATE_DOMAIN
        {
            OPTICK_EVENT("ValidateNode");

            // Check that the argument for the input parameter is bound
            // TODO salvarez Move to a validation step after parsing
            if (!CompoundTaskNodeArgument.IsBound())
            {
#ifdef HTN_DEBUG_DECOMPOSITION
                static constexpr bool ShouldDoubleQuoteString = false;
#endif
                HTN_LOG_ERROR("Method's input parameter [{}] cannot be initialized with compound task node's argument [{}] because it is unbound",
                              VariableID, CompoundTaskNodeArgumentNodes[i]->GetValue().ToString(ShouldDoubleQuoteString));
                static constexpr bool Result = false;
#ifdef HTN_DEBUG_DECOMPOSITION
                RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
                return Result;
            }
#endif
        }

        SetNodeValue(*MethodNodeParameterNode, CompoundTaskNodeArgument, ioDecompositionContext);
    }

    const bool Result = GetNodeValue(*MethodNode, ioDecompositionContext).GetValue<bool>();
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetPrimitiveTaskNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    static constexpr HTNNodeStep StartNodeStep = HTNNodeStep::START;
    static constexpr HTNNodeStep EndNodeStep   = HTNNodeStep::END;
    static constexpr bool        IsChoicePoint = false;
#endif

    const HTNNodeScope PrimitiveTaskNodeScope = HTNNodeScope(inPrimitiveTaskNode.GetID(), DecompositionContext);

#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(StartNodeStep, IsChoicePoint, DecompositionContext);
#endif

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    const std::string                                         ID     = GetNodeValue(*IDNode, ioDecompositionContext).GetValue<std::string>();

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    std::vector<HTNAtom>                                                  Arguments;
    Arguments.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom Argument = GetNodeValue(*ArgumentNode, ioDecompositionContext);
        Arguments.emplace_back(Argument);
    }

    const HTNTaskResult TaskResult = HTNTaskResult(ID, Arguments);
    CurrentDecomposition.AddTaskResultToPlan(TaskResult);

    static constexpr bool Result = true;
#ifdef HTN_DEBUG_DECOMPOSITION
    RecordCurrentNodeSnapshot(Result, EndNodeStep, IsChoicePoint, DecompositionContext);
#endif
    return Result;
}

HTNAtom HTNDomainInterpreter::Visit(const HTNIdentifierExpressionNode&          inIdentifierExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetIdentifierExpressionNodeValue");

    return inIdentifierExpressionNode.GetValue();
}

HTNAtom HTNDomainInterpreter::Visit(const HTNLiteralExpressionNode&             inLiteralExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("GetLiteralExpressionNodeValue");

    return inLiteralExpressionNode.GetValue();
}

void HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                                 HTNNodeVisitorContextBase& ioDecompositionContext) const
{
    OPTICK_EVENT("SetVariableExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

    const std::string     VariableID                  = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const HTNPathManager& CurrentVariablesPathManager = DecompositionContext.GetCurrentVariablesPathManager();
    HTNPathManager        VariablePathManager         = HTNPathManager(CurrentVariablesPathManager);
    HTNScope              VariableScope               = HTNScope(VariableID, VariablePathManager);

    HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    HTNVariablesManager&    VariablesManager     = Environment.GetVariablesManagerMutable();
    const std::string&      CurrentVariablePath  = VariablePathManager.GetPath();
    VariablesManager.SetVariable(CurrentVariablePath, inVariableExpressionNodeValue);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode,
                                    HTNNodeVisitorContextBase&       ioDecompositionContext) const
{
    OPTICK_EVENT("GetVariableExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

    const std::string     VariableID                  = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const HTNPathManager& CurrentVariablesPathManager = DecompositionContext.GetCurrentVariablesPathManager();
    HTNPathManager        VariablePathManager         = HTNPathManager(CurrentVariablesPathManager);
    HTNScope              VariableScope               = HTNScope(VariableID, VariablePathManager);

    const HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecomposition();
    const HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironment();
    const HTNVariablesManager&    VariablesManager     = Environment.GetVariablesManager();
    const std::string&            CurrentVariablePath  = VariablePathManager.GetPath();
    return VariablesManager.FindVariable(CurrentVariablePath);
}

HTNAtom HTNDomainInterpreter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode,
                                    HTNNodeVisitorContextBase&       ioDecompositionContext) const
{
    OPTICK_EVENT("GetConstantExpressionNodeValue");

    HTNDecompositionContext& DecompositionContext = GetDecompositionContext(ioDecompositionContext);

    const std::shared_ptr<const HTNDomainNode>& DomainNode     = DecompositionContext.GetDomainNode();
    const std::string&                          ConstantNodeID = inConstantExpressionNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode>      ConstantNode   = DomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        HTN_LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtom();
    }

    return GetNodeValue(*ConstantNode, ioDecompositionContext);
}
