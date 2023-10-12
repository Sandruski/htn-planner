#include "WorldState/Parser/HTNWorldStateParser.h"

#include "HTNToken.h"
#include "WorldState/HTNWorldState.h"

/*
Backus Naur Form (BNF):
<fact> ::= <identifier> <argument>*
<identifier> ::= 'identifier'
<argument> ::= ('(' <argument>+ ')') | 'true' | 'false' | 'number' | 'string'
*/

bool HTNWorldStateParser::Parse(HTNWorldState& outWorldState)
{
    unsigned int CurrentPosition = 0;

    while (!ParseToken(HTNTokenType::END_OF_FILE, CurrentPosition))
    {
        if (!ParseFact(outWorldState, CurrentPosition))
        {
            LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
            return false;
        }
    }

    return true;
}

bool HTNWorldStateParser::ParseFact(HTNWorldState& outWorldState, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    HTNAtom Identifier;
    if (!ParseIdentifier(Identifier, CurrentPosition))
    {
        return false;
    }

    std::vector<HTNAtom> Arguments;
    HTNAtom              Argument;
    while (ParseArgument(Argument, CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    outWorldState.AddFact(Identifier.GetValue<std::string>().c_str(), Arguments.begin(), Arguments.end());

    ioPosition = CurrentPosition;

    return true;
}

bool HTNWorldStateParser::ParseIdentifier(HTNAtom& outIdentifier, unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return false;
    }

    outIdentifier = IdentifierToken->GetValue();
    inPosition    = CurrentPosition;

    return true;
}

bool HTNWorldStateParser::ParseArgument(HTNAtom& outArgument, unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    HTNAtom Argument;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        HTNAtom ArgumentElement;
        while (ParseArgument(ArgumentElement, CurrentPosition))
        {
            Argument.AddListElement(ArgumentElement);
        }

        if (Argument.IsListEmpty())
        {
            return false;
        }

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
        {
            return false;
        }
    }
    else if (const HTNToken* TrueToken = ParseToken(HTNTokenType::TRUE, CurrentPosition))
    {
        Argument = TrueToken->GetValue();
    }
    else if (const HTNToken* FalseToken = ParseToken(HTNTokenType::FALSE, CurrentPosition))
    {
        Argument = FalseToken->GetValue();
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, CurrentPosition))
    {
        Argument = NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, CurrentPosition))
    {
        Argument = StringToken->GetValue();
    }
    else
    {
        return false;
    }

    outArgument = Argument;
    inPosition  = CurrentPosition;

    return true;
}
