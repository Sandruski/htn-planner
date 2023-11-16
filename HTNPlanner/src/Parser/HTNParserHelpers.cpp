#include "Parser/HTNParserHelpers.h"

#ifdef HTN_DEBUG
#include "Parser/HTNParserContextBase.h"
#endif

namespace HTNParserHelpers
{
#ifdef HTN_DEBUG
void PrintLastErrorMessage(HTNParserContextBase& ioParserContext)
{
    const int32          LastErrorRow     = ioParserContext.GetLastErrorRow();
    const int32          LastErrorColumn  = ioParserContext.GetLastErrorColumn();
    const std::string& LastErrorMessage = ioParserContext.GetLastErrorMessage();
    LOG_HTN_ERROR(LastErrorRow, LastErrorColumn, "{}", LastErrorMessage);
}
#endif
} // namespace HTNParserHelpers
