#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionWatchPrinterBase.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;
enum class HTNDecompositionTooltipMode : unsigned char;

/**
 * Prints the watch tooltip for the selected node
 */
class HTNDecompositionWatchTooltipPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    void Print(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode,
               const HTNDecompositionTooltipMode inTooltipMode);
};
#endif
