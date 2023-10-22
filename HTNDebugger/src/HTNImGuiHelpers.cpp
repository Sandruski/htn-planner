#pragma once

#ifdef HTN_DEBUG
#include "HTNImGuiHelpers.h"

#include "Domain/Interpreter/HTNDecompositionHelpers.h"

namespace HTNImGuiHelpers
{
ImVec4 GetArgumentColor(const std::string& inArgumentID)
{
    if (HTNDecompositionHelpers::IsParameter(inArgumentID))
    {
        return kParameterColor;
    }
    else if (HTNDecompositionHelpers::IsAnyArgument(inArgumentID))
    {
        return kAnyArgumentColor;
    }
    else
    {
        return kArgumentColor;
    }
}
} // namespace HTNImGuiHelpers
#endif
