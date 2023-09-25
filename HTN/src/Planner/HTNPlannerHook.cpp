#include "Planner/HTNPlannerHook.h"

#include "HTNLog.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNTaskResult.h"
#include "Parser/HTNLexer.h"
#include "Parser/HTNParser.h"

#include <fstream>
#include <sstream>

namespace
{
bool ReadFile(const std::string& inPath, std::string& outText)
{
    // Open file
    std::ifstream File;
    File.open(inPath);
    if (!File.is_open())
    {
        LOG_ERROR("File [{}] could not be opened", inPath);
        return false;
    }

    // Read file
    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    outText = Buffer.str();

    // Close file
    File.close();

    return true;
}
} // namespace

HTNPlannerHook::~HTNPlannerHook() = default;

bool HTNPlannerHook::ParseDomain(const std::string& inDomainPath)
{
    std::string Text;
    if (!ReadFile(inDomainPath, Text))
    {
        LOG_ERROR("File [{}] could not be read", inDomainPath);
        return false;
    }

    // Lex
    HTNLexer              Lexer = HTNLexer(Text);
    std::vector<HTNToken> Tokens;
    if (!Lexer.Lex(Tokens))
    {
        LOG_ERROR("Lex failed");
        return false;
    }

    // Parse
    HTNParser Parser = HTNParser(Tokens);
    mDomainNode      = Parser.Parse();
    if (!mDomainNode)
    {
        LOG_ERROR("Parse failed");
        return false;
    }

    return true;
}

std::vector<HTNTaskResult> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const
{
    // Interpret
    HTNInterpreter             Interpreter = HTNInterpreter(mDomainNode, inEntryPointName, inWorldState);
    std::vector<HTNTaskResult> Plan;
    if (!Interpreter.Interpret(Plan))
    {
        LOG_ERROR("Interpret failed");
        return {};
    }

    return Plan;
}
