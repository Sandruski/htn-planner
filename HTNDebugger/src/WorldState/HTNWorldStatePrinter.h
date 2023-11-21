// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
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
