// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

enum class HTNOperationResult : uint8;

namespace HTNDebuggerWindowHelpers
{
bool IsOperationSuccessful(const HTNOperationResult inOperationResult);
} // namespace HTNDebuggerWindowHelpers
#endif
