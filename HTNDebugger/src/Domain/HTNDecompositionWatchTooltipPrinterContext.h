#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
#include "Domain/HTNDecompositionWatchPrinterContextBase.h"
#include "Domain/HTNDecompositionTooltipMode.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;

class HTNDecompositionWatchTooltipPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                        const HTNDecompositionNode&                 inDecompositionNode,
                                                        const HTNDecompositionTooltipMode           inTooltipMode);

    HTNDecompositionTooltipMode GetTooltipMode() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const HTNDecompositionTooltipMode mTooltipMode = HTNDecompositionTooltipMode::NONE;
};

inline HTNDecompositionTooltipMode HTNDecompositionWatchTooltipPrinterContext::GetTooltipMode() const
{
    return mTooltipMode;
}
#endif
