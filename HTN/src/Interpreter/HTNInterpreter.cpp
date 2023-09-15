#include "Interpreter/HTNInterpreter.h"

#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/HTNDecompositionContext.h"
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

    const bool DomainNodeResult = EvaluateNode<bool>(*DomainNode);
    if (!DomainNodeResult)
    {
        return false;
    }

    const HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecomposition();
    outPlan                                            = CurrentDecomposition.GetPlan();

    return true;
}

HTNAtom HTNInterpreter::Visit(const HTNDomainNode& inDomainNode)
{
    const HTNDecompositionScope DomainScope          = HTNDecompositionScope(mDecompositionContext, mDomainNode);
    HTNDecompositionRecord&     CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root compound task
    const std::shared_ptr<const HTNTaskNode> RootTaskNode =
        std::make_shared<HTNTaskNode>(std::make_shared<const HTNValueNode>(HTNAtom(mEntryPointName), HTNValueType::SYMBOL),
                                      std::vector<std::shared_ptr<const HTNValueNode>>(), HTNTaskType::COMPOUND);
    CurrentDecomposition.PushTaskInstanceToProcess(HTNTaskInstance(RootTaskNode, mDecompositionContext.MakeCurrentScopeID()));

    while (CurrentDecomposition.HasTaskInstancesToProcess())
    {
        const HTNTaskInstance&                    CurrentTaskInstance = CurrentDecomposition.PopTaskInstanceToProcess();
        const std::shared_ptr<const HTNTaskNode>& CurrentTaskNode     = CurrentTaskInstance.GetTaskNode();
        if (!CurrentTaskNode)
        {
            break;
        }

        CurrentDecomposition.SetCurrentTaskNode(CurrentTaskNode);

        switch (CurrentTaskNode->GetType())
        {
        case HTNTaskType::PRIMITIVE: {
            CurrentDecomposition.AddTaskToPlan(CurrentTaskInstance);
            break;
        }
        case HTNTaskType::COMPOUND: {
            // The ID of a compound task should match the ID of a method
            const std::string                          CurrentTaskID     = CurrentTaskNode->GetID();
            const std::shared_ptr<const HTNMethodNode> CurrentMethodNode = mDomainNode->FindMethodNodeByID(CurrentTaskID);
            if (!CurrentMethodNode)
            {
                LOG_ERROR("Method node [{}] not found", CurrentTaskID);
                break;
            }

            const HTNDecompositionScope CurrentMethodScope = HTNDecompositionScope(mDecompositionContext, CurrentMethodNode);
            const std::string           CurrentScopeID     = mDecompositionContext.MakeCurrentScopeID();

            // Initialize the input arguments of the method with the arguments of the compound task
            const std::string&                                      CurentTaskScopeID         = CurrentTaskInstance.GetScopeID();
            const std::vector<std::shared_ptr<const HTNValueNode>>& CurentTaskArgumentNodes   = CurrentTaskNode->GetArgumentNodes();
            const std::vector<std::shared_ptr<const HTNValueNode>>& CurentMethodArgumentNodes = CurrentMethodNode->GetArgumentNodes();
            if (!HTN::Helpers::CopyArguments(mDecompositionContext, CurentTaskArgumentNodes, CurentMethodArgumentNodes, CurentTaskScopeID,
                                             CurrentScopeID, {}, HTN::Helpers::InputPrefixes))
            {
                LOG_ERROR("Arguments could not be copied from compound task to method [{}]", CurrentMethodNode->ToString());
                break;
            }

            const unsigned int                                       CurrentBranchIndex = CurrentDecomposition.GetOrAddIndex(CurrentScopeID);
            const std::vector<std::shared_ptr<const HTNBranchNode>>& CurrentBranchNodes = CurrentMethodNode->GetBranchNodes();
            if (CurrentBranchIndex >= CurrentBranchNodes.size())
            {
                break;
            }

            const std::shared_ptr<const HTNBranchNode>&        CurrentBranch           = CurrentBranchNodes[CurrentBranchIndex];
            const std::shared_ptr<const HTNConditionNodeBase>& CurrentPreConditionNode = CurrentBranch->GetPreConditionNode();
            bool                                               HasBoundVariables       = false;
            const bool Result = CurrentPreConditionNode ? CurrentPreConditionNode->Check(mDecompositionContext, HasBoundVariables) : true;
            if (Result)
            {
                const std::vector<std::shared_ptr<const HTNTaskNode>>& TaskNodes = CurrentBranch->GetTaskNodes();
                for (int i = static_cast<int>(TaskNodes.size()) - 1; i >= 0; --i)
                {
                    const std::shared_ptr<const HTNTaskNode>& TaskNode = TaskNodes[i];
                    CurrentDecomposition.PushTaskInstanceToProcess(HTNTaskInstance(TaskNode, CurrentScopeID));
                }
            }
            else
            {
                if (CurrentBranchIndex < CurrentBranchNodes.size() - 1)
                {
                    CurrentDecomposition.PushTaskInstanceToProcess(CurrentTaskInstance);

                    // Continue
                    CurrentDecomposition.IncrementIndex(CurrentScopeID);
                }
                else // Last branch
                {
                    // Restore state: unbound variables but updated indices
                    mDecompositionContext.RestoreDecomposition();

                    CurrentDecomposition.PushTaskInstanceToProcess(HTNTaskInstance(CurrentDecomposition.GetCurrentTaskNode(), CurrentScopeID));
                }
            }
            break;
        }
        default: {
            assert(false);
        }
        }
    }

    return HTNAtom();
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

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNMethodNode& inMethodNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNBranchNode& inBranchNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNConditionNodeBase& inConditionNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNTaskNode& inTaskNode)
{
    return HTNAtom();
}

HTNAtom HTNInterpreter::Visit([[maybe_unused]] const HTNValueNode& inValueNode)
{
    return HTNAtom();
}
