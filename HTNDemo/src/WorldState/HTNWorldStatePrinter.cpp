#include "WorldState/HTNWorldStatePrinter.h"

#include "WorldState/HTNWorldState.h"

#include "imgui.h"

#include <format>

bool HTNWorldStatePrinter::Print(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter) const
{
    const std::unordered_map<std::string, HTNFactArgumentsTables>& Facts = inWorldState.GetFacts();
    for (const std::pair<std::string, HTNFactArgumentsTables>& Fact : Facts)
    {
        const std::string&            FactID              = Fact.first;
        const HTNFactArgumentsTables& FactArgumentsTables = Fact.second;
        for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
        {
            const std::vector<HTNFactArguments>& FactArgumentsContainer = FactArgumentsTable.GetFactArguments();
            for (const HTNFactArguments& FactArgumentsElement : FactArgumentsContainer)
            {
                std::string FactString = FactID;
                for (const HTNAtom& FactArgument : FactArgumentsElement)
                {
                    static constexpr bool ShouldDoubleQuoteString = false;
                    FactString.append(std::format(" {}", FactArgument.ToString(ShouldDoubleQuoteString)));
                }

                if (!inTextFilter.PassFilter(FactString.c_str()))
                {
                    continue;
                }

                PrintFactID(FactID);
                PrintFactArguments(FactArgumentsElement);
            }
        }
    }

    return true;
}

void HTNWorldStatePrinter::PrintFactID(const std::string& inFactID) const
{
    ImGui::Text(inFactID.c_str());
}

void HTNWorldStatePrinter::PrintFactArguments(const HTNFactArguments& inFactArguments) const
{
    for (const HTNAtom& FactArgument : inFactArguments)
    {
        ImGui::SameLine();

        static constexpr bool ShouldDoubleQuoteString = true;
        const std::string     FactArgumentString      = FactArgument.ToString(ShouldDoubleQuoteString);
        ImGui::Text(FactArgumentString.c_str());
    }
}
