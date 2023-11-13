#pragma once

#ifdef HTN_DEBUG
enum class HTNDecompositionTooltipMode : unsigned char
{
    NONE,
    REGULAR,
    FULL
};

namespace HTNDecompositionHelpers
{
// Check if the decomposition step is valid
bool IsDecompositionStepValid(const int inDecompositionStep);

// Check if the decomposition step is in the range [min, max)
bool IsDecompositionStepInRange(const int inDecompositionStep, const int inMinDecompositionStep, const int inMaxDecompositionStep);

constexpr int kInvalidDecompositionStep = -1;
} // namespace HTNDecompositionPrinterHelpers

namespace HTNDecompositionHelpers
{
inline bool IsDecompositionStepValid(const int inDecompositionStep)
{
    return (inDecompositionStep != kInvalidDecompositionStep);
}
} // namespace HTNDecompositionHelpers
#endif
