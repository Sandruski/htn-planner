#include "Interpreter/HTNConditionQuery.h"

#include "WorldState/HTNWorldState.h"

bool HTNConditionQueryWorldState::Check(const HTNWorldState& inWorldState, const std::string& inFactID, const std::size_t inArgumentsIndex,
                                      std::vector<HTNAtom>& ioArguments)
{
    return inWorldState.CheckIndex(inFactID, inArgumentsIndex, ioArguments);
}
