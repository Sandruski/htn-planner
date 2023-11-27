// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNImGuiHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "Domain/Printer/HTNDecompositionPrinter.h"
#include "Domain/Printer/HTNNodeState.h"

#include "imgui_internal.h"

namespace HTNImGuiHelpers
{
void HelpMarker(const std::string& inDescription)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(inDescription.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void SetTreeNodeOpen(const std::string& inTreeNodeLabel, const bool inIsOpen)
{
    ImGuiWindow*  CurrentWindow = ImGui::GetCurrentWindow();
    const ImGuiID TreeNodeID    = CurrentWindow->GetID(inTreeNodeLabel.c_str());
    return ImGui::TreeNodeSetOpen(TreeNodeID, inIsOpen);
}

ImVec4 GetNodeColor(const bool inNodeResult, const HTNNodeStep inNodeStep)
{
    switch (inNodeStep)
    {
    case HTNNodeStep::START: {
        return kNoResultColor;
    }
    case HTNNodeStep::END: {
        return GetResultColor(inNodeResult);
    }
    default: {
        assert(false);
    }
    }

    return ImVec4();
}

ImVec4 GetVariableColor(const std::string& inVariableID)
{
    if (HTNDomainHelpers::IsParameter(inVariableID))
    {
        return kParameterColor;
    }
    else if (HTNDomainHelpers::IsAnyArgument(inVariableID))
    {
        return kAnyArgumentColor;
    }
    else
    {
        return kArgumentColor;
    }
}
} // namespace HTNImGuiHelpers
#endif
