// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Interpreter/HTNIndicesManager.h"
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "HTNCoreMinimal.h"

/**
 * Variables and indices of a decomposition
 */
class HTNEnvironment
{
public:
    // Sets the variables manager to the given one
    void SetVariablesManager(const HTNVariablesManager& inVariablesManager);

    // Returns the variables manager
    HTN_NODISCARD const HTNVariablesManager& GetVariablesManager() const;

    // Returns the variables manager
    HTN_NODISCARD HTNVariablesManager& GetVariablesManagerMutable();

    // Sets the indices manager to the given one
    void SetIndicesManager(const HTNIndicesManager& inIndicesManager);

    // Returns the indices manager
    HTN_NODISCARD const HTNIndicesManager& GetIndicesManager() const;

    // Returns the indices manager
    HTN_NODISCARD HTNIndicesManager& GetIndicesManagerMutable();

private:
    HTNVariablesManager mVariablesManager;
    HTNIndicesManager   mIndicesManager;
};

inline void HTNEnvironment::SetVariablesManager(const HTNVariablesManager& inVariablesManager)
{
    mVariablesManager = inVariablesManager;
}

inline const HTNVariablesManager& HTNEnvironment::GetVariablesManager() const
{
    return mVariablesManager;
}

inline HTNVariablesManager& HTNEnvironment::GetVariablesManagerMutable()
{
    return mVariablesManager;
}

inline void HTNEnvironment::SetIndicesManager(const HTNIndicesManager& inIndicesManager)
{
    mIndicesManager = inIndicesManager;
}

inline const HTNIndicesManager& HTNEnvironment::GetIndicesManager() const
{
    return mIndicesManager;
}

inline HTNIndicesManager& HTNEnvironment::GetIndicesManagerMutable()
{
    return mIndicesManager;
}
