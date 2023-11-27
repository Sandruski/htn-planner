// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

#include "imgui.h"

#include <string>

class HTNNodeSnapshot;
enum class HTNNodeStep : uint8;

namespace HTNImGuiHelpers
{
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
// @see imgui_demo.cpp
void HelpMarker(const std::string& inDescription);

void               SetTreeNodeOpen(const std::string& inLabel, const bool inIsOpen);
HTN_NODISCARD bool IsCurrentItemSelected();

HTN_NODISCARD ImVec4 GetNodeColor(const HTNNodeSnapshot& inNodeSnapshot, const HTNNodeStep inNodeStep);
HTN_NODISCARD ImVec4 GetResultColor(const bool inResult);
HTN_NODISCARD ImVec4 GetVariableColor(const std::string& inVariableID);

inline constexpr ImVec4 kFailColor        = ImVec4(1.f, 0.5f, 0.5f, 1.f);
inline constexpr ImVec4 kSuccessColor     = ImVec4(0.f, 1.f, 0.f, 1.f);
inline constexpr ImVec4 kNoResultColor    = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
inline constexpr ImVec4 kParameterColor   = ImVec4(1.f, 0.6f, 0.f, 1.f);
inline constexpr ImVec4 kArgumentColor    = ImVec4(1.f, 1.f, 0.f, 1.f);
inline constexpr ImVec4 kAnyArgumentColor = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
} // namespace HTNImGuiHelpers

namespace HTNImGuiHelpers
{
inline bool IsCurrentItemSelected()
{
    return ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
}

inline ImVec4 GetResultColor(const bool inResult)
{
    return inResult ? kSuccessColor : kFailColor;
}
} // namespace HTNImGuiHelpers
#endif
