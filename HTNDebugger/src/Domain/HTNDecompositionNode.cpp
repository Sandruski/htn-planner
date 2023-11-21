#include "Domain/HTNDecompositionNode.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeLabel(inNodeLabel)
{
}

HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                          inNodeSnapshot,
                                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                           const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeParameters(inNodeParameters), mVariablesPathHandler(inVariablesPathHandler), mNodeLabel(inNodeLabel)
{
}

HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                           const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                           const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeArguments(inNodeArguments), mVariablesPathHandler(inVariablesPathHandler), mNodeLabel(inNodeLabel)
{
}
#endif
