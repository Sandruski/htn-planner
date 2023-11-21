// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"

enum class HTNOperationResult : uint8
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};

namespace HTNDebuggerWindowHelpers
{
bool IsOperationSuccessful(const HTNOperationResult inOperationResult);
} // namespace HTNDebuggerWindowHelpers

namespace HTNDebuggerWindowHelpers
{
inline bool IsOperationSuccessful(const HTNOperationResult inOperationResult)
{
    return (inOperationResult == HTNOperationResult::SUCCEEDED);
}
} // namespace HTNDebuggerWindowHelpers
#endif
