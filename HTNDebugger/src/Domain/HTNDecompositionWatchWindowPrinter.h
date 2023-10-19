#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionWatchPrinterBase.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;

/**
 * Prints the watch window for the selected node
 */
class HTNDecompositionWatchWindowPrinter final : public HTNDecompositionWatchPrinterBase
{
public:
    explicit HTNDecompositionWatchWindowPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    void Print();
};
#endif
