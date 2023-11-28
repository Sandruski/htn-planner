// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionTooltipMode.h"
#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNNodeInstance;
class HTNDomainNode;

/**
 * Contextual data of a decomposition watch tooltip printer
 */
class HTNDecompositionWatchTooltipPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchTooltipPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                        const HTNNodeInstance& inNodeInstance, const HTNDecompositionTooltipMode inTooltipMode);

    // Returns the mode of the tooltip
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
