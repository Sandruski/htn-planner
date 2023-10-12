#pragma once

#ifdef HTN_DEBUG
#include "WorldState/HTNWorldState.h"

#include <string>

class HTNWorldState;

struct ImGuiTextFilter;

/**
 * Prints a database representing a world state
 */
class HTNWorldStatePrinter
{
public:
    bool Print(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter) const;

private:
    void PrintFactID(const std::string& inFactID) const;
    void PrintFactArguments(const HTNFactArguments& inFactArguments) const;
};
#endif
