// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionWatchTooltipPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNDecompositionWatchTooltipPrinterContext::HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                       const HTNNodeInstance&                      inNodeInstance,
                                                                                       const HTNDecompositionTooltipMode           inTooltipMode)
    : HTNDecompositionWatchPrinterContextBase(inDomainNode, inNodeInstance), mTooltipMode(inTooltipMode)
{
}
#endif
