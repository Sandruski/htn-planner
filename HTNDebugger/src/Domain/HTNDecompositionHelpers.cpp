// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
namespace HTNDecompositionHelpers
{
bool IsDecompositionStepInRange(const int32 inDecompositionStep, const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep)
{
    return ((inDecompositionStep >= inMinDecompositionStep) && (inDecompositionStep < inMaxDecompositionStep));
}
} // namespace HTNDecompositionHelpers
#endif
