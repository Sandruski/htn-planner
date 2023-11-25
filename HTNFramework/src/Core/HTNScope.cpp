// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNScope.h"

#include "HTNPathManager.h"

#include <cassert>

HTNScope::HTNScope(const std::string& inSegment, HTNPathManager& ioPathManager) : mPathManager(ioPathManager)
{
    mResult = ioPathManager.TryPushSegmentToPath(inSegment);
}

HTNScope::~HTNScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the current node from the path
    const bool Result = mPathManager.TryPopSegmentFromPath();
    assert(Result);
}
