// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>
#include <vector>

class HTNAtom;
class HTNWorldState;

/**
 * Base class for condition queries
 */
class HTNConditionQueryBase
{
public:
    virtual ~HTNConditionQueryBase() = 0;
};

/**
 * Condition that performs a query on the world state
 */
class HTNConditionWorldStateQuery final : public HTNConditionQueryBase
{
public:
    // Queries the given fact in the world state
    HTN_NODISCARD bool Check(const HTNWorldState& inWorldState, const std::string& inFactID, const size inFactArgumentsIndex,
                             std::vector<HTNAtom>& ioFactArguments) const;
};
