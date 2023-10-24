#pragma once

#ifdef HTN_DEBUG
#include "HTNImGuiHelpers.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"

#include "imgui_internal.h"

namespace HTNImGuiHelpers
{
bool IsTreeNodeOpen(const std::string& inLabel, const ImGuiTreeNodeFlags inTreeNodeFlags)
{
    ImGuiWindow*  CurrentWindow = ImGui::GetCurrentWindow();
    const ImGuiID ID            = CurrentWindow->GetID(inLabel.c_str());
    return ImGui::TreeNodeUpdateNextOpen(ID, inTreeNodeFlags);
}

ImVec4 GetArgumentColor(const std::string& inArgumentID)
{
    if (HTNDecompositionHelpers::IsParameter(inArgumentID))
    {
        return kParameterColor;
    }
    else if (HTNDecompositionHelpers::IsAnyArgument(inArgumentID))
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
