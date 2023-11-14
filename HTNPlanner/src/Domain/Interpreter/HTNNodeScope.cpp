#include "Domain/Interpreter/HTNNodeScope.h"

#include "Domain/Interpreter/HTNNodePath.h"

HTNNodeScope::HTNNodeScope(const std::string& inNodeID, HTNNodePath& ioNodePath) : mNodePath(ioNodePath)
{
    mResult = ioNodePath.TryPushNodeToNodePath(inNodeID);
}

HTNNodeScope::~HTNNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the current node from the path
    const bool Result = mNodePath.TryPopNodeFromNodePath();
    assert(Result);
}
