#pragma once

#ifdef HTN_DEBUG
#include "imgui.h"

#include <format>
#include <string>

namespace HTNImGuiHelpers
{
std::string MakeLabel(const std::string& inLabel, const std::string& inID);

void SelectTreeNode(ImGuiTreeNodeFlags& outTreeNodeFlags);
bool IsCurrentItemSelected();

const ImGuiWindowFlags     kDefaultWindowFlags     = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
const ImGuiTabBarFlags     kDefaultTabBarFlags     = ImGuiTabBarFlags_None;
const ImGuiComboFlags      kDefaultComboFlags      = ImGuiComboFlags_None;
const ImGuiSelectableFlags kDefaultSelectableFlags = ImGuiSelectableFlags_None;
const ImGuiTableFlags      kDefaultTableFlags      = ImGuiTableFlags_RowBg;
const ImGuiInputTextFlags  kDefaultInputTextFlags  = ImGuiInputTextFlags_CharsNoBlank;
const ImGuiTreeNodeFlags   kDefaultTreeNodeFlags   = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

constexpr ImVec4 kFailColor      = ImVec4(1.f, 0.5f, 0.5f, 1.f);
constexpr ImVec4 kSuccessColor   = ImVec4(0.f, 1.f, 0.f, 1.f);
constexpr ImVec4 kParameterColor = ImVec4(1.f, 0.5f, 0.f, 1.f);
constexpr ImVec4 kArgumentColor  = ImVec4(1.f, 1.f, 0.f, 1.f);
} // namespace HTNImGuiHelpers

namespace HTNImGuiHelpers
{
inline std::string MakeLabel(const std::string& inLabel, const std::string& inID)
{
    return std::format("##{}{}", inLabel, inID);
}

inline void SelectTreeNode(ImGuiTreeNodeFlags& outTreeNodeFlags)
{
    outTreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
}

inline bool IsCurrentItemSelected()
{
    return ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && !ImGui::IsItemToggledOpen();
}
} // namespace HTNImGuiHelpers
#endif
