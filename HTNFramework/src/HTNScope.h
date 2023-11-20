#pragma once

#include "HTNPlannerMinimal.h"
#include "HTNUncopyable.h"

#include <string>

class HTNPathHandler;

/**
 * Helper that uses RAII to automatically push/pop a node to/from a node path
 */
class HTNScope : private HTNUncopyable
{
public:
    explicit HTNScope(const std::string& inSegment, HTNPathHandler& ioPathHandler);
    virtual ~HTNScope();

protected:
    HTNPathHandler& mPathHandler;
    bool            mResult = false;
};
