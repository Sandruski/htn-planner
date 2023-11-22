// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionWatchTooltipPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchTooltipPrinter::Print(HTNDecompositionWatchTooltipPrinterContext& ioDecompositionWatchTooltipPrinterContext) const
{
    const HTNDecompositionTooltipMode TooltipMode = ioDecompositionWatchTooltipPrinterContext.GetTooltipMode();
    if (TooltipMode == HTNDecompositionTooltipMode::NONE)
    {
        return;
    }

    const HTNDecompositionNode& Node         = ioDecompositionWatchTooltipPrinterContext.GetNode();
    const HTNNodeSnapshotDebug* NodeSnapshot = Node.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    bool                                                                 ShouldPrint       = false;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters    = Node.GetNodeParameters();
    const bool                                                           HasNodeParameters = !NodeParameters.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeParameters;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments    = Node.GetNodeArguments();
    const bool                                                            HasNodeArguments = !NodeArguments.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeArguments;
    const std::unordered_map<std::string, HTNAtom>& Variables                              = NodeSnapshot->GetVariables().GetVariables();
    const bool                                      HasVariables                           = !Variables.empty();
    if (TooltipMode == HTNDecompositionTooltipMode::FULL)
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
                const HTNAtom         Parameter               = GetNodeValue(*NodeParameter, ioDecompositionWatchTooltipPrinterContext);
                static constexpr bool ShouldDoubleQuoteString = false;

                // Parameter ID
                const std::string ParameterID      = Parameter.FindListElement(0)->ToString(ShouldDoubleQuoteString);
                const HTNAtom&    ParameterIDColor = *Parameter.FindListElement(1);
                const ImVec4      ParameterIDImGuiColor =
                    ImVec4(ParameterIDColor.FindListElement(0)->GetValue<float>(), ParameterIDColor.FindListElement(1)->GetValue<float>(),
                           ParameterIDColor.FindListElement(2)->GetValue<float>(), ParameterIDColor.FindListElement(3)->GetValue<float>());
                ImGui::TextColored(ParameterIDImGuiColor, ParameterID.c_str());

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
                const HTNAtom         Argument                = GetNodeValue(*NodeArgument, ioDecompositionWatchTooltipPrinterContext);
                static constexpr bool ShouldDoubleQuoteString = false;

                // Argument ID
                const std::string ArgumentID      = Argument.FindListElement(0)->ToString(ShouldDoubleQuoteString);
                const HTNAtom&    ArgumentIDColor = *Argument.FindListElement(1);
                const ImVec4      ArgumentIDImGuiColor =
                    ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                           ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
                ImGui::TextColored(ArgumentIDImGuiColor, ArgumentID.c_str());

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

        if (TooltipMode == HTNDecompositionTooltipMode::FULL)
        {
            // Print remaining variables
            if (HasVariables)
            {
                const std::vector<std::string>& NodeVariablePaths = ioDecompositionWatchTooltipPrinterContext.GetNodeVariablePaths();
                for (const std::pair<const std::string, HTNAtom>& VariablePair : Variables)
                {
                    const std::string& VariablePath = VariablePair.first;
                    const auto         It           = std::find(NodeVariablePaths.begin(), NodeVariablePaths.end(), VariablePath);
                    if (It != NodeVariablePaths.end())
                    {
                        continue;
                    }

                    // Variable ID
                    const HTNPathHandler VariablePathHandler = HTNPathHandler(VariablePath);
                    std::string          VariableID;
                    const bool           FindLastPathSegmentResult = VariablePathHandler.FindLastPathSegment(VariableID);
                    assert(FindLastPathSegmentResult);
                    const std::string VariableIDString = std::format("?{}", VariableID);
                    const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
                    ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

                    // Variable value
                    const HTNAtom&        Variable                = VariablePair.second;
                    static constexpr bool ShouldDoubleQuoteString = true;
                    const std::string     VariableString          = Variable.ToString(ShouldDoubleQuoteString);
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
