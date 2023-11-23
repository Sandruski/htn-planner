// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNConditionQuery.h"

#include "WorldState/HTNWorldState.h"

bool HTNConditionQueryWorldState::Check(const HTNWorldState& inWorldState, const std::string& inFactID, const size inArgumentsIndex,
                                      std::vector<HTNAtom>& ioArguments)
{
    return inWorldState.CheckIndex(inFactID, inArgumentsIndex, ioArguments.begin(), ioArguments.end());
}
