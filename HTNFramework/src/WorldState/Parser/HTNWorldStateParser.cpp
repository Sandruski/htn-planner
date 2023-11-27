// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "WorldState/Parser/HTNWorldStateParser.h"

#include "Parser/HTNParserHelpers.h"
#include "Parser/HTNToken.h"
#include "Parser/HTNTokenType.h"
#include "WorldState/HTNWorldState.h"
#include "WorldState/Parser/HTNWorldStateParserContext.h"

/*
 * Backus Naur Form (BNF)
 * <fact> ::= <identifier> <argument>*
 * <identifier> ::= 'identifier'
 * <argument> ::= ('(' <argument>+ ')') | 'true' | 'false' | 'number' | 'string'
 */

bool HTNWorldStateParser::Parse(HTNWorldStateParserContext& ioWorldStateParserContext) const
{
    OPTICK_EVENT("ParseWorldState");

    HTNWorldState& WorldState = ioWorldStateParserContext.GetWorldStateMutable();
    while (!ParseToken(HTNTokenType::END_OF_FILE, ioWorldStateParserContext))
    {
        if (!ParseFact(ioWorldStateParserContext, WorldState))
        {
#ifdef HTN_DEBUG
            HTNParserHelpers::PrintLastError(ioWorldStateParserContext);
#endif
            return false;
        }
    }

    return true;
}

bool HTNWorldStateParser::ParseFact(HTNWorldStateParserContext& ioWorldStateParserContext, HTNWorldState& outWorldState) const
{
    OPTICK_EVENT("ParseFact");

    const uint32 StartPosition = ioWorldStateParserContext.GetPosition();

    HTNAtom Identifier;
    if (!ParseIdentifier(ioWorldStateParserContext, Identifier))
    {
        ioWorldStateParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<HTNAtom> Arguments;
    HTNAtom              Argument;
    while (ParseArgument(ioWorldStateParserContext, Argument))
    {
        Arguments.emplace_back(Argument);
    }

    outWorldState.AddFact(Identifier.GetValue<std::string>().c_str(), Arguments);

    return true;
}

bool HTNWorldStateParser::ParseIdentifier(HTNWorldStateParserContext& ioWorldStateParserContext, HTNAtom& outIdentifier) const
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

bool HTNWorldStateParser::ParseArgument(HTNWorldStateParserContext& ioWorldStateParserContext, HTNAtom& outArgument) const
{
    OPTICK_EVENT("ParseArgument");

    const uint32 StartPosition = ioWorldStateParserContext.GetPosition();

    HTNAtom Argument;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioWorldStateParserContext))
    {
        HTNAtom ArgumentElement;
        while (ParseArgument(ioWorldStateParserContext, ArgumentElement))
        {
            Argument.PushBackElementToList(ArgumentElement);
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
