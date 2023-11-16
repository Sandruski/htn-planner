#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
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
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                          inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                  const HTNNodePath& inNodeVariableScopeNodePath, const std::string& inNodeLabel);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                  const HTNNodePath& inNodeVariableScopeNodePath, const std::string& inNodeLabel);

    const HTNNodeSnapshotDebug*                                           GetNodeSnapshot() const;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  GetNodeParameters() const;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;
    const HTNNodePath&                                                    GetNodeVariableScopeNodePath() const;
    const std::string&                                                    GetNodeLabel() const;

private:
    const HTNNodeSnapshotDebug*                                    mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNNodePath                                                    mNodeVariableScopeNodePath;
    std::string                                                    mNodeLabel;
};

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

inline const std::string& HTNDecompositionNode::GetNodeLabel() const
{
    return mNodeLabel;
}
#endif
