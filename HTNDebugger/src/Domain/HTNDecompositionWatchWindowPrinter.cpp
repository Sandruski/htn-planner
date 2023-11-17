#include "Domain/HTNDecompositionWatchWindowPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchWindowPrinter::Print(HTNDecompositionWatchWindowPrinterContext& ioDecompositionWatchWindowPrinterContext) const
{
    const HTNDecompositionNode& Node         = ioDecompositionWatchWindowPrinterContext.GetNode();
    const HTNNodeSnapshotDebug* NodeSnapshot = Node.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    constexpr ImGuiTableFlags WatchWindowTableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoSavedSettings;
    if (ImGui::BeginTable("WatchWindowTable", 2, WatchWindowTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = Node.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();

            const HTNAtom  Parameter               = GetNodeValue(*NodeParameter, ioDecompositionWatchWindowPrinterContext);
            constexpr bool ShouldDoubleQuoteString = false;

            // Parameter ID
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
            const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ParameterIDColor  = *Parameter.FindListElement(1);
            const ImVec4      ParameterIDImGuiColor =
                ImVec4(ParameterIDColor.FindListElement(0)->GetValue<float>(), ParameterIDColor.FindListElement(1)->GetValue<float>(),
                       ParameterIDColor.FindListElement(2)->GetValue<float>(), ParameterIDColor.FindListElement(3)->GetValue<float>());
            ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());

            // Parameter value
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterValue       = *Parameter.FindListElement(2);
            const std::string ParameterValueString = ParameterValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ParameterValueString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = Node.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();

            const HTNAtom  Argument                = GetNodeValue(*NodeArgument, ioDecompositionWatchWindowPrinterContext);
            constexpr bool ShouldDoubleQuoteString = false;

            // Argument ID
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());

            // Argument value
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentValue       = *Argument.FindListElement(2);
            const std::string ArgumentValueString = ArgumentValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ArgumentValueString.c_str());
        }

        // Print remaining variables
        const std::vector<std::string>&                 NodeVariablePaths = ioDecompositionWatchWindowPrinterContext.GetNodeVariablePaths();
        const std::unordered_map<std::string, HTNAtom>& Variables         = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
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

            std::string VariableID;
            const bool  FindVariableIDResult = HTNDecompositionHelpers::FindVariableID(VariablePath, VariableID);
            assert(FindVariableIDResult);
            const std::string VariableIDString = std::format("?{}", VariableID);
            const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
            ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

            // Variable value
            ImGui::TableNextColumn();

            const HTNAtom&    Variable                = VariablePair.second;
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string VariableString          = Variable.ToString(ShouldDoubleQuoteString);
            ImGui::Text(VariableString.c_str());
        }

        ImGui::EndTable();
    }
}
#endif
