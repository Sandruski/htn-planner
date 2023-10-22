#pragma once

#ifdef HTN_DEBUG
#include "imgui.h"

#include <format>
#include <string>

namespace HTNImGuiHelpers
{
void SelectTreeNode(ImGuiTreeNodeFlags& outTreeNodeFlags);
bool IsCurrentItemHovered();
bool        IsCurrentItemSelected();

ImVec4 GetResultColor(const bool inResult);
ImVec4 GetArgumentColor(const std::string& inArgumentID);

const ImGuiWindowFlags     kDefaultWindowFlags     = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
const ImGuiTabBarFlags     kDefaultTabBarFlags     = ImGuiTabBarFlags_None;
const ImGuiComboFlags      kDefaultComboFlags      = ImGuiComboFlags_None;
const ImGuiSelectableFlags kDefaultSelectableFlags = ImGuiSelectableFlags_None;
const ImGuiTableFlags      kDefaultTableFlags      = ImGuiTableFlags_RowBg;
const ImGuiInputTextFlags  kDefaultInputTextFlags  = ImGuiInputTextFlags_CharsNoBlank;
const ImGuiTreeNodeFlags   kDefaultTreeNodeFlags   = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

constexpr ImVec4 kFailColor        = ImVec4(1.f, 0.5f, 0.5f, 1.f);
constexpr ImVec4 kSuccessColor     = ImVec4(0.f, 1.f, 0.f, 1.f);
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

inline bool IsCurrentItemHovered()
{
    return ImGui::IsItemHovered();
}

inline bool IsCurrentItemSelected()
{
    return IsCurrentItemHovered() && ImGui::IsMouseDoubleClicked(0);
}

inline ImVec4 GetResultColor(const bool inResult)
{
    return inResult ? kSuccessColor : kFailColor;
}
} // namespace HTNImGuiHelpers
#endif
