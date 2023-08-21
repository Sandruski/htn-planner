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

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_DOMAIN);

    const std::shared_ptr<HTNDomain> Domain = std::make_shared<HTNDomain>(std::move(Name), IsTopLevelDomain);

    std::vector<std::shared_ptr<const HTNConstants>> ConstantsContainer;
    while (std::shared_ptr<HTNConstants> Constants = ParseConstants(CurrentPosition))
    {
        Constants->SetScope(Domain);
        ConstantsContainer.emplace_back(Constants);
    }

    Domain->SetConstants(ConstantsContainer);

    std::vector<std::shared_ptr<const HTNAxiom>> Axioms;
    while (std::shared_ptr<HTNAxiom> Axiom = ParseAxiom(CurrentPosition))
    {
        Axiom->SetScope(Domain);
        Axioms.emplace_back(Axiom);
    }

    Domain->SetAxioms(Axioms);

    std::vector<std::shared_ptr<const HTNMethod>> Methods;
    while (std::shared_ptr<HTNMethod> Method = ParseMethod(CurrentPosition))
    {
        Method->SetScope(Domain);
        Methods.emplace_back(Method);
    }

    Domain->SetMethods(Methods);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Domain;
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

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);

    const std::shared_ptr<HTNConstants> Constants = std::make_shared<HTNConstants>(std::move(Name));

    std::vector<std::shared_ptr<const HTNConstant>> ConstantsContainer;
    while (std::shared_ptr<HTNConstant> Constant = ParseConstant(CurrentPosition))
    {
        Constant->SetScope(Constants);
        ConstantsContainer.emplace_back(Constant);
    }

    Constants->SetConstants(ConstantsContainer);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Constants;
}

std::shared_ptr<HTNConstant> HTNParser::ParseConstant(unsigned int& inPosition)
{
    // '(' <identifier> <argument>* ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstant>(std::move(Name), Argument);
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

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<HTNAxiom> Axiom = std::make_shared<HTNAxiom>(std::move(Name), Arguments);

    const std::shared_ptr<HTNConditionBase> Condition = ParseCondition(CurrentPosition, Axiom);
    if (Condition)
    {
        Condition->SetScope(Axiom);
        Axiom->SetCondition(Condition);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Axiom;
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

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_METHOD);

    const std::shared_ptr<HTNMethod> Method = std::make_shared<HTNMethod>(std::move(Name), Arguments, IsTopLevelMethod);

    std::vector<std::shared_ptr<const HTNBranch>> Branches;
    while (std::shared_ptr<HTNBranch> Branch = ParseBranch(CurrentPosition, Method))
    {
        Branch->SetScope(Method);
        Branches.emplace_back(Branch);
    }

    Method->SetBranches(Branches);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Method;
}

std::shared_ptr<HTNBranch> HTNParser::ParseBranch(unsigned int& inPosition, const std::shared_ptr<HTNMethod>& inScope)
{
    // '(' <identifier> '(' <condition>* ')' '(' <task>* ')' ')'

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    const std::shared_ptr<HTNBranch> Branch = std::make_shared<HTNBranch>(std::move(Name));

    const std::shared_ptr<HTNConditionBase> PreCondition = ParseCondition(CurrentPosition, inScope);
    if (PreCondition)
    {
        PreCondition->SetScope(inScope);
        Branch->SetPreCondition(PreCondition);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTask>> Tasks;
    while (std::shared_ptr<HTNTask> Task = ParseTask(CurrentPosition))
    {
        Task->SetScope(inScope);
        Tasks.emplace_back(Task);
    }

    Branch->SetTasks(Tasks);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Branch;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseCondition(unsigned int& inPosition, const std::shared_ptr<HTNNodeBase>& inScope)
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
        const std::shared_ptr<HTNConditionAnd> ConditionAnd = std::make_shared<HTNConditionAnd>();
        Condition                                           = ConditionAnd;

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition, inScope))
        {
            SubCondition->SetScope(inScope);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAnd->SetSubConditions(SubConditions);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::OR))
    {
        const std::shared_ptr<HTNConditionOr> ConditionOr = std::make_shared<HTNConditionOr>();
        Condition                                         = ConditionOr;

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition, inScope))
        {
            SubCondition->SetScope(inScope);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionOr->SetSubConditions(SubConditions);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::ALT))
    {
        const std::shared_ptr<HTNConditionAlt> ConditionAlt = std::make_shared<HTNConditionAlt>();
        Condition                                           = ConditionAlt;

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition, inScope))
        {
            SubCondition->SetScope(inScope);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAlt->SetSubConditions(SubConditions);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Condition;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseSubCondition(unsigned int& inPosition, const std::shared_ptr<HTNNodeBase>& inScope)
{
    // <condition> | ('(' (<identifier> <argument>*)* ')')

    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNConditionBase> Condition = ParseCondition(CurrentPosition, inScope);
    if (!Condition)
    {
        if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
        {
            return nullptr;
        }

        if (ParseToken(CurrentPosition, HTNTokenType::NOT))
        {
            std::shared_ptr<HTNConditionNot> ConditionNot = std::make_shared<HTNConditionNot>();
            Condition                                     = ConditionNot;

            const std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition, inScope);
            if (!SubCondition)
            {
                return nullptr;
            }

            SubCondition->SetScope(inScope);
            ConditionNot->SetSubCondition(SubCondition);
        }
        else
        {
            const bool IsAxiom = ParseToken(CurrentPosition, HTNTokenType::HASH);

            if (std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition))
            {
                std::vector<std::shared_ptr<const HTNValue>> Arguments;
                while (std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
                {
                    Arguments.emplace_back(Argument);
                }

                if (IsAxiom)
                {
                    Condition = std::make_shared<HTNAxiomCondition>(std::move(Name), Arguments);
                }
                else
                {
                    Condition = std::make_shared<HTNCondition>(std::move(Name), Arguments);
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

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNTask>(Type, std::move(Name), Arguments);
}

std::unique_ptr<HTNValue> HTNParser::ParseArgument(unsigned int& inPosition)
{
    // ('?' <identifier>) | <number> | <string>

    unsigned int CurrentPosition = inPosition;

    std::unique_ptr<HTNValue> Argument;

    if (ParseToken(CurrentPosition, HTNTokenType::QUESTION_MARK))
    {
        Argument = ParseIdentifier(CurrentPosition);
        if (!Argument)
        {
            return nullptr;
        }
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::AT))
    {
        // TODO salvarez Store that this is a constant somewhere
        // TODO salvarez Store flags in HTNValue and HTNCondition classes or create new subclasses (e.g. HTNAxiomCondition)
        Argument = ParseIdentifier(CurrentPosition);
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

std::unique_ptr<HTNValue> HTNParser::ParseIdentifier(unsigned int& inPosition)
{
    // IDENTIFIER

    unsigned int CurrentPosition = inPosition;

    const HTNToken* Identifier = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER);
    if (!Identifier)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(Identifier->GetValue(), true);
}

std::unique_ptr<HTNValue> HTNParser::ParseNumber(unsigned int& inPosition)
{
    // NUMBER

    unsigned int CurrentPosition = inPosition;

    const HTNToken* Number = ParseToken(CurrentPosition, HTNTokenType::NUMBER);
    if (!Number)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(Number->GetValue(), false);
}

std::unique_ptr<HTNValue> HTNParser::ParseString(unsigned int& inPosition)
{
    // STRING

    unsigned int CurrentPosition = inPosition;

    const HTNToken* String = ParseToken(CurrentPosition, HTNTokenType::STRING);
    if (!String)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(String->GetValue(), false);
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
