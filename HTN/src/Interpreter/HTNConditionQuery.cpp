#include "Interpreter/HTNConditionQuery.h"

#include "WorldState/HTNWorldState.h"

bool HTNConditionQueryWorldState::Check(const HTNWorldState& inWorldState, const int inIndex) const
{
    return inWorldState.CheckIndex(mKey.c_str(), inIndex, mArguments);
}
