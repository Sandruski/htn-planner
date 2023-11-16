#pragma once

#include "HTNCoreMinimal.h"

#include <string>

#ifdef HTN_DEBUG
class HTNParserContextBase;
#endif

namespace HTNParserHelpers
{
// Name of an anonymous namespace
const std::string kAnonymousNamespaceName = "unnamed";

#ifdef HTN_DEBUG
void PrintLastErrorMessage(HTNParserContextBase& ioParserContext);
#endif
} // namespace HTNParserHelpers
