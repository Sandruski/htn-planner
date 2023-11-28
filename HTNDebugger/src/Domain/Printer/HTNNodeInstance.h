// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNPathManager.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>
#include <vector>

class HTNNodeSnapshot;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

/**
 * Instance of a node in a decomposition printer
 * - TODO salvarez Make this a base class with three derived classes - one with node parameters, one with node arguments, and one without either of
 * the two
 */
class HTNNodeInstance
{
public:
    HTNNodeInstance() = default;
    explicit HTNNodeInstance(const HTNNodeSnapshot& inNodeSnapshot, const std::string& inNodeLabel);
    explicit HTNNodeInstance(const HTNNodeSnapshot&                                               inNodeSnapshot,
                             const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                             const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel);
    explicit HTNNodeInstance(const HTNNodeSnapshot&                                                inNodeSnapshot,
                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                             const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel);

    // Returns the snapshot of the node
    HTN_NODISCARD const HTNNodeSnapshot* GetNodeSnapshot() const;

    // Returns the parameters of the node
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& GetNodeParameters() const;

    // Returns the arguments of the node
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetNodeArguments() const;

    // Returns the variables path manager
    HTN_NODISCARD const HTNPathManager& GetVariablesPathManager() const;

    // Returns the variables path manager
    HTN_NODISCARD HTNPathManager& GetVariablesPathManagerMutable();

    // Returns the label of the node
    HTN_NODISCARD const std::string& GetNodeLabel() const;

private:
    const HTNNodeSnapshot*                                         mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNVariableExpressionNode>>  mNodeParameters;
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mNodeArguments;
    HTNPathManager                                                 mVariablesPathManager;
    std::string                                                    mNodeLabel;
};

inline const HTNNodeSnapshot* HTNNodeInstance::GetNodeSnapshot() const
{
    return mNodeSnapshot;
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
