#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionWatchPrinterContextBase.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;

enum class HTNDecompositionTooltipMode : unsigned char
{
    NONE,
    REGULAR,
    FULL
};

class HTNDecompositionWatchTooltipPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                        const HTNDecompositionNode&                 inDecompositionNode,                                                        
                                                        const HTNDecompositionTooltipMode           inTooltipMode);

    HTNDecompositionTooltipMode GetTooltipMode() const;

private:
    HTNDecompositionTooltipMode mTooltipMode = HTNDecompositionTooltipMode::NONE;
};

inline HTNDecompositionTooltipMode HTNDecompositionWatchTooltipPrinterContext::GetTooltipMode() const;
{
    return mTooltipMode;
}
#endif
