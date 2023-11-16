#include "WorldState/HTNWorldStatePrinterContext.h"

#ifdef HTN_DEBUG
HTNWorldStatePrinterContext::HTNWorldStatePrinterContext(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter)
    : mWorldState(inWorldState), mTextFilter(inTextFilter)
{
}
#endif
