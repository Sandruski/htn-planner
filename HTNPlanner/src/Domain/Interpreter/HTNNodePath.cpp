#include "Domain/Interpreter/HTNNodePath.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"

bool HTNNodePath::TryPushNodeToNodePath(const std::string& inNodeID)
{
    return HTNDecompositionHelpers::TryPushSegmentToPath(inNodeID, mNodePath);
}

bool HTNNodePath::TryPopNodeFromNodePath()
{
    return HTNDecompositionHelpers::TryPopSegmentFromPath(mNodePath);
}
