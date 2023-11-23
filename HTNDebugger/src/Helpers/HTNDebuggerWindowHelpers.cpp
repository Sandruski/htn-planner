// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Helpers/HTNDebuggerWindowHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Helpers/HTNOperationResult.h"

namespace HTNDebuggerWindowHelpers
{
bool IsOperationSuccessful(const HTNOperationResult inOperationResult)
{
    return (inOperationResult == HTNOperationResult::SUCCEEDED);
}
} // namespace HTNDebuggerWindowHelpers
#endif
