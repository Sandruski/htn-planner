#include "Planner/HTNPlannerHook.h"

#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Parser/HTNDomainLexer.h"
#include "Domain/Parser/HTNDomainParser.h"
#include "Domain/Parser/HTNDomainValidator.h"
#include "HTNFileHandler.h"
#include "HTNToken.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    const HTNFileHandler DomainFileHandler = HTNFileHandler(inDomainFilePath);
    std::string          DomainFileText;
    if (!DomainFileHandler.ReadFile(DomainFileText))
    {
        LOG_ERROR("Domain [{}] could not be read", inDomainFilePath);
        return false;
    }

    HTNDomainLexer        DomainLexer = HTNDomainLexer(DomainFileText);
    std::vector<HTNToken> Tokens;
    if (!DomainLexer.Lex(Tokens))
    {
        LOG_ERROR("Domain [{}] could not be lexed", inDomainFilePath);
        return false;
    }

    HTNDomainParser                      DomainParser = HTNDomainParser(Tokens);
    std::shared_ptr<const HTNDomainNode> DomainNode;
    if (!DomainParser.Parse(DomainNode))
    {
        LOG_ERROR("Domain [{}] could not be parsed", inDomainFilePath);
        return false;
    }

    HTNDomainValidator DomainValidator = HTNDomainValidator(DomainNode);
    if (!DomainValidator.Validate())
    {
        LOG_ERROR("Domain [{}] could not be validated", DomainNode->GetID());
        return false;
    }

    mDomainNode = DomainNode;

    return true;
}

bool HTNPlannerHook::MakePlan(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const
{
    if (!mDomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    HTNDomainInterpreter DomainInterpreter = HTNDomainInterpreter(mDomainNode, inEntryPointName, ioDecompositionContext);
    if (!DomainInterpreter.Interpret())
    {
        LOG_ERROR("Domain [{}] could not be interpreted", mDomainNode->GetID());
        return false;
    }

    return true;
}
