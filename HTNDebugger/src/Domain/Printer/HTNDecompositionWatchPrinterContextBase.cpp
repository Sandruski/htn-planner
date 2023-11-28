// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNNodeInstance.h"

HTNDecompositionWatchPrinterContextBase::HTNDecompositionWatchPrinterContextBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                 const HTNNodeInstance&                      inNodeInstance)
    : mDomainNode(inDomainNode), mNodeInstance(inNodeInstance)
{
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = inNodeInstance.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = inNodeInstance.GetNodeArguments();
    mVariablePaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

HTNDecompositionWatchPrinterContextBase::~HTNDecompositionWatchPrinterContextBase() = default;
#endif
