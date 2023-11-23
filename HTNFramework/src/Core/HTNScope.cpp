// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNScope.h"

#include "HTNPathHandler.h"

#include <cassert>

HTNScope::HTNScope(const std::string& inSegment, HTNPathHandler& ioPathHandler) : mPathHandler(ioPathHandler)
{
    mResult = ioPathHandler.TryPushSegmentToPath(inSegment);
}

HTNScope::~HTNScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the current node from the path
    const bool Result = mPathHandler.TryPopSegmentFromPath();
    assert(Result);
}
