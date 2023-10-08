#pragma once

#include <string>

namespace HTNDecompositionContextHelpers
{
bool FindVariableID(const std::string& inVariablePath, std::string& outVariableID);

const std::string kPathSegmentSeparator = " ";
} // namespace HTNDecompositionContextHelpers

namespace HTNDecompositionContextHelpers
{
inline bool FindVariableID(const std::string& inVariablePath, std::string& outVariableID)
{
    const std::size_t Index = inVariablePath.find_last_of(kPathSegmentSeparator);
    if (Index == std::string::npos)
    {
        return false;
    }

    outVariableID = inVariablePath.substr(Index + 1);

    return true;
}
} // namespace HTNDecompositionContextHelpers
