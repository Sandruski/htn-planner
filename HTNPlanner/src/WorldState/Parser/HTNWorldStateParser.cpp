#include "WorldState/Parser/HTNWorldStateParser.h"

#include "Helpers/HTNToken.h"
#include "Parser/HTNParserHelpers.h"
#include "WorldState/HTNWorldState.h"
#include "WorldState/Parser/HTNWorldStateParserContext.h"

/*
Backus Naur Form (BNF):
<fact> ::= <identifier> <argument>*
<identifier> ::= 'identifier'
<argument> ::= ('(' <argument>+ ')') | 'true' | 'false' | 'number' | 'string'
*/

bool HTNWorldStateParser::Parse(HTNWorldStateParserContext& ioWorldStateParserContext) const
{
    OPTICK_EVENT("ParseWorldState");

    HTNWorldState& WorldState = ioWorldStateParserContext.GetWorldStateMutable();
    while (!ParseToken(HTNTokenType::END_OF_FILE, ioWorldStateParserContext))
    {
        if (!ParseFact(WorldState, ioWorldStateParserContext))
        {
#ifdef HTN_DEBUG
            HTNParserHelpers::PrintLastErrorMessage(ioWorldStateParserContext);
#endif
            return false;
        }
    }

    return true;
}

bool HTNWorldStateParser::ParseFact(HTNWorldState& outWorldState, HTNWorldStateParserContext& ioWorldStateParserContext) const
{
    OPTICK_EVENT("ParseFact");

    const uint32 StartPosition = ioWorldStateParserContext.GetPosition();

    HTNAtom Identifier;
    if (!ParseIdentifier(Identifier, ioWorldStateParserContext))
    {
        ioWorldStateParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<HTNAtom> Arguments;
    HTNAtom              Argument;
    while (ParseArgument(Argument, ioWorldStateParserContext))
    {
        Arguments.emplace_back(Argument);
    }

    outWorldState.AddFact(Identifier.GetValue<std::string>().c_str(), Arguments.begin(), Arguments.end());

    return true;
}

bool HTNWorldStateParser::ParseIdentifier(HTNAtom& outIdentifier, HTNWorldStateParserContext& ioWorldStateParserContext) const
{
    OPTICK_EVENT("ParseIdentifier");

    const uint32 StartPosition = ioWorldStateParserContext.GetPosition();

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, ioWorldStateParserContext);
    if (!IdentifierToken)
    {
        ioWorldStateParserContext.SetPosition(StartPosition);
        return false;
    }

    outIdentifier = IdentifierToken->GetValue();

    return true;
}

bool HTNWorldStateParser::ParseArgument(HTNAtom& outArgument, HTNWorldStateParserContext& ioWorldStateParserContext) const
{
    OPTICK_EVENT("ParseArgument");

    const uint32 StartPosition = ioWorldStateParserContext.GetPosition();

    HTNAtom Argument;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioWorldStateParserContext))
    {
        HTNAtom ArgumentElement;
        while (ParseArgument(ArgumentElement, ioWorldStateParserContext))
        {
            Argument.AddListElement(ArgumentElement);
        }

        if (Argument.IsListEmpty())
        {
            ioWorldStateParserContext.SetPosition(StartPosition);
            return false;
        }

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioWorldStateParserContext))
        {
            ioWorldStateParserContext.SetPosition(StartPosition);
            return false;
        }
    }
    else if (const HTNToken* TrueToken = ParseToken(HTNTokenType::TRUE, ioWorldStateParserContext))
    {
        Argument = TrueToken->GetValue();
    }
    else if (const HTNToken* FalseToken = ParseToken(HTNTokenType::FALSE, ioWorldStateParserContext))
    {
        Argument = FalseToken->GetValue();
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, ioWorldStateParserContext))
    {
        Argument = NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, ioWorldStateParserContext))
    {
        Argument = StringToken->GetValue();
    }
    else
    {
        ioWorldStateParserContext.SetPosition(StartPosition);
        return false;
    }

    outArgument = Argument;

    return true;
}
