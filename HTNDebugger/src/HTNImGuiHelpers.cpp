#pragma once

#ifdef HTN_DEBUG
#include "HTNImGuiHelpers.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"

#include "imgui_internal.h"

namespace HTNImGuiHelpers
{
void SetTreeNodeOpen(const std::string& inLabel, const bool inIsOpen)
{
    ImGuiWindow*  CurrentWindow = ImGui::GetCurrentWindow();
    const ImGuiID ID            = CurrentWindow->GetID(inLabel.c_str());
    return ImGui::TreeNodeSetOpen(ID, inIsOpen);
}

bool IsTreeNodeOpen(const std::string& inLabel, const ImGuiTreeNodeFlags inTreeNodeFlags)
{
    ImGuiWindow*  CurrentWindow = ImGui::GetCurrentWindow();
    const ImGuiID ID            = CurrentWindow->GetID(inLabel.c_str());
    return ImGui::TreeNodeUpdateNextOpen(ID, inTreeNodeFlags);
}

ImVec4 GetVariableColor(const std::string& inVariableID)
{
    if (HTNDecompositionHelpers::IsParameter(inVariableID))
    {
        return kParameterColor;
    }
    else if (HTNDecompositionHelpers::IsAnyArgument(inVariableID))
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
