#include "Domain/Interpreter/HTNDecompositionHelpers.h"

#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

namespace HTNDecompositionHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID)
{
    static constexpr bool IsTopLevel = true;
    return std::make_shared<HTNCompoundTaskNode>(std::make_shared<const HTNIdentifierExpressionNode>(inEntryPointID),
                                                 std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>(), kTopLevelCompoundTaskNodeID,
                                                 IsTopLevel);
}

bool MakeVariablePath(const std::string& inVariableID, const std::string& inVariableScopeNodePath, std::string& outVariablePath)
{
    std::string VariablePath = inVariableScopeNodePath;
    if (!TryPushSegmentToPath(inVariableID, VariablePath))
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
} // namespace HTNDecompositionHelpers
