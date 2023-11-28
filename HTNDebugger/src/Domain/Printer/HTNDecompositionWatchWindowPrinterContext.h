// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNNodeInstance;
class HTNDomainNode;

/**
 * Contextual data of a decomposition watch window printer
 */
class HTNDecompositionWatchWindowPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                       const HTNNodeInstance&                      inNodeInstance);
};
#endif
