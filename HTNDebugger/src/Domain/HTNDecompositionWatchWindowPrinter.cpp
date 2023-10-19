#include "Domain/HTNDecompositionWatchWindowPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

HTNDecompositionWatchWindowPrinter::HTNDecompositionWatchWindowPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                       const HTNDecompositionNode&                 inNode)
    : HTNDecompositionWatchPrinterBase(inDomainNode, inNode)
{
}

void HTNDecompositionWatchWindowPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("WatchWindowTable", 2, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = mNode.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();

            const HTNAtom  Parameter               = GetNodeValue(*NodeParameter);
            constexpr bool ShouldDoubleQuoteString = false;

            // Parameter ID
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
            const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ParameterIDColor  = HTNImGuiHelpers::kParameterColor;
            ImGui::TextColored(ParameterIDColor, ParameterIDString.c_str());

            // Parameter value
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterValue       = *Parameter.FindListElement(1);
            const std::string ParameterValueString = ParameterValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ParameterValueString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = mNode.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();

            const HTNAtom  Argument                = GetNodeValue(*NodeArgument);
            constexpr bool ShouldDoubleQuoteString = false;

            // Argument ID
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const ImVec4      ArgumentIDColor =
                HTNDecompositionHelpers::IsParameter(ArgumentIDString) ? HTNImGuiHelpers::kParameterColor : HTNImGuiHelpers::kArgumentColor;
            ImGui::TextColored(ArgumentIDColor, ArgumentIDString.c_str());

            // Argument value
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentValue       = *Argument.FindListElement(1);
            const std::string ArgumentValueString = ArgumentValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ArgumentValueString.c_str());
        }

        // Print remaining variables
        const std::unordered_map<std::string, HTNAtom>& Variables = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
        {
            const std::string& VariablePath = VariablePair.first;
            const auto         It           = std::find(mNodeVariablePaths.begin(), mNodeVariablePaths.end(), VariablePath);
            if (It != mNodeVariablePaths.end())
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
            const ImVec4      VariableIDColor =
                HTNDecompositionHelpers::IsParameter(VariableID) ? HTNImGuiHelpers::kParameterColor : HTNImGuiHelpers::kArgumentColor;
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
