// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

namespace HTNWorldStateHelpers
{
// Returns the number of fact arguments that are bound
template<typename T>
HTN_NODISCARD uint32 CountFactArgumentsBound(const T& inFactArguments);

// Maximum number of fact arguments
inline constexpr size kMaxFactArgumentsNum = 10;

// Maximum number of fact tables
// One more than the maximum number of fact arguments for the fact with zero arguments
inline constexpr size kFactArgumentsContainerSize = kMaxFactArgumentsNum + 1;
} // namespace HTNWorldStateHelpers

namespace HTNWorldStateHelpers
{
template<typename T>
uint32 CountFactArgumentsBound(const T& inFactArguments)
{
    uint32 FactArgumentsBoundNum = 0;

    const T::const_iterator ItEnd = std::cend(inFactArguments);
    for (auto It = std::cbegin(inFactArguments); It != ItEnd; ++It)
    {
        const HTNAtom& FactArgument = *It;
        if (FactArgument.IsBound())
        {
            ++FactArgumentsBoundNum;
        }
    }

    return FactArgumentsBoundNum;
}
} // namespace HTNWorldStateHelpers
