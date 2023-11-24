// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>
#include <vector>

class HTNAtom;
class HTNWorldState;

class HTNConditionQueryBase
{
public:
    virtual ~HTNConditionQueryBase() = 0;
};

/**
 * Condition world state query
 * - Queries the arguments in the database
 * - Binds unbound arguments
 */
class HTNConditionQueryWorldState final : public HTNConditionQueryBase
{
public:
    // Check if the condition is true
    HTN_NODISCARD bool Check(const HTNWorldState& inWorldState, const std::string& inFactID, const size inArgumentsIndex,
                             std::vector<HTNAtom>& ioArguments) const;
};
