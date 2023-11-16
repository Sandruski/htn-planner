#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"

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
