#include "Domain/Interpreter/HTNDecompositionContextHelpers.h"

namespace HTNDecompositionContextHelpers
{
bool FindVariableID(const std::string& inVariablePath, std::string& outVariableID)
{
    const std::size_t Index = inVariablePath.find_last_of(kPathSegmentSeparator);
    if (Index == std::string::npos)
    {
        return false;
    }

    outVariableID = inVariablePath.substr(Index + 1);

    return true;
}

bool MakeVariablePath(const std::string& inVariableID, const std::string& inVariableScopePath, std::string& outVariablePath)
{
    std::string VariablePath = inVariableScopePath;
    if (!HTNDecompositionContextHelpers::TryPushSegmentToPath(inVariableID, VariablePath))
    {
        return false;
    }

    outVariablePath = VariablePath;

    return true;
}

bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath)
{
    if (inSegment.empty())
    {
        return false;
    }

    if (!ioPath.empty())
    {
        ioPath.append(kPathSegmentSeparator);
    }

    ioPath.append(inSegment);

    return true;
}

bool TryPopSegmentFromPath(std::string& ioPath)
{
    if (ioPath.empty())
    {
        return false;
    }

    const std::size_t Index = ioPath.find_last_of(kPathSegmentSeparator);
    if (Index != std::string::npos)
    {
        ioPath.erase(Index);
    }
    else
    {
        ioPath.clear();
    }

    return true;
}
} // namespace HTNDecompositionContextHelpers
