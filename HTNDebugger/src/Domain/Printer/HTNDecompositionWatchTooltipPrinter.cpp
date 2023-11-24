// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionWatchTooltipPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeResult.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Domain/Printer/HTNDecompositionHelpers.h"
#include "Domain/Printer/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionWatchTooltipPrinter::Print(HTNDecompositionWatchTooltipPrinterContext& ioDecompositionWatchTooltipPrinterContext) const
{
    const HTNDecompositionTooltipMode TooltipMode = ioDecompositionWatchTooltipPrinterContext.GetTooltipMode();
    if (TooltipMode == HTNDecompositionTooltipMode::NONE)
    {
        return;
    }

    const HTNNodeInstance& NodeInstance = ioDecompositionWatchTooltipPrinterContext.GetNodeInstance();
    const HTNNodeResult*   NodeResult   = NodeInstance.GetNodeResult();
    if (!NodeResult)
    {
        return;
    }

    bool                                                                 ShouldPrint       = false;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters    = NodeInstance.GetNodeParameters();
    const bool                                                           HasNodeParameters = !NodeParameters.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeParameters;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments    = NodeInstance.GetNodeArguments();
    const bool                                                            HasNodeArguments = !NodeArguments.empty();
    ShouldPrint                                                                            = ShouldPrint || HasNodeArguments;
    const std::unordered_map<std::string, HTNAtom>& Variables                              = NodeResult->GetVariables().GetVariables();
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
                PrintColoredValueIDExpressionNode(NodeParameter, ioDecompositionWatchTooltipPrinterContext);
                ImGui::SameLine();

                PrintValueValueExpressionNode(NodeParameter, ioDecompositionWatchTooltipPrinterContext);
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
                PrintColoredValueIDExpressionNode(NodeArgument, ioDecompositionWatchTooltipPrinterContext);
                ImGui::SameLine();

                PrintValueValueExpressionNode(NodeArgument, ioDecompositionWatchTooltipPrinterContext);
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
                    ImGui::SameLine();

                    // Variable value
                    static constexpr bool ShouldDoubleQuoteString = true;
                    const std::string     VariableValueString     = VariablePair.second.ToString(ShouldDoubleQuoteString);
                    ImGui::Text(VariableValueString.c_str());
                    ImGui::SameLine();
                }

                ImGui::SameLine(-1.f);
            }
        }

        ImGui::EndTooltip();
    }
}
#endif
