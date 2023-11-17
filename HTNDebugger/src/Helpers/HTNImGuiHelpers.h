#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"

#include "imgui.h"

#include <format>
#include <string>

class HTNNodeSnapshotDebug;
enum class HTNNodeStep : uint8;

namespace HTNImGuiHelpers
{
void SelectTreeNode(ImGuiTreeNodeFlags& outTreeNodeFlags);

void SetTreeNodeOpen(const std::string& inLabel, const bool inIsOpen);
bool IsTreeNodeOpen(const std::string& inLabel, const ImGuiTreeNodeFlags inTreeNodeFlags);

bool IsCurrentItemHovered(const ImGuiHoveredFlags inHoveredFlags);
bool IsCurrentItemSelected();

ImVec4 GetNodeColor(const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep);
ImVec4 GetResultColor(const bool inResult);
ImVec4 GetVariableColor(const std::string& inVariableID);

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
// @see imgui_demo.cpp
void HelpMarker(const std::string& inDescription);

const ImGuiWindowFlags     kDefaultWindowFlags     = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
const ImGuiTabBarFlags     kDefaultTabBarFlags     = ImGuiTabBarFlags_None;
const ImGuiComboFlags      kDefaultComboFlags      = ImGuiComboFlags_None;
const ImGuiSelectableFlags kDefaultSelectableFlags = ImGuiSelectableFlags_None;
const ImGuiTableFlags      kDefaultTableFlags      = ImGuiTableFlags_RowBg;
const ImGuiInputTextFlags  kDefaultInputTextFlags  = ImGuiInputTextFlags_CharsNoBlank;
const ImGuiTreeNodeFlags   kDefaultTreeNodeFlags   = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
const ImGuiHoveredFlags    kDefaultHoveredFlags    = ImGuiHoveredFlags_ForTooltip;

constexpr ImVec4 kFailColor        = ImVec4(1.f, 0.5f, 0.5f, 1.f);
constexpr ImVec4 kSuccessColor     = ImVec4(0.f, 1.f, 0.f, 1.f);
constexpr ImVec4 kNoResultColor    = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
constexpr ImVec4 kParameterColor   = ImVec4(1.f, 0.6f, 0.f, 1.f);
constexpr ImVec4 kArgumentColor    = ImVec4(1.f, 1.f, 0.f, 1.f);
constexpr ImVec4 kAnyArgumentColor = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
} // namespace HTNImGuiHelpers

namespace HTNImGuiHelpers
{
inline void SelectTreeNode(ImGuiTreeNodeFlags& outTreeNodeFlags)
{
    outTreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
}

inline bool IsCurrentItemHovered(const ImGuiHoveredFlags inHoveredFlags)
{
    return ImGui::IsItemHovered(inHoveredFlags);
}

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
