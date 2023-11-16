#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"

namespace HTNDecompositionHelpers
{
// Check if the decomposition step is valid
bool IsDecompositionStepValid(const int32 inDecompositionStep);

// Check if the decomposition step is in the range [min, max)
bool IsDecompositionStepInRange(const int32 inDecompositionStep, const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep);

constexpr int32 kInvalidDecompositionStep = -1;
} // namespace HTNDecompositionPrinterHelpers

namespace HTNDecompositionHelpers
{
inline bool IsDecompositionStepValid(const int32 inDecompositionStep)
{
    return (inDecompositionStep != kInvalidDecompositionStep);
}
} // namespace HTNDecompositionHelpers
#endif
