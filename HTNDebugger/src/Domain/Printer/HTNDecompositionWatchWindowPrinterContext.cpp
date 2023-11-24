// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionWatchWindowPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNDecompositionWatchWindowPrinterContext::HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                     const HTNNodeInstance&                      inNodeInstance)
    : HTNDecompositionWatchPrinterContextBase(inDomainNode, inNodeInstance)
{
}
#endif
