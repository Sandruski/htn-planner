#pragma once

#include <array>

class HTNAtom;
class HTNFactArgumentsTable;

namespace HTNWorldStateHelpers
{
// Returns the number of fact arguments that are bound
template<typename It>
unsigned int CountFactArgumentsBound(const It& inFactArgumentsBegin, const It& inFactArgumentsEnd);

// Maximum number of fact arguments
constexpr std::size_t kMaxFactArgumentsNum = 10;

// Maximum number of fact tables
// One more than the maximum number of fact arguments for the fact with zero arguments
constexpr std::size_t kFactArgumentsContainerSize = kMaxFactArgumentsNum + 1;
} // namespace HTNWorldStateHelpers

using HTNFactArguments       = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsContainerSize>;

template<typename It>
inline unsigned int HTNWorldStateHelpers::CountFactArgumentsBound(const It& inFactArgumentsBegin, const It& inFactArgumentsEnd)
{
    unsigned int FactArgumentsBoundNum = 0;

    for (auto It = inFactArgumentsBegin; It != inFactArgumentsEnd; ++It)
    {
        if ((*It).IsSet())
        {
            ++FactArgumentsBoundNum;
        }
    }

    return FactArgumentsBoundNum;
}
