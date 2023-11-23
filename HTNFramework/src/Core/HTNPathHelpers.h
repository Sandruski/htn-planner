// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

namespace HTNPathHelpers
{
bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath);
bool TryPopSegmentFromPath(std::string& ioPath);
bool FindLastPathSegment(const std::string& inPath, std::string& outSegment);
} // namespace HTNPathHelpers
