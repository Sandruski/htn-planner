// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

namespace HTNDecompositionHelpers
{
// Returns whether the decomposition step is valid
HTN_NODISCARD bool IsDecompositionStepValid(const int32 inDecompositionStep);

// Returns whether the decomposition step is in the range [min, max)
HTN_NODISCARD bool IsDecompositionStepInRange(const int32 inDecompositionStep, const int32 inMinDecompositionStep,
                                              const int32 inMaxDecompositionStep);

inline constexpr int32 kInvalidDecompositionStep = -1;
} // namespace HTNDecompositionHelpers

namespace HTNDecompositionHelpers
{
inline bool IsDecompositionStepValid(const int32 inDecompositionStep)
{
    return inDecompositionStep != kInvalidDecompositionStep;
}
} // namespace HTNDecompositionHelpers
#endif
