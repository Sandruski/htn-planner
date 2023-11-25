// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Interpreter/HTNIndicesManager.h"
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "HTNCoreMinimal.h"

class HTNEnvironment
{
public:
    void                                     SetVariablesManager(const HTNVariablesManager& inVariablesManager);
    HTN_NODISCARD const HTNVariablesManager& GetVariablesManager() const;
    HTN_NODISCARD HTNVariablesManager&       GetVariablesManagerMutable();

    void                                   SetIndicesManager(const HTNIndicesManager& inIndicesManager);
    HTN_NODISCARD const HTNIndicesManager& GetIndicesManager() const;
    HTN_NODISCARD HTNIndicesManager&       GetIndicesManagerMutable();

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
