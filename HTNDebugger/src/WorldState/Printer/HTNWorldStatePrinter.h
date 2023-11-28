// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "WorldState/HTNWorldStateFwd.h"

#include <string>

class HTNWorldStatePrinterContext;

/**
 * Prints a world state
 */
class HTNWorldStatePrinter
{
public:
    // Main print member method
    bool Print(HTNWorldStatePrinterContext& ioWorldStatePrinterContext) const;

private:
    // Prints the ID of a fact
    void PrintFactID(const std::string& inFactID) const;

    // Prints the arguments of a fact
    void PrintFactArguments(const HTNFactArguments& inFactArguments) const;
};
#endif
