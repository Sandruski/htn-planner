#pragma once

#include "HTNPlannerMinimal.h"

class HTNParserContextBase;
class HTNToken;
enum class HTNTokenType : uint8;

class HTNParserBase
{
public:
    virtual ~HTNParserBase() = 0;

protected:
    const HTNToken* ParseToken(const HTNTokenType inTokenType, HTNParserContextBase& ioParserContext) const;
};
