#include "Domain/Interpreter/HTNDecompositionContext.h"

#include "Domain/Interpreter/HTNDecompositionContextHelpers.h"

namespace
{
bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath)
{
    if (inSegment.empty())
    {
        return false;
    }

    if (!ioPath.empty())
    {
        ioPath.append(HTNDecompositionContextHelpers::kPathSegmentSeparator);
    }

    ioPath.append(inSegment);

    return true;
}

bool TryPopSegmentFromPath(std::string& ioPath)
{
    if (ioPath.empty())
    {
        return false;
    }

    const std::size_t Index = ioPath.find_last_of(HTNDecompositionContextHelpers::kPathSegmentSeparator);
    if (Index != std::string::npos)
    {
        ioPath.erase(Index);
    }
    else
    {
        ioPath.clear();
    }

    return true;
}
} // namespace

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
    return TryPushSegmentToPath(inNodeID, mCurrentNodePath);
}

bool HTNDecompositionContext::TryPopNodeFromCurrentNodePath()
{
    return TryPopSegmentFromPath(mCurrentNodePath);
}

bool HTNDecompositionContext::TryPushNodeToCurrentVariableScopePath(const std::string& inNodeID)
{
    return TryPushSegmentToPath(inNodeID, mCurrentVariableScopePath);
}

bool HTNDecompositionContext::TryPopNodeFromCurrentVariableScopePath()
{
    return TryPopSegmentFromPath(mCurrentVariableScopePath);
}

std::string HTNDecompositionContext::MakeCurrentVariablePath(const std::string& inVariableID) const
{
    std::string CurrentVariablePath = mCurrentVariableScopePath;
    TryPushSegmentToPath(inVariableID, CurrentVariablePath);

    return CurrentVariablePath;
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
