#include "WorldState/Parser/HTNWorldStateParser.h"

#include "HTNAtom.h"
#include "HTNLog.h"
#include "HTNToken.h"
#include "WorldState/HTNWorldState.h"

/*
Backus Naur Form (BNF):
<fact> ::= <identifier> <argument>*
<identifier> ::= 'identifier'
<argument> ::= 'number' | 'string'
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

    const HTNAtom* Identifier = ParseIdentifier(CurrentPosition);
    if (!Identifier)
    {
        return false;
    }

    std::vector<HTNAtom> Arguments;
    while (const HTNAtom* Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(*Argument);
    }

    outWorldState.AddFact(Identifier->GetValue<std::string>().c_str(), Arguments);

    ioPosition = CurrentPosition;

    return true;
}

const HTNAtom* HTNWorldStateParser::ParseIdentifier(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return &IdentifierToken->GetValue();
}

const HTNAtom* HTNWorldStateParser::ParseArgument(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNAtom* Argument = nullptr;

    if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, CurrentPosition))
    {
        Argument = &NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, CurrentPosition))
    {
        Argument = &StringToken->GetValue();
    }

    inPosition = CurrentPosition;

    return Argument;
}
