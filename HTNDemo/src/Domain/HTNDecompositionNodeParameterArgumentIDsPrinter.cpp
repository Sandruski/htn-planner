#include "Domain/HTNDecompositionNodeParameterArgumentIDsPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

HTNDecompositionNodeParameterArgumentIDsPrinter::HTNDecompositionNodeParameterArgumentIDsPrinter(const HTNDecompositionNode& inNode) : mNode(inNode)
{
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = mNode.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = mNode.GetNodeArguments();
    mNodeVariablesPaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

void HTNDecompositionNodeParameterArgumentIDsPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("NodeArgumentsIDs", 1, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = mNode.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Parameter               = GetNodeValue(*NodeParameter);
            constexpr bool    ShouldDoubleQuoteString = false;
            const std::string ParametertString        = Parameter.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ParameterColor          = HTNImGuiHelpers::kParametersColor;
            ImGui::TextColored(ParameterColor, ParametertString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = mNode.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Argument                = GetNodeValue(*NodeArgument);
            constexpr bool    ShouldDoubleQuoteString = false;
            const std::string ArgumentString          = Argument.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ArgumentColor           = HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }

        // Print remaining variables
        const std::unordered_map<std::string, HTNAtom>& Variables = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
        {
            const std::string& VariablePath = VariablePair.first;
            const auto         It           = std::find(mNodeVariablesPaths.begin(), mNodeVariablesPaths.end(), VariablePath);
            if (It != mNodeVariablesPaths.end())
            {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            std::string VariableID;
            const bool  Result = HTNDecompositionHelpers::FindVariableID(VariablePath, VariableID);
            assert(Result);
            const ImVec4 VariableColor =
                HTNDecompositionHelpers::IsParameter(VariableID) ? HTNImGuiHelpers::kParametersColor : HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(VariableColor, VariableID.c_str());
        }

        ImGui::EndTable();
    }
}

HTNAtom HTNDecompositionNodeParameterArgumentIDsPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    const HTNAtom&     Value                   = inLiteralExpressionNode.GetValue();
    const bool         ShouldDoubleQuoteString = false;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionNodeParameterArgumentIDsPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    const std::string& VariableID                = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
    std::string        VariablePath;
    const bool         Result = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
    assert(Result);
    mNodeVariablesPaths.emplace_back(VariablePath);

    const std::string VariableValueString = inVariableExpressionNode.ToString();
    return std::format("?{}", VariableValueString);
}

HTNAtom HTNDecompositionNodeParameterArgumentIDsPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    const std::string ConstantValueString = inConstantExpressionNode.ToString();
    return std::format("@{}", ConstantValueString);
}
#endif
