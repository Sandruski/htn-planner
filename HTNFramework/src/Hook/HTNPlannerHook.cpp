// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Hook/HTNPlannerHook.h"

#include "Core/HTNFileReader.h"
#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Parser/HTNDomainLexerContext.h"
#include "Domain/Parser/HTNDomainParserContext.h"
#include "Parser/HTNToken.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    mDomainNode.reset();

    const HTNFileReader DomainFileReader = HTNFileReader(inDomainFilePath);
    std::string         DomainFileText;
    if (!DomainFileReader.ReadFile(DomainFileText))
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
