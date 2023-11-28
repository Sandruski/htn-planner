// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "WorldState/Printer/HTNWorldStatePrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNWorldStatePrinterContext::HTNWorldStatePrinterContext(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter)
    : mWorldState(inWorldState), mTextFilter(inTextFilter)
{
}
#endif
