// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"

#include <string>

#ifdef HTN_DEBUG
class HTNParserContextBase;
#endif

namespace HTNParserHelpers
{
// Name of an anonymous namespace
inline const std::string kAnonymousNamespaceName = "unnamed";

#ifdef HTN_DEBUG
void PrintLastErrorMessage(HTNParserContextBase& ioParserContext);
#endif
} // namespace HTNParserHelpers
