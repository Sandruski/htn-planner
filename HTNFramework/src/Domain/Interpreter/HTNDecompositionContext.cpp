// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNDecompositionContext.h"

HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState, const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                 const std::string& inEntryPointID)
    : mWorldState(inWorldState), mDomainNode(inDomainNode), mEntryPointID(inEntryPointID)
{
}

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionHistory.empty())
    {
        return false;
    }

    mCurrentDecomposition = mDecompositionHistory.back();
    mDecompositionHistory.pop_back();

#ifdef HTN_DEBUG_DECOMPOSITION
    mDecompositionSnapshot.IncrementDecompositionStep();
#endif

    return true;
}

#ifdef HTN_DEBUG_DECOMPOSITION
void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const bool inResult, const HTNNodeStep inNodeStep,
                                                 const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(Variables, inResult);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}

void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(Variables);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}
#endif
