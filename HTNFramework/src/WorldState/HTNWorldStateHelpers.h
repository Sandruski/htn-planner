// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

namespace HTNWorldStateHelpers
{
// Returns the number of fact arguments that are bound
template<typename T>
uint32 CountFactArgumentsBound(const T inFactArgumentsBegin, const T inFactArgumentsEnd);

// Maximum number of fact arguments
inline constexpr size kMaxFactArgumentsNum = 10;

// Maximum number of fact tables
// One more than the maximum number of fact arguments for the fact with zero arguments
inline constexpr size kFactArgumentsContainerSize = kMaxFactArgumentsNum + 1;
} // namespace HTNWorldStateHelpers

namespace HTNWorldStateHelpers
{
template<typename T>
uint32 CountFactArgumentsBound(const T inFactArgumentsBegin, const T inFactArgumentsEnd)
{
    uint32 FactArgumentsBoundNum = 0;

    for (auto It = inFactArgumentsBegin; It != inFactArgumentsEnd; ++It)
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
