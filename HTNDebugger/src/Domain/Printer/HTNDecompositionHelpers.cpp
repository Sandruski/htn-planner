// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDecompositionHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
namespace HTNDecompositionHelpers
{
bool IsDecompositionStepInRange(const int32 inDecompositionStep, const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep)
{
    return (inDecompositionStep >= inMinDecompositionStep) && (inDecompositionStep < inMaxDecompositionStep);
}
} // namespace HTNDecompositionHelpers
#endif
