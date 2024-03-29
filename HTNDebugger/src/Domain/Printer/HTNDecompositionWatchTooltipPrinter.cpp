// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDecompositionWatchTooltipPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeSnapshot.h"
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
    const HTNNodeSnapshot* NodeSnapshot = NodeInstance.GetNodeSnapshot();
    if (!NodeSnapshot)
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
    const HTNVariablesManager& VariablesManager                                            = NodeSnapshot->GetVariablesManager();
    const HTNVariables&        Variables                                                   = VariablesManager.GetVariables();
    const bool                 HasVariables                                                = !Variables.empty();
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
                const std::vector<std::string>& VariablePaths = ioDecompositionWatchTooltipPrinterContext.GetVariablePaths();
                const auto                      ItBegin       = VariablePaths.cbegin();
                const auto                      ItEnd         = VariablePaths.cend();
                for (const HTNVariable& Variable : Variables)
                {
                    const std::string& VariablePath = Variable.first;
                    const auto         It           = std::find(ItBegin, ItEnd, VariablePath);
                    if (It != ItEnd)
                    {
                        continue;
                    }

                    // Variable ID
                    const HTNPathManager VariablePathManager = HTNPathManager(VariablePath);
                    std::string          VariableID;
                    const bool           FindLastPathSegmentResult = VariablePathManager.FindLastPathSegment(VariableID);
                    assert(FindLastPathSegmentResult);
                    const std::string VariableIDString = std::format("?{}", VariableID);
                    const ImVec4      VariableIDColor  = HTNImGuiHelpers::GetVariableColor(VariableID);
                    ImGui::TextColored(VariableIDColor, VariableIDString.c_str());
                    ImGui::SameLine();

                    // Variable value
                    static constexpr bool ShouldDoubleQuoteString = true;
                    const HTNAtom&        VariableValue           = Variable.second;
                    const std::string     VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
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
