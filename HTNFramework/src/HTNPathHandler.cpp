// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNPathHandler.h"

#include "HTNPathHelpers.h"

HTNPathHandler::HTNPathHandler(const std::string& inPath) : mPath(inPath)
{
}

bool HTNPathHandler::TryPushSegmentToPath(const std::string& inSegment)
{
    return HTNPathHelpers::TryPushSegmentToPath(inSegment, mPath);
}

bool HTNPathHandler::TryPopSegmentFromPath()
{
    return HTNPathHelpers::TryPopSegmentFromPath(mPath);
}

bool HTNPathHandler::FindLastPathSegment(std::string& outSegment) const
{
    return HTNPathHelpers::FindLastPathSegment(mPath, outSegment);
}
