#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"

#ifdef HTN_DEBUG
HTNDecompositionWatchTooltipPrinterContext::HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                       const HTNDecompositionNode& inDecompositionNode,
                                                                                       const HTNDecompositionTooltipMode           inTooltipMode)
    : HTNDecompositionWatchPrinterContextBase(inDomainNode, inDecompositionNode), mTooltipMode(inTooltipMode)
{
}
#endif
