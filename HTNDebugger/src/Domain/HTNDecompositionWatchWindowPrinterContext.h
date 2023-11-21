// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
#include "Domain/HTNDecompositionWatchPrinterContextBase.h"

#include <memory>

class HTNDecompositionNode;
class HTNDomainNode;

class HTNDecompositionWatchWindowPrinterContext final : public HTNDecompositionWatchPrinterContextBase
{
public:
    explicit HTNDecompositionWatchWindowPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                       const HTNDecompositionNode&                 inDecompositionNode);
};
#endif
