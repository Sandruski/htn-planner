#pragma once

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNNodePath.h"

#include <memory>
#include <vector>

class HTNNodeSnapshotDebug;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

class HTNDecompositionNode
{
public:
    HTNDecompositionNode() = default;
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                          inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                  const HTNNodePath&                                                   inNodeVariableScopeNodePath);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                  const HTNNodePath&                                                    inNodeVariableScopeNodePath);

    const HTNNodeSnapshotDebug*                                           GetNodeSnapshot() const;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  GetNodeParameters() const;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;
    const HTNNodePath&                                                    GetNodeVariableScopeNodePath() const;

private:
    const HTNNodeSnapshotDebug*                                    mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNNodePath                                                    mNodeVariableScopeNodePath;
};

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot) : mNodeSnapshot(&inNodeSnapshot)
{
}

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                          inNodeSnapshot,
                                                  const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                                  const HTNNodePath&                                                   inNodeVariableScopeNodePath)
    : mNodeSnapshot(&inNodeSnapshot), mNodeParameters(inNodeParameters), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath)
{
}

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                                  const HTNNodePath&                                                    inNodeVariableScopeNodePath)
    : mNodeSnapshot(&inNodeSnapshot), mNodeArguments(inNodeArguments), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath)
{
}

inline const HTNNodeSnapshotDebug* HTNDecompositionNode::GetNodeSnapshot() const
{
    return mNodeSnapshot;
}

inline const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& HTNDecompositionNode::GetNodeParameters() const
{
    return mNodeParameters;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNDecompositionNode::GetNodeArguments() const
{
    return mNodeArguments;
}

inline const HTNNodePath& HTNDecompositionNode::GetNodeVariableScopeNodePath() const
{
    return mNodeVariableScopeNodePath;
}
#endif
