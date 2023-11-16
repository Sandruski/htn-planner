#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
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
