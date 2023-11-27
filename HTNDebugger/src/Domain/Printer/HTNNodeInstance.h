// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNPathManager.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNNodeResult;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

/**
 *
 * - TODO salvarez Make this a base class with three derived classes - one with node parameters, one with node arguments, and one without either of
 * the two
 */
class HTNNodeInstance
{
public:
    HTNNodeInstance() = default;
    explicit HTNNodeInstance(const HTNNodeResult& inNodeResult, const std::string& inNodeLabel);
    explicit HTNNodeInstance(const HTNNodeResult& inNodeResult, const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                             const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel);
    explicit HTNNodeInstance(const HTNNodeResult& inNodeResult, const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                             const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel);

    HTN_NODISCARD const HTNNodeResult* GetNodeResult() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& GetNodeParameters() const;
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;
    HTN_NODISCARD const HTNPathManager&                                                 GetVariablesPathManager() const;
    HTN_NODISCARD HTNPathManager&                                                       GetVariablesPathManagerMutable();
    HTN_NODISCARD const std::string& GetNodeLabel() const;

private:
    const HTNNodeResult*                                           mNodeResult = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNPathManager                                                 mVariablesPathManager;
    std::string                                                    mNodeLabel;
};

inline const HTNNodeResult* HTNNodeInstance::GetNodeResult() const
{
    return mNodeResult;
}

inline const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& HTNNodeInstance::GetNodeParameters() const
{
    return mNodeParameters;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNNodeInstance::GetNodeArguments() const
{
    return mNodeArguments;
}

inline const HTNPathManager& HTNNodeInstance::GetVariablesPathManager() const
{
    return mVariablesPathManager;
}

inline HTNPathManager& HTNNodeInstance::GetVariablesPathManagerMutable()
{
    return mVariablesPathManager;
}

inline const std::string& HTNNodeInstance::GetNodeLabel() const
{
    return mNodeLabel;
}
#endif
