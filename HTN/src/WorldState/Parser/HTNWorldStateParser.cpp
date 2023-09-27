#include "WorldState/Parser/HTNWorldStateParser.h"

#include "WorldState/HTNWorldState.h"

// TODO salvarez Implement lexer for world state (; and ,)
bool HTNWorldStateParser::Parse(HTNWorldState& ioWorldState)
{
    unsigned int CurrentPosition = 0;
    if (!ParseFacts(ioWorldState, CurrentPosition))
    {
#if HTN_DEBUG
        LogLastError();
#endif
        return false;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, CurrentPosition))
    {
#if HTN_DEBUG
        LogLastError();
#endif
        return false;
    }

    return true;
}

bool HTNWorldStateParser::ParseFacts(HTNWorldState& ioWorldState, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    while (!ParseToken(HTNTokenType::END_OF_LINE, CurrentPosition))
    {
        ParseFact(ioWorldState, CurrentPosition);
    }

    return true;
}

bool HTNWorldStateParser::ParseFact(HTNWorldState& ioWorldState, unsigned int& ioPosition)
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
        // TODO salvarez If not bound it is invalid
        Arguments.emplace_back(*Argument);
    }

    ioWorldState.AddFact(Identifier->GetValue<std::string>().c_str(), Arguments);

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

    if (!ParseToken(HTNTokenType::SEMICOLON, CurrentPosition))
    {
        return nullptr;
    }

    const HTNAtom* Identifier = ParseIdentifier(CurrentPosition);
    if (!Identifier)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Identifier;
}
