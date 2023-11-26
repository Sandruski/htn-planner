// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#ifdef HTN_DEBUG
class HTNParserContextBase;
#endif

namespace HTNParserHelpers
{
#ifdef HTN_DEBUG
void PrintLastError(const HTNParserContextBase& inParserContext);
#endif
} // namespace HTNParserHelpers
