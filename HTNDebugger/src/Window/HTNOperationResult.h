// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

/**
 * Result of an operation
 */
enum class HTNOperationResult : uint8
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};
#endif
