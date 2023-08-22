#include "Interpreter/HTNParser.h"

#include "HTNLog.h"
#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNConstant.h"
#include "Interpreter/AST/HTNConstants.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNAtom.h"

std::shared_ptr<const HTNDomain> HTNParser::Parse()
{
    unsigned int                           InitialPosition = 0;
    const std::shared_ptr<const HTNDomain> Domain          = ParseDomain(InitialPosition);
    CLOG_HTN_ERROR(!Domain, mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
    return Domain;
}

std::shared_ptr<HTNDomain> HTNParser::ParseDomain(unsigned int& inPosition)
{
    // '(' ':' 'domain' <identifier> 'top_level_domain'* <axiom>* <method>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::HTN_DOMAIN))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_DOMAIN);

    std::vector<std::shared_ptr<const HTNConstants>> Constants;
    while (const std::shared_ptr<const HTNConstants> ConstantsContainer = ParseConstants(CurrentPosition))
    {
        Constants.emplace_back(ConstantsContainer);
    }

    std::vector<std::shared_ptr<const HTNAxiom>> Axioms;
    while (const std::shared_ptr<const HTNAxiom> Axiom = ParseAxiom(CurrentPosition))
    {
        Axioms.emplace_back(Axiom);
    }

    std::vector<std::shared_ptr<const HTNMethod>> Methods;
    while (const std::shared_ptr<const HTNMethod> Method = ParseMethod(CurrentPosition))
    {
        Methods.emplace_back(Method);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNDomain>(IDArgument, Constants, Axioms, Methods, IsTopLevelDomain);
}

std::shared_ptr<HTNConstants> HTNParser::ParseConstants(unsigned int& inPosition)
{
    // '(' ':' 'constants' <identifier>* <constant>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::HTN_CONSTANTS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);

    std::vector<std::shared_ptr<const HTNConstant>> Constants;
    while (const std::shared_ptr<const HTNConstant> Constant = ParseConstant(CurrentPosition))
    {
        Constants.emplace_back(Constant);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstants>(IDArgument, Constants);
}

std::shared_ptr<HTNConstant> HTNParser::ParseConstant(unsigned int& inPosition)
{
    // '(' <identifier> <argument>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> ValueArgument = ParseArgument(CurrentPosition);
    if (!ValueArgument)
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::vector<std::shared_ptr<const HTNValue>> Arguments = {IDArgument, ValueArgument};

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstant>(Arguments);
}

std::shared_ptr<HTNAxiom> HTNParser::ParseAxiom(unsigned int& inPosition)
{
    // '(' ':' 'axiom' '(' <identifier> <argument>* ')' <condition>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::HTN_AXIOM))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments = {IDArgument};
    while (const std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionBase> Condition = ParseCondition(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNAxiom>(Arguments, Condition);
}

std::shared_ptr<HTNMethod> HTNParser::ParseMethod(unsigned int& inPosition)
{
    // '(' ':' 'method' '(' <identifier> <argument>* 'top_level_method'* ')' <branch>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::HTN_METHOD))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments = {IDArgument};
    while (const std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_METHOD);

    std::vector<std::shared_ptr<const HTNBranch>> Branches;
    while (const std::shared_ptr<const HTNBranch> Branch = ParseBranch(CurrentPosition))
    {
        Branches.emplace_back(Branch);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNMethod>(Arguments, Branches, IsTopLevelMethod);
}

std::shared_ptr<HTNBranch> HTNParser::ParseBranch(unsigned int& inPosition)
{
    // '(' <identifier> '(' <condition>* ')' '(' <task>* ')' ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionBase> PreCondition = ParseCondition(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTask>> Tasks;
    while (const std::shared_ptr<const HTNTask> Task = ParseTask(CurrentPosition))
    {
        Tasks.emplace_back(Task);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNBranch>(IDArgument, PreCondition, Tasks);
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseCondition(unsigned int& inPosition)
{
    // '(' (('and' | 'or' | 'alt') <sub-condition>)* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::shared_ptr<HTNConditionBase> Condition;

    if (ParseToken(CurrentPosition, HTNTokenType::AND))
    {
        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (const std::shared_ptr<const HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubConditions.emplace_back(SubCondition);
        }

        Condition = std::make_shared<HTNConditionAnd>(SubConditions);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::OR))
    {
        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (const std::shared_ptr<const HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubConditions.emplace_back(SubCondition);
        }

        Condition = std::make_shared<HTNConditionOr>(SubConditions);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::ALT))
    {
        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (const std::shared_ptr<const HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubConditions.emplace_back(SubCondition);
        }

        Condition = std::make_shared<HTNConditionAlt>(SubConditions);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Condition;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseSubCondition(unsigned int& inPosition)
{
    // <condition> | ('(' (<identifier> <argument>*)* ')')

    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNConditionBase> Condition = ParseCondition(CurrentPosition);
    if (!Condition)
    {
        if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
        {
            return nullptr;
        }

        if (ParseToken(CurrentPosition, HTNTokenType::NOT))
        {
            const std::shared_ptr<const HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition);
            Condition                                                  = std::make_shared<HTNConditionNot>(SubCondition);
        }
        else
        {
            const bool IsAxiom = ParseToken(CurrentPosition, HTNTokenType::HASH);

            if (const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL))
            {
                std::vector<std::shared_ptr<const HTNValue>> Arguments = {IDArgument};
                while (const std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
                {
                    Arguments.emplace_back(Argument);
                }

                if (IsAxiom)
                {
                    Condition = std::make_shared<HTNConditionAxiom>(Arguments);
                }
                else
                {
                    Condition = std::make_shared<HTNCondition>(Arguments);
                }
            }
        }

        if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
        {
            return nullptr;
        }
    }

    inPosition = CurrentPosition;

    return Condition;
}

std::shared_ptr<HTNTask> HTNParser::ParseTask(unsigned int& inPosition)
{
    // '(' '!'* <identifier> <argument>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const HTNTaskType Type = ParseToken(CurrentPosition, HTNTokenType::EXCLAMATION_MARK) ? HTNTaskType::PRIMITIVE : HTNTaskType::COMPOUND;

    const std::shared_ptr<const HTNValue> IDArgument = ParseIdentifier(CurrentPosition, HTNValueType::LITERAL);
    if (!IDArgument)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments = {IDArgument};
    while (const std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNTask>(Arguments, Type);
}

std::shared_ptr<HTNValue> HTNParser::ParseArgument(unsigned int& inPosition)
{
    // ('?' <identifier>) | <number> | <string>

    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNValue> Argument;

    if (ParseToken(CurrentPosition, HTNTokenType::QUESTION_MARK))
    {
        Argument = ParseIdentifier(CurrentPosition, HTNValueType::VARIABLE);
        if (!Argument)
        {
            return nullptr;
        }
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::AT))
    {
        Argument = ParseIdentifier(CurrentPosition, HTNValueType::CONSTANT);
        if (!Argument)
        {
            return nullptr;
        }
    }
    else
    {
        Argument = ParseNumber(CurrentPosition);
        if (!Argument)
        {
            Argument = ParseString(CurrentPosition);
            if (!Argument)
            {
                return nullptr;
            }
        }
    }

    inPosition = CurrentPosition;

    return Argument;
}

std::shared_ptr<HTNValue> HTNParser::ParseIdentifier(unsigned int& inPosition, const HTNValueType inValueType)
{
    // IDENTIFIER

    unsigned int CurrentPosition = inPosition;

    const HTNToken* Identifier = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER);
    if (!Identifier)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValue>(Identifier->GetValue(), inValueType);
}

std::shared_ptr<HTNValue> HTNParser::ParseNumber(unsigned int& inPosition)
{
    // NUMBER

    unsigned int CurrentPosition = inPosition;

    const HTNToken* Number = ParseToken(CurrentPosition, HTNTokenType::NUMBER);
    if (!Number)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValue>(Number->GetValue(), HTNValueType::LITERAL);
}

std::shared_ptr<HTNValue> HTNParser::ParseString(unsigned int& inPosition)
{
    // STRING

    unsigned int CurrentPosition = inPosition;

    const HTNToken* String = ParseToken(CurrentPosition, HTNTokenType::STRING);
    if (!String)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValue>(String->GetValue(), HTNValueType::LITERAL);
}

const HTNToken* HTNParser::ParseToken(unsigned int& inPosition, const HTNTokenType inTokenType)
{
    const HTNToken* Token = GetToken(inPosition);
    if (!Token)
    {
#if HTN_DEBUG
        mLastErrorMessage = std::format("Token at [{}] is out of bounds [{}]", inPosition, mTokens.size());
        mLastErrorRow     = mLastErrorColumn - 1;
#endif
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
#if HTN_DEBUG
        mLastErrorMessage = std::format("Token [{}] is of type [{}] instead of [{}]", Token->GetLexeme(), GetTokenTypeString(Token->GetType()),
                                        GetTokenTypeString(inTokenType));
        mLastErrorRow     = Token->GetRow();
        mLastErrorColumn  = Token->GetColumn();
#endif
        return nullptr;
    }

    ++inPosition;

    return Token;
}
