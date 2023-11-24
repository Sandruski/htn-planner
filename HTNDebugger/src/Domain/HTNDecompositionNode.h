// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNPathHandler.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNNodeResult;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

class HTNDecompositionNode
{
public:
    HTNDecompositionNode() = default;
    explicit HTNDecompositionNode(const HTNNodeResult& inNodeResult, const std::string& inNodeLabel);
    explicit HTNDecompositionNode(const HTNNodeResult&                                                 inNodeResult,
                                  const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                  const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel);
    explicit HTNDecompositionNode(const HTNNodeResult&                                                  inNodeResult,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                  const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel);

    HTN_NODISCARD const HTNNodeResult*                                                  GetNodeResult() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  GetNodeParameters() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;
    HTN_NODISCARD const HTNPathHandler&                                                 GetVariablesPathHandler() const;
    HTN_NODISCARD HTNPathHandler&                                                       GetVariablesPathHandlerMutable();
    HTN_NODISCARD const std::string&                                                    GetNodeLabel() const;

private:
    const HTNNodeResult*                                           mNodeResult = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNPathHandler                                                 mVariablesPathHandler;
    std::string                                                    mNodeLabel;
};

inline const HTNNodeResult* HTNDecompositionNode::GetNodeResult() const
{
    return mNodeResult;
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
