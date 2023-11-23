// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Core/HTNPathHandler.h"

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
                                  const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                           inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                  const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel);

    const HTNNodeSnapshotDebug*                                           GetNodeSnapshot() const;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  GetNodeParameters() const;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;
    const HTNPathHandler&                                                 GetVariablesPathHandler() const;
    HTNPathHandler&                                                       GetVariablesPathHandlerMutable();
    const std::string&                                                    GetNodeLabel() const;

private:
    const HTNNodeSnapshotDebug*                                    mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNPathHandler                                                 mVariablesPathHandler;
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

inline const HTNPathHandler& HTNDecompositionNode::GetVariablesPathHandler() const
{
    return mVariablesPathHandler;
}

inline HTNPathHandler& HTNDecompositionNode::GetVariablesPathHandlerMutable()
{
    return mVariablesPathHandler;
}

inline const std::string& HTNDecompositionNode::GetNodeLabel() const
{
    return mNodeLabel;
}
#endif
