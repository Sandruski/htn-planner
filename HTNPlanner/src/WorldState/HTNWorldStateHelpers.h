#pragma once

#include <cstddef>

namespace HTNWorldStateHelpers
{
// Returns the number of fact arguments that are bound
template<typename T>
unsigned int CountFactArgumentsBound(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd);

// Maximum number of fact arguments
constexpr std::size_t kMaxFactArgumentsNum = 10;

// Maximum number of fact tables
// One more than the maximum number of fact arguments for the fact with zero arguments
constexpr std::size_t kFactArgumentsContainerSize = kMaxFactArgumentsNum + 1;
} // namespace HTNWorldStateHelpers

template<typename T>
unsigned int HTNWorldStateHelpers::CountFactArgumentsBound(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd)
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
