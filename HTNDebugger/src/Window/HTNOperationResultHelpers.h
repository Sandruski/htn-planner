// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

enum class HTNOperationResult : uint8;

namespace HTNOperationResultHelpers
{
// Returns whether an operation is successful
HTN_NODISCARD bool IsOperationSuccessful(const HTNOperationResult inOperationResult);
} // namespace HTNOperationResultHelpers
#endif
