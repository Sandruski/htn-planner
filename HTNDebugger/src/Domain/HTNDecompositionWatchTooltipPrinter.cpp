// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionWatchTooltipPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Interpreter/HTNNodeResult.h"
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

void HTNDecompositionWatchTooltipPrinter::Print(HTNDecompositionWatchTooltipPrinterContext& ioDecompositionWatchTooltipPrinterContext) const
{
    const HTNDecompositionTooltipMode TooltipMode = ioDecompositionWatchTooltipPrinterContext.GetTooltipMode();
    if (TooltipMode == HTNDecompositionTooltipMode::NONE)
    {
        return;
    }

    const HTNDecompositionNode& Node       = ioDecompositionWatchTooltipPrinterContext.GetNode();
    const HTNNodeResult*        NodeResult = Node.GetNodeResult();
    if (!NodeResult)
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
                const HTNAtom Parameter = GetNodeValue(*NodeParameter, ioDecompositionWatchTooltipPrinterContext);

                // Parameter ID
                const std::string ParameterIDString = Parameter.GetListElement(HTNVariableExpressionNodeResult::ID_STRING).GetValue<std::string>();
                const HTNAtom&    ParameterIDColor  = Parameter.GetListElement(HTNVariableExpressionNodeResult::ID_COLOR);
                const ImVec4      ParameterIDImGuiColor =
                    ImVec4(ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::X).GetValue<float>(),
                           ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Y).GetValue<float>(),
                           ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Z).GetValue<float>(),
                           ParameterIDColor.GetListElement(HTNVariableExpressionNodeColorResult::W).GetValue<float>());
                ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());

                // Parameter value
                const std::string ParameterValueString =
                    Parameter.GetListElement(HTNVariableExpressionNodeResult::VALUE_STRING).GetValue<std::string>();
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
                const HTNAtom Argument = GetNodeValue(*NodeArgument, ioDecompositionWatchTooltipPrinterContext);

                // Argument ID
                const std::string ArgumentIDString = Argument.GetListElement(HTNVariableExpressionNodeResult::ID_STRING).GetValue<std::string>();
                const HTNAtom&    ArgumentIDColor  = Argument.GetListElement(HTNVariableExpressionNodeResult::ID_COLOR);
                const ImVec4 ArgumentIDImGuiColor  = ImVec4(ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::X).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Y).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::Z).GetValue<float>(),
                                                            ArgumentIDColor.GetListElement(HTNVariableExpressionNodeColorResult::W).GetValue<float>());
                ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());

                // Argument value
                const std::string ArgumentValueString =
                    Argument.GetListElement(HTNVariableExpressionNodeResult::VALUE_STRING).GetValue<std::string>();
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
                    static constexpr bool ShouldDoubleQuoteString = true;
                    const std::string     VariableValueString     = VariablePair.second.ToString(ShouldDoubleQuoteString);
                    ImGui::SameLine();
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
