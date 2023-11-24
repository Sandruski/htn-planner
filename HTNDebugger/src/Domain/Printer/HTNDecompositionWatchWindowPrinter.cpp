// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionWatchWindowPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeResult.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Domain/Printer/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchWindowPrinter::Print(HTNDecompositionWatchWindowPrinterContext& ioDecompositionWatchWindowPrinterContext) const
{
    const HTNNodeInstance& NodeInstance = ioDecompositionWatchWindowPrinterContext.GetNodeInstance();
    const HTNNodeResult*   NodeResult   = NodeInstance.GetNodeResult();
    if (!NodeResult)
    {
        return;
    }

    static constexpr ImGuiTableFlags WatchWindowTableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoSavedSettings;
    if (ImGui::BeginTable("WatchWindowTable", 2, WatchWindowTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = NodeInstance.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();

            // Parameter ID
            ImGui::TableNextColumn();
            PrintColoredValueIDExpressionNode(NodeParameter, ioDecompositionWatchWindowPrinterContext);

            // Parameter value
            ImGui::TableNextColumn();
            PrintValueValueExpressionNode(NodeParameter, ioDecompositionWatchWindowPrinterContext);
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = NodeInstance.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();

            // Argument ID
            ImGui::TableNextColumn();
            PrintColoredValueIDExpressionNode(NodeArgument, ioDecompositionWatchWindowPrinterContext);

            // Argument value
            ImGui::TableNextColumn();
            PrintValueValueExpressionNode(NodeArgument, ioDecompositionWatchWindowPrinterContext);
        }

        // Print remaining variables
        const std::vector<std::string>&                 NodeVariablePaths = ioDecompositionWatchWindowPrinterContext.GetNodeVariablePaths();
        const std::unordered_map<std::string, HTNAtom>& Variables         = NodeResult->GetVariables().GetVariables();
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
