#include "Domain/HTNDecompositionPrinterContext.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

HTNDecompositionPrinterContext::HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                               const std::string&                          inEntryPointID,
                                                               const HTNDecompositionSnapshotDebug&        inDecompositionSnapshot,
                                                               const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreNewNodeOpen,
                                                               HTNDecompositionNode& ioSelectedNode)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(&inDecompositionSnapshot), mTooltipMode(inTooltipMode),
      mShouldIgnoreNewNodeOpen(inShouldIgnoreNewNodeOpen), mSelectedNode(&ioSelectedNode)
{
}

void HTNDecompositionPrinterContext::Reset()
{
    mNodeStates.clear();
    mChoicePointNodeStates.clear();
    *mSelectedNode = HTNDecompositionNode();
}

void HTNDecompositionPrinterContext::SelectNode(const HTNDecompositionNode& inNode)
{
    *mSelectedNode          = inNode;
    mIsSelectedNodeSelected = true;
}

void HTNDecompositionPrinterContext::UnselectSelectedNode()
{
    *mSelectedNode          = HTNDecompositionNode();
    mIsSelectedNodeSelected = false;
}

bool HTNDecompositionPrinterContext::IsNodeSelected(const std::string& inNodeLabel) const
{
    const std::string& SelectedNodeLabel = mSelectedNode->GetNodeLabel();
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

// TODO salvarez
/*
void HTNDecompositionPrinter::Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                    const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const HTNDecompositionTooltipMode inTooltipMode,
                                    const bool inShouldIgnoreNewNodeOpen, const bool inShouldReset, HTNDecompositionNode& ioSelectedNode)
{
    bool ShouldReset = inShouldReset;
    ShouldReset      = ShouldReset || (mDomainNode != inDomainNode);
    ShouldReset      = ShouldReset || (mEntryPointID != inEntryPointID);
    ShouldReset      = ShouldReset || (mDecompositionSnapshot != &inDecompositionSnapshot);

    mDomainNode              = inDomainNode;
    mEntryPointID            = inEntryPointID;
    mDecompositionSnapshot   = &inDecompositionSnapshot;
    mTooltipMode             = inTooltipMode;
    mShouldIgnoreNewNodeOpen = inShouldIgnoreNewNodeOpen;
    mSelectedNode            = &ioSelectedNode;

    if (ShouldReset)
    {
        mNodeStates.clear();
        mChoicePointNodeStates.clear();
        *mSelectedNode = HTNDecompositionNode();
    }

    mIsSelectedNodeSelected = false;

    mCurrentNodePath              = HTNNodePath();
    mCurrentVariableScopeNodePath = HTNNodePath();

    mCurrentDecompositionStep = 0;
    mMinDecompositionStep     = std::numeric_limits<int>::min();
    mMaxDecompositionStep     = std::numeric_limits<int>::max();

    mIsCurrentNodeVisible    = true;
    mShouldRefreshNodeStates = false;
}
*/
#endif
