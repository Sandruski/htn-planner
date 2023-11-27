// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

/**
 * Step in the execution of a node in a decomposition
 */
enum class HTNNodeStep : uint8
{
    START,
    END // RETRY
};
#endif
