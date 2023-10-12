#include "Domain/Interpreter/HTNDecompositionContext.h"

#include "Domain/Interpreter/HTNDecompositionContextHelpers.h"

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionHistory.empty())
    {
        return false;
    }

    mCurrentDecomposition = mDecompositionHistory.back();
    mDecompositionHistory.pop_back();

#ifdef HTN_DEBUG
    mDecompositionSnapshot.IncrementDecompositionStep();
#endif

    return true;
}

bool HTNDecompositionContext::TryPushNodeToCurrentNodePath(const std::string& inNodeID)
{
    return HTNDecompositionContextHelpers::TryPushSegmentToPath(inNodeID, mCurrentNodePath);
}

bool HTNDecompositionContext::TryPopNodeFromCurrentNodePath()
{
    return HTNDecompositionContextHelpers::TryPopSegmentFromPath(mCurrentNodePath);
}

bool HTNDecompositionContext::TryPushNodeToCurrentVariableScopePath(const std::string& inNodeID)
{
    return HTNDecompositionContextHelpers::TryPushSegmentToPath(inNodeID, mCurrentVariableScopePath);
}

bool HTNDecompositionContext::TryPopNodeFromCurrentVariableScopePath()
{
    return HTNDecompositionContextHelpers::TryPopSegmentFromPath(mCurrentVariableScopePath);
}

bool HTNDecompositionContext::MakeCurrentVariablePath(const std::string& inVariableID, std::string& outVariablePath) const
{
    return HTNDecompositionContextHelpers::MakeVariablePath(inVariableID, mCurrentVariableScopePath, outVariablePath);
}

#ifdef HTN_DEBUG
void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(Variables);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot);
}
#endif
