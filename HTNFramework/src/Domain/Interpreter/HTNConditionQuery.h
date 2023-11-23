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
    virtual ~HTNConditionQueryBase() = default;
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
    static bool Check(const HTNWorldState& inWorldState, const std::string& inFactID, const size inArgumentsIndex, std::vector<HTNAtom>& ioArguments);
};
