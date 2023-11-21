// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNDecompositionWatchWindowPrinterContext::HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                     const HTNDecompositionNode&                 inDecompositionNode)
    : HTNDecompositionWatchPrinterContextBase(inDomainNode, inDecompositionNode)
{
}
#endif
