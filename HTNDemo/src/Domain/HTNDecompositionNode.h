#pragma once

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNNodePath.h"

#include <memory>
#include <vector>

class HTNNodeSnapshotDebug;
class HTNValueExpressionNodeBase;

class HTNDecompositionNode
{
public:
    HTNDecompositionNode() = default;
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeParametersArguments,
                                  const HTNNodePath&                                                    inNodeVariableScopeNodePath);
    const HTNNodeSnapshotDebug*                                           GetNodeSnapshot() const;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeParametersArguments() const;
    const HTNNodePath&                                                    GetNodeVariableScopeNodePath() const;

private:
    const HTNNodeSnapshotDebug*                                    mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeParametersArguments;
    HTNNodePath                                                    mNodeVariableScopeNodePath;
};

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot) : mNodeSnapshot(&inNodeSnapshot)
{
}

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeParametersArguments,
                                                  const HTNNodePath&                                                    inNodeVariableScopeNodePath)
    : mNodeSnapshot(&inNodeSnapshot), mNodeParametersArguments(inNodeParametersArguments), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath)
{
}

inline const HTNNodeSnapshotDebug* HTNDecompositionNode::GetNodeSnapshot() const
{
    return mNodeSnapshot;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNDecompositionNode::GetNodeParametersArguments() const
{
    return mNodeParametersArguments;
}

inline const HTNNodePath& HTNDecompositionNode::GetNodeVariableScopeNodePath() const
{
    return mNodeVariableScopeNodePath;
}
#endif
