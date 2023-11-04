#pragma once

#ifdef HTN_DEBUG
namespace HTNDecompositionHelpers
{
bool IsDecompositionStepInRange(const int inDecompositionStep, const int inMinDecompositionStep, const int inMaxDecompositionStep)
{
    return ((inDecompositionStep >= inMinDecompositionStep) && (inDecompositionStep < inMaxDecompositionStep));
}
} // namespace HTNDecompositionHelpers
#endif
