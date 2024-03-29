// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDecompositionPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionSnapshot.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "Domain/Printer/HTNDecompositionHelpers.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "Domain/Printer/HTNNodeState.h"

HTNDecompositionPrinterContext::HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                               const std::string&                          inEntryPointID,
                                                               const HTNDecompositionSnapshot&             inDecompositionSnapshot,
                                                               const bool                                  inIsDecompositionSuccessful,
                                                               const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreImGuiState,
                                                               HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                                               HTNNodeInstance& ioSelectedNodeInstance)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot),
      mIsDecompositionSuccessful(inIsDecompositionSuccessful), mTooltipMode(inTooltipMode), mShouldIgnoreImGuiState(inShouldIgnoreImGuiState),
      mNodeStates(ioNodeStates), mChoicePointNodeStates(ioChoicePointNodeStates), mSelectedNodeInstance(ioSelectedNodeInstance)
{
}

void HTNDecompositionPrinterContext::SelectNodeInstance(const HTNNodeInstance& inNodeInstance)
{
    mSelectedNodeInstance = inNodeInstance;
}

bool HTNDecompositionPrinterContext::IsNodeSelected(const std::string& inNodeLabel) const
{
    const std::string& NodeLabel = mSelectedNodeInstance.GetNodeLabel();
    return inNodeLabel == NodeLabel;
}

void HTNDecompositionPrinterContext::AddNodeState(const std::string& inNodePath, const HTNNodeState& inNodeState)
{
    mNodeStates[inNodePath] = inNodeState;
}

const HTNNodeState& HTNDecompositionPrinterContext::GetNodeState(const std::string& inNodePath) const
{
    return mNodeStates.at(inNodePath);
}

HTNNodeState& HTNDecompositionPrinterContext::GetNodeStateMutable(const std::string& inNodePath)
{
    return mNodeStates.at(inNodePath);
}

const HTNNodeState* HTNDecompositionPrinterContext::FindNodeState(const std::string& inNodePath) const
{
    const auto It = mNodeStates.find(inNodePath);
    return (It != mNodeStates.cend()) ? &It->second : nullptr;
}

HTNNodeState* HTNDecompositionPrinterContext::FindNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mNodeStates.find(inNodePath);
    return (It != mNodeStates.cend()) ? &It->second : nullptr;
}

void HTNDecompositionPrinterContext::AddChoicePointNodeState(const std::string& inNodePath, const HTNChoicePointNodeState& inChoicePointNodeState)
{
    mChoicePointNodeStates[inNodePath] = inChoicePointNodeState;
}

const HTNChoicePointNodeState& HTNDecompositionPrinterContext::GetChoicePointNodeState(const std::string& inNodePath) const
{
    return mChoicePointNodeStates.at(inNodePath);
}

HTNChoicePointNodeState& HTNDecompositionPrinterContext::GetChoicePointNodeStateMutable(const std::string& inNodePath)
{
    return mChoicePointNodeStates.at(inNodePath);
}

const HTNChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeState(const std::string& inNodePath) const
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return (It != mChoicePointNodeStates.cend()) ? &It->second : nullptr;
}

HTNChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return (It != mChoicePointNodeStates.cend()) ? &It->second : nullptr;
}

HTNNodeStep HTNDecompositionPrinterContext::GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const
{
    const HTNNodeStateBase& NodeState = inIsChoicePoint ? static_cast<const HTNNodeStateBase&>(mChoicePointNodeStates.at(inNodePath))
                                                        : static_cast<const HTNNodeStateBase&>(mNodeStates.at(inNodePath));
    const HTNNodeStep       NodeStep  = NodeState.GetNodeStep();
    return NodeStep;
}

int32 HTNDecompositionPrinterContext::GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const
{
    const HTNNodeStateBase& NodeState = inIsChoicePoint ? static_cast<const HTNNodeStateBase&>(mChoicePointNodeStates.at(inNodePath))
                                                        : static_cast<const HTNNodeStateBase&>(mNodeStates.at(inNodePath));

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
        if (It != mChoicePointNodeStates.cend())
        {
            const HTNChoicePointNodeState& NodeState = It->second;
            return NodeState.IsOpen(inDecompositionStep);
        }
    }
    else
    {
        const auto It = mNodeStates.find(inNodePath);
        if (It != mNodeStates.cend())
        {
            const HTNNodeState& NodeState = mNodeStates.at(inNodePath);
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
