#include "Domain/Interpreter/HTNNodeScope.h"

#include "Domain/Interpreter/HTNNodePath.h"

HTNNodeScope::HTNNodeScope(HTNNodePath& outNodePath, const std::string& inNodeID) : mNodePath(outNodePath)
{
    mResult = mNodePath.TryPushNodeToNodePath(inNodeID);
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
