#include "Domain/HTNDecompositionNode.h"

#ifdef HTN_DEBUG
HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeLabel(inNodeLabel)
{
}

HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                          inNodeSnapshot,
                                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                           const HTNNodePath& inNodeVariableScopeNodePath, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeParameters(inNodeParameters), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath),
      mNodeLabel(inNodeLabel)
{
}

HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                           const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                           const HTNNodePath& inNodeVariableScopeNodePath, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeArguments(inNodeArguments), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath),
      mNodeLabel(inNodeLabel)
{
}
#endif
