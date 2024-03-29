// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDecompositionWatchWindowPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeSnapshot.h"
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Domain/Printer/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchWindowPrinter::Print(HTNDecompositionWatchWindowPrinterContext& ioDecompositionWatchWindowPrinterContext) const
{
    const HTNNodeInstance& NodeInstance = ioDecompositionWatchWindowPrinterContext.GetNodeInstance();
    const HTNNodeSnapshot* NodeSnapshot = NodeInstance.GetNodeSnapshot();
    if (!NodeSnapshot)
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
        const std::vector<std::string>& VariablePaths    = ioDecompositionWatchWindowPrinterContext.GetVariablePaths();
        const auto                      ItBegin          = VariablePaths.cbegin();
        const auto                      ItEnd            = VariablePaths.cend();
        const HTNVariablesManager&      VariablesManager = NodeSnapshot->GetVariablesManager();
        const HTNVariables&             Variables        = VariablesManager.GetVariables();
        for (const HTNVariable& Variable : Variables)
        {
            const std::string& VariablePath = Variable.first;
            const auto         It           = std::find(ItBegin, ItEnd, VariablePath);
            if (It != ItEnd)
            {
                continue;
            }

            ImGui::TableNextRow();

            // Variable ID
            ImGui::TableNextColumn();

            const HTNPathManager VariablePathManager = HTNPathManager(VariablePath);
            std::string          VariableID;
            const bool           FindLastPathSegmentResult = VariablePathManager.FindLastPathSegment(VariableID);
            assert(FindLastPathSegmentResult);
            const std::string VariableIDString = std::format("?{}", VariableID);
            const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
            ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

            // Variable value
            ImGui::TableNextColumn();

            static constexpr bool ShouldDoubleQuoteString = true;
            const HTNAtom&        VariableValue           = Variable.second;
            const std::string     VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(VariableValueString.c_str());
        }

        ImGui::EndTable();
    }
}
#endif
