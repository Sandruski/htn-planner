#include "Domain/HTNDecompositionPrinterContext.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

HTNDecompositionPrinterContext::HTNDecompositionPrinterContext(
    const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
    const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const std::unordered_map<std::string, HTNDecompositionNodeState>& inNodeStates,
    const std::unordered_map<std::string, HTNDecompositionChoicePointNodeState>& inChoicePointNodeStates, const HTNDecompositionNode& inSelectedNode,
    const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreNewNodeOpen)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(&inDecompositionSnapshot), mTooltipMode(inTooltipMode),
      mShouldIgnoreNewNodeOpen(inShouldIgnoreNewNodeOpen), mNodeStates(inNodeStates), mChoicePointNodeStates(inChoicePointNodeStates),
      mSelectedNode(inSelectedNode)
{
}

void HTNDecompositionPrinterContext::SelectNode(const HTNDecompositionNode& inNode)
{
    mSelectedNode           = inNode;
    mIsSelectedNodeSelected = true;
}

void HTNDecompositionPrinterContext::UnselectSelectedNode()
{
    mSelectedNode           = HTNDecompositionNode();
    mIsSelectedNodeSelected = false;
}

bool HTNDecompositionPrinterContext::IsNodeSelected(const std::string& inNodeLabel) const
{
    const std::string& SelectedNodeLabel = mSelectedNode.GetNodeLabel();
    return (inNodeLabel == SelectedNodeLabel);
}

bool HTNDecompositionPrinterContext::IsCurrentDecompositionStepValid() const
{
    return HTNDecompositionHelpers::IsDecompositionStepValid(mCurrentDecompositionStep);
}

HTNNodeStep HTNDecompositionPrinterContext::GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const
{
    const HTNDecompositionNodeStateBase& NodeState = inIsChoicePoint
                                                         ? static_cast<const HTNDecompositionNodeStateBase&>(mChoicePointNodeStates.at(inNodePath))
                                                         : static_cast<const HTNDecompositionNodeStateBase&>(mNodeStates.at(inNodePath));
    return NodeState.GetNodeStep();
}

int HTNDecompositionPrinterContext::GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const
{
    const HTNDecompositionNodeStateBase& NodeState = inIsChoicePoint
                                                         ? static_cast<const HTNDecompositionNodeStateBase&>(mChoicePointNodeStates.at(inNodePath))
                                                         : static_cast<const HTNDecompositionNodeStateBase&>(mNodeStates.at(inNodePath));

    const HTNNodeStep NodeStep = NodeState.GetNodeStep();
    switch (NodeStep)
    {
    case HTNNodeStep::START: {
        return mCurrentDecompositionStep;
    }
    case HTNNodeStep::END: {
        return NodeState.GetDecompositionStep();
    }
    case HTNNodeStep::NONE:
    default: {
        assert(false);
    }
    }

    return HTNDecompositionHelpers::kInvalidDecompositionStep;
}

bool HTNDecompositionPrinterContext::IsNodeOpen(const std::string& inNodePath, const int inDecompositionStep, const bool inIsChoicePoint) const
{
    if (inIsChoicePoint)
    {
        const HTNDecompositionChoicePointNodeState& NodeState = mChoicePointNodeStates.at(inNodePath);
        return NodeState.IsOpen(inDecompositionStep);
    }
    else
    {
        const HTNDecompositionNodeState& NodeState = mNodeStates.at(inNodePath);
        return NodeState.IsOpen();
    }
}
#endif
