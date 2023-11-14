#include "Domain/HTNDecompositionWatchTooltipPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchTooltipPrinter::Print(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode,
                                                const HTNDecompositionTooltipMode inTooltipMode)
{
    Reset(inDomainNode, inNode);

    HTNNodeVisitorContext Context;

    if (inTooltipMode == HTNDecompositionTooltipMode::NONE)
    {
        return;
    }

    const HTNNodeSnapshotDebug* NodeSnapshot = mNode->GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    bool                                                                 ShouldPrint       = false;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters    = mNode->GetNodeParameters();
    const bool                                                           HasNodeParameters = !NodeParameters.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeParameters;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments    = mNode->GetNodeArguments();
    const bool                                                            HasNodeArguments = !NodeArguments.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeArguments;
    const std::unordered_map<std::string, HTNAtom>& Variables                              = NodeSnapshot->GetVariables().GetVariables();
    const bool                                      HasVariables                           = !Variables.empty();
    if (inTooltipMode == HTNDecompositionTooltipMode::FULL)
    {
        ShouldPrint = ShouldPrint || HasVariables;
    }

    if (!ShouldPrint)
    {
        return;
    }

    if (ImGui::BeginTooltip())
    {
        // Print node parameters
        if (HasNodeParameters)
        {
            for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
            {
                const HTNAtom  Parameter               = GetNodeValue(*NodeParameter, Context);
                constexpr bool ShouldDoubleQuoteString = false;

                // Parameter ID
                const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
                const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
                const HTNAtom&    ParameterIDColor  = *Parameter.FindListElement(1);
                const ImVec4      ParameterIDImGuiColor =
                    ImVec4(ParameterIDColor.FindListElement(0)->GetValue<float>(), ParameterIDColor.FindListElement(1)->GetValue<float>(),
                           ParameterIDColor.FindListElement(2)->GetValue<float>(), ParameterIDColor.FindListElement(3)->GetValue<float>());
                ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());

                // Parameter value
                const HTNAtom&    ParameterValue       = *Parameter.FindListElement(2);
                const std::string ParameterValueString = ParameterValue.ToString(ShouldDoubleQuoteString);
                ImGui::SameLine();
                ImGui::Text(ParameterValueString.c_str());
                ImGui::SameLine();
            }

            ImGui::SameLine(-1.f);
            ImGui::Spacing();
        }

        // Print node arguments
        if (HasNodeArguments)
        {
            for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
            {
                const HTNAtom  Argument                = GetNodeValue(*NodeArgument, Context);
                constexpr bool ShouldDoubleQuoteString = false;

                // Argument ID
                const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
                const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
                const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
                const ImVec4      ArgumentIDImGuiColor =
                    ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                           ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
                ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());

                // Argument value
                const HTNAtom&    ArgumentValue       = *Argument.FindListElement(2);
                const std::string ArgumentValueString = ArgumentValue.ToString(ShouldDoubleQuoteString);
                ImGui::SameLine();
                ImGui::Text(ArgumentValueString.c_str());
                ImGui::SameLine();
            }

            ImGui::SameLine(-1.f);
            ImGui::Spacing();
        }

        if (inTooltipMode == HTNDecompositionTooltipMode::FULL)
        {
            // Print remaining variables
            if (HasVariables)
            {
                for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
                {
                    const std::string& VariablePath = VariablePair.first;
                    const auto         It           = std::find(mNodeVariablePaths.begin(), mNodeVariablePaths.end(), VariablePath);
                    if (It != mNodeVariablePaths.end())
                    {
                        continue;
                    }

                    // Variable ID
                    std::string VariableID;
                    const bool  FindVariableIDResult = HTNDecompositionHelpers::FindVariableID(VariablePath, VariableID);
                    assert(FindVariableIDResult);
                    const std::string VariableIDString = std::format("?{}", VariableID);
                    const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
                    ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

                    // Variable value
                    const HTNAtom&    Variable                = VariablePair.second;
                    constexpr bool    ShouldDoubleQuoteString = true;
                    const std::string VariableString          = Variable.ToString(ShouldDoubleQuoteString);
                    ImGui::SameLine();
                    ImGui::Text(VariableString.c_str());
                    ImGui::SameLine();
                }

                ImGui::SameLine(-1.f);
            }
        }

        ImGui::EndTooltip();
    }
}
#endif
