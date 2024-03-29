// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "WorldState/Printer/HTNWorldStatePrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "WorldState/HTNWorldState.h"
#include "WorldState/Printer/HTNWorldStatePrinterContext.h"

#include "imgui.h"

namespace
{
HTN_NODISCARD std::string MakeFactDescription(const std::string& inFactID, const HTNFactArguments& inFactArguments,
                                              const bool inShouldDoubleQuoteString)
{
    std::string FactDescription = inFactID;

    for (const HTNAtom& FactArgument : inFactArguments)
    {
        FactDescription.append(std::format(" {}", FactArgument.ToString(inShouldDoubleQuoteString)));
    }

    return FactDescription;
}
} // namespace

bool HTNWorldStatePrinter::Print(HTNWorldStatePrinterContext& ioWorldStatePrinterContext) const
{
    OPTICK_EVENT("PrintWorldState");

    const HTNWorldState&   WorldState = ioWorldStatePrinterContext.GetWorldState();
    const ImGuiTextFilter& TextFilter = ioWorldStatePrinterContext.GetTextFilter();

    const HTNFacts& Facts = WorldState.GetFacts();
    for (const HTNFact& Fact : Facts)
    {
        const std::string&            FactID              = Fact.first;
        const HTNFactArgumentsTables& FactArgumentsTables = Fact.second;
        for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
        {
            const std::vector<HTNFactArguments>& FactArgumentsCollection = FactArgumentsTable.GetFactArgumentsCollection();
            for (const HTNFactArguments& FactArguments : FactArgumentsCollection)
            {
                bool        ShouldDoubleQuoteString = false;
                std::string Text                    = MakeFactDescription(FactID, FactArguments, ShouldDoubleQuoteString);

                // Filter text with and without double quotes for usability
                bool PassFilterResult = TextFilter.PassFilter(Text.c_str());
                if (!PassFilterResult)
                {
                    ShouldDoubleQuoteString = true;
                    Text                    = MakeFactDescription(FactID, FactArguments, ShouldDoubleQuoteString);

                    PassFilterResult = TextFilter.PassFilter(Text.c_str());
                    if (!PassFilterResult)
                    {
                        continue;
                    }
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
    OPTICK_EVENT("PrintFactID");

    ImGui::Text(inFactID.c_str());
}

void HTNWorldStatePrinter::PrintFactArguments(const HTNFactArguments& inFactArguments) const
{
    OPTICK_EVENT("PrintFactArguments");

    for (const HTNAtom& FactArgument : inFactArguments)
    {
        ImGui::SameLine();

        static constexpr bool ShouldDoubleQuoteString = true;
        const std::string     FactArgumentString      = FactArgument.ToString(ShouldDoubleQuoteString);
        ImGui::Text(FactArgumentString.c_str());
    }
}
#endif
