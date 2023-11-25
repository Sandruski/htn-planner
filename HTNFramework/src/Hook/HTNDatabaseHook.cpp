// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Hook/HTNDatabaseHook.h"

#include "Core/HTNFileReader.h"
#include "Core/HTNToken.h"
#include "WorldState/Parser/HTNWorldStateLexerContext.h"
#include "WorldState/Parser/HTNWorldStateParserContext.h"

bool HTNDatabaseHook::ParseWorldStateFile(const std::string& inWorldStateFilePath)
{
    const HTNFileReader WorldStateFileReader = HTNFileReader(inWorldStateFilePath);
    std::string         WorldStateText;
    if (!WorldStateFileReader.ReadFile(WorldStateText))
    {
        HTN_LOG_ERROR("World state [{}] could not be read", inWorldStateFilePath);
        return false;
    }

    std::vector<HTNToken>     Tokens;
    HTNWorldStateLexerContext WorldStateLexerContext = HTNWorldStateLexerContext(WorldStateText, Tokens);
    if (!mWorldStateLexer.Lex(WorldStateLexerContext))
    {
        HTN_LOG_ERROR("World state [{}] could not be lexed", inWorldStateFilePath);
        return false;
    }

    mWorldState                                        = HTNWorldState();
    HTNWorldStateParserContext WorldStateParserContext = HTNWorldStateParserContext(Tokens, mWorldState);
    if (!mWorldStateParser.Parse(WorldStateParserContext))
    {
        HTN_LOG_ERROR("World state [{}] could not be parsed", inWorldStateFilePath);
        return false;
    }

    return true;
}
