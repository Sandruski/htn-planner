#include "Domain/HTNDecompositionWatchPrinterContextBase.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"

HTNDecompositionWatchPrinterContextBase::HTNDecompositionWatchPrinterContextBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                 const HTNDecompositionNode&                 inDecompositionNode)
    : mDomainNode(inDomainNode), mNode(&inDecompositionNode)
{
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = inDecompositionNode.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = inDecompositionNode.GetNodeArguments();
    mNodeVariablePaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

HTNDecompositionWatchPrinterContextBase::~HTNDecompositionWatchPrinterContextBase() = default;
#endif
