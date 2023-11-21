// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Planner/HTNPlannerHook.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Parser/HTNDomainLexerContext.h"
#include "Domain/Parser/HTNDomainParserContext.h"
#include "HTNFileHandler.h"
#include "Helpers/HTNToken.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    const HTNFileHandler DomainFileHandler = HTNFileHandler(inDomainFilePath);
    std::string          DomainFileText;
    if (!DomainFileHandler.ReadFile(DomainFileText))
    {
        HTN_LOG_ERROR("Domain [{}] could not be read", inDomainFilePath);
        return false;
    }

    std::vector<HTNToken> Tokens;
    HTNDomainLexerContext DomainLexerContext = HTNDomainLexerContext(DomainFileText, Tokens);
    if (!mDomainLexer.Lex(DomainLexerContext))
    {
        HTN_LOG_ERROR("Domain [{}] could not be lexed", inDomainFilePath);
        return false;
    }

    mDomainNode.reset();
    HTNDomainParserContext DomainParserContext = HTNDomainParserContext(Tokens, mDomainNode);
    if (!mDomainParser.Parse(DomainParserContext))
    {
        HTN_LOG_ERROR("Domain [{}] could not be parsed", inDomainFilePath);
        return false;
    }

    return true;
}

bool HTNPlannerHook::MakePlan(HTNDecompositionContext& ioDecompositionContext) const
{
    if (!mDomainNode)
    {
        HTN_LOG_ERROR("Domain is null");
        return false;
    }

    if (!mDomainInterpreter.Interpret(ioDecompositionContext))
    {
        HTN_LOG_ERROR("Domain [{}] could not be interpreted", mDomainNode->GetID());
        return false;
    }

    return true;
}
