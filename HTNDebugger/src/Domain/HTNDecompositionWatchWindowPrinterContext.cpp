#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"

#ifdef HTN_DEBUG
HTNDecompositionWatchWindowPrinterContext::HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                     const HTNDecompositionNode&                 inDecompositionNode)
    : HTNDecompositionWatchPrinterContextBase(inDomainNode, inDecompositionNode)
{
}
#endif
