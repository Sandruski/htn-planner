#include "WorldState/HTNWorldStatePrinter.h"

#ifdef HTN_DEBUG
#include "WorldState/HTNWorldState.h"
#include "WorldState/HTNWorldStatePrinterContext.h"

#include "imgui.h"

bool HTNWorldStatePrinter::Print(HTNWorldStatePrinterContext& ioWorldStatePrinterContext) const
{
    const HTNWorldState&   WorldState = ioWorldStatePrinterContext.GetWorldState();
    const ImGuiTextFilter& TextFilter = ioWorldStatePrinterContext.GetTextFilter();

    const std::unordered_map<std::string, HTNFactArgumentsTables>& Facts = WorldState.GetFacts();
    for (const std::pair<std::string, HTNFactArgumentsTables>& Fact : Facts)
    {
        const std::string&            FactID              = Fact.first;
        const HTNFactArgumentsTables& FactArgumentsTables = Fact.second;
        for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
        {
            const std::vector<HTNFactArguments>& FactArgumentsContainer = FactArgumentsTable.GetFactArguments();
            for (const HTNFactArguments& FactArgumentsElement : FactArgumentsContainer)
            {
                std::string Text = FactID;
                for (const HTNAtom& FactArgument : FactArgumentsElement)
                {
                    constexpr bool ShouldDoubleQuoteString = false;
                    Text.append(std::format(" {}", FactArgument.ToString(ShouldDoubleQuoteString)));
                }

                // Filter text with and without double quotes for usability
                bool PassFilterResult = TextFilter.PassFilter(Text.c_str());
                if (!PassFilterResult)
                {
                    Text = FactID;
                    for (const HTNAtom& FactArgument : FactArgumentsElement)
                    {
                        constexpr bool ShouldDoubleQuoteString = true;
                        Text.append(std::format(" {}", FactArgument.ToString(ShouldDoubleQuoteString)));
                    }

                    PassFilterResult = TextFilter.PassFilter(Text.c_str());

                    if (!PassFilterResult)
                    {
                        continue;
                    }
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

        constexpr bool    ShouldDoubleQuoteString = true;
        const std::string FactArgumentString      = FactArgument.ToString(ShouldDoubleQuoteString);
        ImGui::Text(FactArgumentString.c_str());
    }
}
#endif
