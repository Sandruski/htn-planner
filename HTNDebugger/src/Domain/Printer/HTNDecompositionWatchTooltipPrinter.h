// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionWatchPrinterBase.h"
#include "HTNCoreMinimal.h"

class HTNDecompositionWatchTooltipPrinterContext;

/**
 * Prints a tooltip with the state of the variables of the hovered node
 */
class HTNDecompositionWatchTooltipPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    // Main print member method
    void Print(HTNDecompositionWatchTooltipPrinterContext& ioDecompositionWatchTooltipPrinterContext) const;
};
#endif
