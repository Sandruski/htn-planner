// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

HTNDecompositionPrinterContext::HTNDecompositionPrinterContext(
    const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
    const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const HTNDecompositionTooltipMode inTooltipMode,
    const bool inShouldIgnoreImGuiState, std::unordered_map<std::string, HTNDecompositionNodeState>& ioNodeStates,
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState>& ioChoicePointNodeStates, HTNDecompositionNode& ioSelectedNode)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot), mTooltipMode(inTooltipMode),
      mShouldIgnoreImGuiState(inShouldIgnoreImGuiState), mNodeStates(ioNodeStates), mChoicePointNodeStates(ioChoicePointNodeStates),
      mSelectedNode(ioSelectedNode)
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

void HTNDecompositionPrinterContext::AddNodeState(const std::string& inNodePath, const HTNDecompositionNodeState& inNodeState)
{
    mNodeStates[inNodePath] = inNodeState;
}

const HTNDecompositionNodeState& HTNDecompositionPrinterContext::GetNodeState(const std::string& inNodePath) const
{
    return mNodeStates.at(inNodePath);
}

HTNDecompositionNodeState& HTNDecompositionPrinterContext::GetNodeStateMutable(const std::string& inNodePath)
{
    return mNodeStates.at(inNodePath);
}

const HTNDecompositionNodeState* HTNDecompositionPrinterContext::FindNodeState(const std::string& inNodePath) const
{
    const auto It = mNodeStates.find(inNodePath);
    return ((It != mNodeStates.end()) ? &It->second : nullptr);
}

HTNDecompositionNodeState* HTNDecompositionPrinterContext::FindNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mNodeStates.find(inNodePath);
    return ((It != mNodeStates.end()) ? &It->second : nullptr);
}

void HTNDecompositionPrinterContext::AddChoicePointNodeState(const std::string&                          inNodePath,
                                                             const HTNDecompositionChoicePointNodeState& inChoicePointNodeState)
{
    mChoicePointNodeStates[inNodePath] = inChoicePointNodeState;
}

const HTNDecompositionChoicePointNodeState& HTNDecompositionPrinterContext::GetChoicePointNodeState(const std::string& inNodePath) const
{
    return mChoicePointNodeStates.at(inNodePath);
}

HTNDecompositionChoicePointNodeState& HTNDecompositionPrinterContext::GetChoicePointNodeStateMutable(const std::string& inNodePath)
{
    return mChoicePointNodeStates.at(inNodePath);
}

const HTNDecompositionChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeState(const std::string& inNodePath) const
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return ((It != mChoicePointNodeStates.end()) ? &It->second : nullptr);
}

HTNDecompositionChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return ((It != mChoicePointNodeStates.end()) ? &It->second : nullptr);
}

HTNNodeStep HTNDecompositionPrinterContext::GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const
{
    const HTNDecompositionNodeStateBase& NodeState = inIsChoicePoint
                                                         ? static_cast<const HTNDecompositionNodeStateBase&>(mChoicePointNodeStates.at(inNodePath))
                                                         : static_cast<const HTNDecompositionNodeStateBase&>(mNodeStates.at(inNodePath));
    const HTNNodeStep                    NodeStep  = NodeState.GetNodeStep();
    return NodeStep;
}

int32 HTNDecompositionPrinterContext::GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const
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
    default: {
        assert(false);
    }
    }

    return HTNDecompositionHelpers::kInvalidDecompositionStep;
}

bool HTNDecompositionPrinterContext::IsNodeOpen(const std::string& inNodePath, const int32 inDecompositionStep, const bool inIsChoicePoint) const
{
    if (inIsChoicePoint)
    {
        const auto It = mChoicePointNodeStates.find(inNodePath);
        if (It != mChoicePointNodeStates.end())
        {
            const HTNDecompositionChoicePointNodeState& NodeState = It->second;
            return NodeState.IsOpen(inDecompositionStep);
        }
    }
    else
    {
        const auto It = mNodeStates.find(inNodePath);
        if (It != mNodeStates.end())
        {
            const HTNDecompositionNodeState& NodeState = mNodeStates.at(inNodePath);
            return NodeState.IsOpen();
        }
    }

    return false;
}

bool HTNDecompositionPrinterContext::IsCurrentDecompositionStepValid() const
{
    return HTNDecompositionHelpers::IsDecompositionStepValid(mCurrentDecompositionStep);
}
#endif
