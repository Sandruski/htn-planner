#include "HTNDebugHelpers.h"

#include "imgui.h"

#include <format>

namespace HTNDebug::Helpers
{
void PrintKeyword(const std::string& inKeyword)
{
    ImGui::TextDisabled(inKeyword.c_str());
}
} // namespace HTNLexer::Helpers
