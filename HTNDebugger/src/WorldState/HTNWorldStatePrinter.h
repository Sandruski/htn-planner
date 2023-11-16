#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
#include "WorldState/HTNWorldState.h"

#include <string>

class HTNWorldStatePrinterContext;

/**
 * Prints a database representing a world state
 */
class HTNWorldStatePrinter
{
public:
    bool Print(HTNWorldStatePrinterContext& ioWorldStatePrinterContext) const;

private:
    void PrintFactID(const std::string& inFactID) const;
    void PrintFactArguments(const HTNFactArguments& inFactArguments) const;
};
#endif
