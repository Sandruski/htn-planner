// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

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

bool IsInputParameter(const std::string& inVariableID)
{
    return (inVariableID.find(kInputParameterPrefix) != std::string::npos);
}

bool IsOutputParameter(const std::string& inVariableID)
{
    return (inVariableID.find(kOutputParameterPrefix) != std::string::npos);
}

bool IsInputOutputParameter(const std::string& inVariableID)
{
    return (inVariableID.find(kInputOutputParameterPrefix) != std::string::npos);
}

bool IsParameter(const std::string& inVariableID)
{
    return (IsInputParameter(inVariableID) || IsOutputParameter(inVariableID) || IsInputOutputParameter(inVariableID));
}

bool IsAnyArgument(const std::string& inVariableID)
{
    return (inVariableID.find(kAnyArgumentPrefix) != std::string::npos);
}
} // namespace HTNDecompositionHelpers
