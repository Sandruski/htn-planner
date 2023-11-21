// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
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
