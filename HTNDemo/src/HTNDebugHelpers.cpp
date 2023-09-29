#include "HTNDebugHelpers.h"

#include "imgui.h"

#include <format>

namespace HTNDebug::Helpers
{
void PrintKeyword(const std::string& inKeyword)
{
    ImGui::TextDisabled(inKeyword.c_str());
}

void PrintString(const std::string& inString)
{
    const std::string String = std::format("\"{}\"", inString);
    ImGui::Text(String.c_str());
}
} // namespace HTNLexer::Helpers
