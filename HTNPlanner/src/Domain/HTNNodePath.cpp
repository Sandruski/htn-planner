#include "Domain/HTNNodePath.h"

#include "Domain/HTNDomainHelpers.h"

bool HTNNodePath::TryPushNodeToNodePath(const std::string& inNodeID)
{
    return HTNDomainHelpers::TryPushSegmentToPath(inNodeID, mNodePath);
}

bool HTNNodePath::TryPopNodeFromNodePath()
{
    return HTNDomainHelpers::TryPopSegmentFromPath(mNodePath);
}
