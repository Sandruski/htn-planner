#pragma once

#include "Helpers/HTNUncopyable.h"

#include <string>

class HTNNodePath;

/**
 * Helper that uses RAII to automatically push/pop a node to/from a node path
 */
class HTNNodeScope : private HTNUncopyable
{
public:
    explicit HTNNodeScope(const std::string& inNodeID, HTNNodePath& ioNodePath);
    virtual ~HTNNodeScope();

protected:
    HTNNodePath& mNodePath;
    bool         mResult = false;
};
