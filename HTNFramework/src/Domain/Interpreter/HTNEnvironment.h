// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNIndices.h"
#include "Domain/Interpreter/HTNVariables.h"

class HTNEnvironment
{
public:
    void                SetVariables(const HTNVariables& inVariables);
    HTN_NODISCARD const HTNVariables& GetVariables() const;
    HTN_NODISCARD HTNVariables&       GetVariablesMutable();

    void              SetIndices(const HTNIndices& inIndices);
    HTN_NODISCARD const HTNIndices& GetIndices() const;
    HTN_NODISCARD HTNIndices&       GetIndicesMutable();

private:
    HTNVariables mVariables;
    HTNIndices   mIndices;
};

inline void HTNEnvironment::SetVariables(const HTNVariables& inVariables)
{
    mVariables = inVariables;
}

inline const HTNVariables& HTNEnvironment::GetVariables() const
{
    return mVariables;
}

inline HTNVariables& HTNEnvironment::GetVariablesMutable()
{
    return mVariables;
}

inline void HTNEnvironment::SetIndices(const HTNIndices& inIndices)
{
    mIndices = inIndices;
}

inline const HTNIndices& HTNEnvironment::GetIndices() const
{
    return mIndices;
}

inline HTNIndices& HTNEnvironment::GetIndicesMutable()
{
    return mIndices;
}
