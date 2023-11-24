// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

namespace HTNPathHelpers
{
HTN_NODISCARD bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath);
HTN_NODISCARD bool TryPopSegmentFromPath(std::string& ioPath);
HTN_NODISCARD bool FindLastPathSegment(const std::string& inPath, std::string& outSegment);
} // namespace HTNPathHelpers
