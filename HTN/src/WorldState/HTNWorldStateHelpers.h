#pragma once

#include <array>

class HTNAtom;
class HTNFactArgumentsTable;

namespace HTNWorldStateHelpers
{
// Maximum number of fact arguments
constexpr std::size_t kMaxFactArgumentsNum = 10;

// Maximum number of fact tables
// One more than the maximum number of fact arguments for the fact with zero arguments
constexpr std::size_t kFactArgumentsContainerSize = kMaxFactArgumentsNum + 1;
} // namespace HTNWorldStateHelpers

using HTNFactArguments      = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
