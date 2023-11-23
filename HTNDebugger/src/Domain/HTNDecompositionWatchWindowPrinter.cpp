// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionWatchWindowPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
enum HTNVariableExpressionNodeResult : uint8
{
    ID_STRING,
    ID_COLOR,
    VALUE_STRING
};

enum HTNVariableExpressionNodeColorResult : uint8
{
    X,
    Y,
    Z,
    W
};
} // namespace

void HTNDecompositionWatchWindowPrinter::Print(HTNDecompositionWatchWindowPrinterContext& ioDecompositionWatchWindowPrinterContext) const
{
    const HTNDecompositionNode& Node         = ioDecompositionWatchWindowPrinterContext.GetNode();
    const HTNNodeSnapshotDebug* NodeSnapshot = Node.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    static constexpr ImGuiTableFlags WatchWindowTableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoSavedSettings;
    if (ImGui::BeginTable("WatchWindowTable", 2, WatchWindowTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = Node.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();

            const HTNAtom Parameter = GetNodeValue(*NodeParameter, ioDecompositionWatchWindowPrinterContext);

            // Parameter ID
            ImGui::TableNextColumn();

            const std::string ParameterIDString = Parameter.GetListElement(HTNVariableExpressionNodeResult::ID_STRING).GetValue<std::string>();
            const HTNAtom&    ParameterIDColor  = Parameter.GetListElement(HTNVariableExpressionNodeResult::ID_COLOR);
            const ImVec4 ParameterIDImGuiColor  = ImVec4(ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::X).GetValue<float>(),
                                                         ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Y).GetValue<float>(),
                                                         ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Z).GetValue<float>(),
                                                         ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::W).GetValue<float>());
            ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());

            // Parameter value
            ImGui::TableNextColumn();

            const std::string ParameterValueString = Parameter.GetListElement(HTNVariableExpressionNodeResult::VALUE_STRING).GetValue<std::string>();
            ImGui::Text(ParameterValueString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = Node.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();

            const HTNAtom Argument = GetNodeValue(*NodeArgument, ioDecompositionWatchWindowPrinterContext);

            // Argument ID
            ImGui::TableNextColumn();

            const std::string ArgumentIDString     = Argument.GetListElement(HTNVariableExpressionNodeResult::ID_STRING).GetValue<std::string>();
            const HTNAtom&    ArgumentIDColor      = Argument.GetListElement(HTNVariableExpressionNodeResult::ID_COLOR);
            const ImVec4      ArgumentIDImGuiColor = ImVec4(ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::X).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Y).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Z).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::W).GetValue<float>());
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());

            // Argument value
            ImGui::TableNextColumn();

            const std::string ArgumentValueString = Argument.GetListElement(HTNVariableExpressionNodeResult::VALUE_STRING).GetValue<std::string>();
            ImGui::Text(ArgumentValueString.c_str());
        }

        // Print remaining variables
        const std::vector<std::string>&                 NodeVariablePaths = ioDecompositionWatchWindowPrinterContext.GetNodeVariablePaths();
        const std::unordered_map<std::string, HTNAtom>& Variables         = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<const std::string, HTNAtom>& VariablePair : Variables)
        {
            const std::string& VariablePath = VariablePair.first;
            const auto         It           = std::find(NodeVariablePaths.begin(), NodeVariablePaths.end(), VariablePath);
            if (It != NodeVariablePaths.end())
            {
                continue;
            }

            ImGui::TableNextRow();

            // Variable ID
            ImGui::TableNextColumn();

            const HTNPathHandler VariablePathHandler = HTNPathHandler(VariablePath);
            std::string          VariableID;
            const bool           FindLastPathSegmentResult = VariablePathHandler.FindLastPathSegment(VariableID);
            assert(FindLastPathSegmentResult);
            const std::string VariableIDString = std::format("?{}", VariableID);
            const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
            ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

            // Variable value
            ImGui::TableNextColumn();

            static constexpr bool ShouldDoubleQuoteString = true;
            const std::string     VariableValueString     = VariablePair.second.ToString(ShouldDoubleQuoteString);
            ImGui::Text(VariableValueString.c_str());
        }

        ImGui::EndTable();
    }
}
#endif
