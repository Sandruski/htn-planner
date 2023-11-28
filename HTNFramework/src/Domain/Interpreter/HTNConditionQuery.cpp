// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Interpreter/HTNConditionQuery.h"

#include "WorldState/HTNWorldState.h"

HTNConditionQueryBase::~HTNConditionQueryBase() = default;

bool HTNConditionWorldStateQuery::Check(const HTNWorldState& inWorldState, const std::string& inFactID, const size inFactArgumentsIndex,
                                        std::vector<HTNAtom>& ioFactArguments) const
{
    return inWorldState.CheckIndex(inFactID, inFactArgumentsIndex, ioFactArguments);
}
