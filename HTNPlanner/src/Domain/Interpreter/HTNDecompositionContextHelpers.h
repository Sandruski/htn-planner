#pragma once

#include <string>

namespace HTNDecompositionContextHelpers
{
bool FindVariableID(const std::string& inVariablePath, std::string& outVariableID);

bool MakeVariablePath(const std::string& inVariableID, const std::string& inVariableScopePath, std::string& outVariablePath);

bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath);
bool TryPopSegmentFromPath(std::string& ioPath);

const std::string kPathSegmentSeparator = " ";
} // namespace HTNDecompositionContextHelpers
