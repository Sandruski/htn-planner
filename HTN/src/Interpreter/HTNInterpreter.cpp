#include "Interpreter/HTNInterpreter.h"

#include "HTNHelpers.h"
#include "HTNLog.h"
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
    if (!Result)
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

        return false;
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNBranchNode& inBranchNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = inBranchNode.GetPreConditionNode();
    const bool                                         Result = !CurrentPreConditionNode || EvaluateNode<bool>(*CurrentPreConditionNode);
    if (!Result)
    {
        return false;
    }

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
    {
        const std::shared_ptr<const HTNTaskNodeBase>& TaskNode = TaskNodes[i];
        CurrentDecomposition.PushTaskInstanceToProcess(TaskNode);
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNConditionNodeBase& inConditionNode)
{
    bool HasBoundVariables = false;
    return inConditionNode.Check(mDecompositionContext, HasBoundVariables);
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

    const bool Result = EvaluateNode<bool>(*CurrentMethodNode);
    if (!Result)
    {
        return false;
    }

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    const HTNTaskInstance&  CurrentTaskInstance  = CurrentDecomposition.GetCurrentTaskInstance();
    assert(CurrentTaskInstance.GetTaskNode().get() == &inPrimitiveTaskNode);
    CurrentDecomposition.AddTaskToPlan(CurrentTaskInstance);
    return true;
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNValueNode& inValueNode)
{
    return HTNAtom();
}
