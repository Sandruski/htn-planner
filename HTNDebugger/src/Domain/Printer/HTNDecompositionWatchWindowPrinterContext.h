// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"

#include <memory>

class HTNNodeInstance;
class HTNDomainNode;

class HTNDecompositionWatchWindowPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                       const HTNNodeInstance&                 inNodeInstance);
};
#endif
