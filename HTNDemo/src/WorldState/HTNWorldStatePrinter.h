#pragma once

#include <string>
#include <vector>

class HTNAtom;
class HTNWorldState;

struct ImGuiTextFilter;

// Prints a database representing a world state
class HTNWorldStatePrinter
{
public:
    bool Print(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter) const;

private:
    void PrintFactID(const std::string& inFactID) const;
    void PrintFactArguments(const std::vector<HTNAtom>& inFactArguments) const;
};
