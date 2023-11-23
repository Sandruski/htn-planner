// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNIndices.h"
#include "Domain/Interpreter/HTNVariables.h"

class HTNEnvironment
{
public:
    void                SetVariables(const HTNVariables& inVariables);
    const HTNVariables& GetVariables() const;
    HTNVariables&       GetVariablesMutable();

    void              SetIndices(const HTNIndices& inIndices);
    const HTNIndices& GetIndices() const;
    HTNIndices&       GetIndicesMutable();

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
