#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
#include "Domain/HTNDecompositionWatchPrinterBase.h"

class HTNDecompositionWatchWindowPrinterContext;

/**
 * Prints the watch window for the selected node
 */
class HTNDecompositionWatchWindowPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    void Print(HTNDecompositionWatchWindowPrinterContext& ioDecompositionWatchWindowPrinterContext) const;
};
#endif
