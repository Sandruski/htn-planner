// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Helpers/HTNImGuiHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

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

void SetTreeNodeOpen(const std::string& inLabel, const bool inIsOpen)
{
    ImGuiWindow*  CurrentWindow = ImGui::GetCurrentWindow();
    const ImGuiID ID            = CurrentWindow->GetID(inLabel.c_str());
    return ImGui::TreeNodeSetOpen(ID, inIsOpen);
}

ImVec4 GetNodeColor(const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep)
{
    switch (inNodeStep)
    {
    case HTNNodeStep::START: {
        return kNoResultColor;
    }
    case HTNNodeStep::END: {
        const bool Result = inNodeSnapshot.GetResult();
        return GetResultColor(Result);
    }
    default: {
        assert(false);
    }
    }

    return ImVec4();
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
