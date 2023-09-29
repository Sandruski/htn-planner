#include "WorldState/HTNWorldStatePrinter.h"

#include "WorldState/HTNWorldState.h"

#include "imgui.h"

#include <format>

bool HTNWorldStatePrinter::Print(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter) const
{
    const std::unordered_map<std::string, HTNWorldState::HTNFact>& Facts = inWorldState.GetFacts();
    for (const std::pair<std::string, HTNWorldState::HTNFact>& Fact : Facts)
    {
        const std::string&            FactID             = Fact.first;
        const HTNWorldState::HTNFact& FactArgumentTables = Fact.second;
        for (const HTNTableBase* FactArgumentTable : FactArgumentTables)
        {
            if (!FactArgumentTable)
            {
                continue;
            }

            const std::vector<std::vector<HTNAtom>> FactArgumentTableEntries = FactArgumentTable->GetEntries(); // TODO salvarez Fix reference
            for (const std::vector<HTNAtom>& FactArguments : FactArgumentTableEntries)
            {
                std::string FactString = FactID;
                for (const HTNAtom& FactArgument : FactArguments)
                {
                static constexpr bool ShouldDoubleQuoteString = false;
                    FactString.append(std::format(" {}", FactArgument.ToString(ShouldDoubleQuoteString)));
                }

                if (!inTextFilter.PassFilter(FactString.c_str()))
                {
                    continue;
                }

                PrintFactID(FactID);
                PrintFactArguments(FactArguments);
            }
        }
    }

    return true;
}

void HTNWorldStatePrinter::PrintFactID(const std::string& inFactID) const
{
    ImGui::Text(inFactID.c_str());
}

void HTNWorldStatePrinter::PrintFactArguments(const std::vector<HTNAtom>& inFactArguments) const
{
    for (const HTNAtom& FactArgument : inFactArguments)
    {
        ImGui::SameLine();

    static constexpr bool ShouldDoubleQuoteString = true;
        const std::string FactArgumentString = FactArgument.ToString(ShouldDoubleQuoteString);
        ImGui::Text(FactArgumentString.c_str());
    }
}
