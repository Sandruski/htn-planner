#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionWatchPrinterBase.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;

/**
 * Prints the watch tooltip for the selected node
 */
class HTNDecompositionWatchTooltipPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    explicit HTNDecompositionWatchTooltipPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    void Print(const bool inShouldPrintFullTooltip);
};
#endif
