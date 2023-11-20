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

bool IsParameter(const std::string& inVariableID)
{
    return (inVariableID.find(kInputParameterPrefix) != std::string::npos) || (inVariableID.find(kOutputParameterPrefix) != std::string::npos) ||
           (inVariableID.find(kInputOutputParameterPrefix) != std::string::npos);
}

bool IsAnyArgument(const std::string& inVariableID)
{
    return (inVariableID.find(kAnyArgumentPrefix) != std::string::npos);
}
} // namespace HTNDecompositionHelpers
