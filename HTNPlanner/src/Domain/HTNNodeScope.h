#pragma once

#include <string>

class HTNNodePath;

/**
 * Helper that uses RAII to automatically push/pop a node to/from a node path
 */
class HTNNodeScope
{
public:
    HTNNodeScope(HTNNodePath& outNodePath, const std::string& inNodeID);
    virtual ~HTNNodeScope();

protected:
    HTNNodePath& mNodePath;
    bool         mResult = false;
};
