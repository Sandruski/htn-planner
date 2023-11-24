// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"
#include "Domain/Printer/HTNDecompositionTooltipMode.h"

#include <memory>

class HTNNodeInstance;
class HTNDomainNode;

class HTNDecompositionWatchTooltipPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                        const HTNNodeInstance&                 inNodeInstance,
                                                        const HTNDecompositionTooltipMode           inTooltipMode);

    HTN_NODISCARD HTNDecompositionTooltipMode GetTooltipMode() const;

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
