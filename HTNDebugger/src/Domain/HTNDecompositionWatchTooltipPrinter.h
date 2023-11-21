// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
#include "Domain/HTNDecompositionWatchPrinterBase.h"

class HTNDecompositionWatchTooltipPrinterContext;

/**
 * Prints the watch tooltip for the selected node
 */
class HTNDecompositionWatchTooltipPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    void Print(HTNDecompositionWatchTooltipPrinterContext& ioDecompositionWatchTooltipPrinterContext) const;
};
#endif
