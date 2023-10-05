#include "Domain/Interpreter/HTNDecompositionContext.h"

namespace
{
bool PushSegmentToPath(const std::string& inSegment, std::string& ioPath)
{
    if (inSegment.empty())
    {
        return false;
    }

    if (!ioPath.empty())
    {
        ioPath.append(" ");
    }

    ioPath.append(inSegment);

    return true;
}

bool PopSegmentFromPath(std::string& ioPath)
{
    const std::size_t Index = ioPath.find_last_of(" ");
    if (Index == std::string::npos)
    {
        return false;
    }

    ioPath.erase(Index);

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

bool HTNDecompositionContext::PushNodeToCurrentNodePath(const std::string& inNodeID)
{
    return PushSegmentToPath(inNodeID, mCurrentNodePath);
}

bool HTNDecompositionContext::PopNodeFromCurrentNodePath()
{
    return PopSegmentFromPath(mCurrentNodePath);
}

bool HTNDecompositionContext::PushNodeToCurrentVariableScopePath(const std::string& inNodeID)
{
    return PushSegmentToPath(inNodeID, mCurrentVariableScopePath);
}

bool HTNDecompositionContext::PopNodeFromCurrentVariableScopePath()
{
    return PopSegmentFromPath(mCurrentVariableScopePath);
}

std::string HTNDecompositionContext::MakeCurrentVariablePath(const std::string& inVariableID) const
{
    std::string CurrentVariablePath = mCurrentVariableScopePath;
    PushSegmentToPath(inVariableID, CurrentVariablePath);

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
