// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDomainHelpers.h"

#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

namespace HTNDomainHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID)
{
    static constexpr uint32 TopLevelCompoundTaskNodeID = 0;
    static constexpr bool   IsTopLevel                 = true;
    return std::make_shared<HTNCompoundTaskNode>(std::make_shared<const HTNIdentifierExpressionNode>(inEntryPointID),
                                                 std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>(), TopLevelCompoundTaskNodeID,
                                                 IsTopLevel);
}

bool IsInputParameter(const std::string& inVariableID)
{
    static const std::string InputParameterPrefix = "inp_";
    return (inVariableID.find(InputParameterPrefix) != std::string::npos);
}

bool IsOutputParameter(const std::string& inVariableID)
{
    static const std::string OutputParameterPrefix = "out_";
    return (inVariableID.find(OutputParameterPrefix) != std::string::npos);
}

bool IsInputOutputParameter(const std::string& inVariableID)
{
    static const std::string InputOutputParameterPrefix = "io_";
    return (inVariableID.find(InputOutputParameterPrefix) != std::string::npos);
}

bool IsParameter(const std::string& inVariableID)
{
    return (IsInputParameter(inVariableID) || IsOutputParameter(inVariableID) || IsInputOutputParameter(inVariableID));
}

bool IsAnyArgument(const std::string& inVariableID)
{
    static const std::string AnyArgumentPrefix = "any_";
    return (inVariableID.find(AnyArgumentPrefix) != std::string::npos);
}
} // namespace HTNDomainHelpers
