#include "Interpreter/HTNParser.h"

#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Log.h"
#include "Runtime/HTNAtom.h"

std::shared_ptr<const HTNDomain> HTNParser::Parse()
{
    unsigned int InitialPosition = 0;
    return ParseDomain(InitialPosition);
}

std::shared_ptr<HTNDomain> HTNParser::ParseDomain(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS COLON DOMAIN <identifier> <method>* TOP_LEVEL_DOMAIN* RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::HTN_DOMAIN))
    {
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(CurrentPosition++, HTNTokenType::HTN_TOP_LEVEL_DOMAIN);

    const std::shared_ptr<HTNDomain> Domain = std::make_shared<HTNDomain>(std::move(Name), IsTopLevelDomain);

    std::vector<std::shared_ptr<const HTNAxiom>> Axioms;
    while (std::shared_ptr<HTNAxiom> Axiom = ParseAxiom(CurrentPosition))
    {
        Axiom->SetParent(Domain);
        Axioms.emplace_back(Axiom);
    }

    Domain->SetAxioms(Axioms);

    std::vector<std::shared_ptr<const HTNMethod>> Methods;
    while (std::shared_ptr<HTNMethod> Method = ParseMethod(CurrentPosition))
    {
        Method->SetParent(Domain);
        Methods.emplace_back(Method);
    }

    Domain->SetMethods(Methods);

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Domain;
}

std::shared_ptr<HTNAxiom> HTNParser::ParseAxiom(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS COLON AXIOM LEFT_PARENTHESIS <identifier> <argument>* RIGHT_PARENTHESIS <condition>* RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::HTN_AXIOM))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
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

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<HTNAxiom> Axiom = std::make_shared<HTNAxiom>(std::move(Name));

    const std::shared_ptr<HTNConditionBase> Condition = ParseCondition(CurrentPosition);
    if (Condition)
    {
        // Only set the parent to the root condition
        Condition->SetParent(Axiom);

        Axiom->SetCondition(Condition);
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Axiom;
}

std::shared_ptr<HTNMethod> HTNParser::ParseMethod(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS COLON METHOD LEFT_PARENTHESIS <identifier> <argument>* TOP_LEVEL_METHOD* RIGHT_PARENTHESIS <branch>* RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::COLON))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::HTN_METHOD))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
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

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(CurrentPosition++, HTNTokenType::HTN_TOP_LEVEL_METHOD);

    const std::shared_ptr<HTNMethod> Method = std::make_shared<HTNMethod>(std::move(Name), Arguments, IsTopLevelMethod);

    std::vector<std::shared_ptr<const HTNBranch>> Branches;
    while (std::shared_ptr<HTNBranch> Branch = ParseBranch(CurrentPosition))
    {
        Branch->SetParent(Method);
        Branches.emplace_back(Branch);
    }

    Method->SetBranches(Branches);

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Method;
}

std::shared_ptr<HTNBranch> HTNParser::ParseBranch(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS <identifier> LEFT_PARENTHESIS <condition>* RIGHT_PARENTHESIS LEFT_PARENTHESIS <task>* RIGHT_PARENTHESIS RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier(CurrentPosition);
    if (!Name)
    {
        return nullptr;
    }

    const std::shared_ptr<HTNBranch> Branch = std::make_shared<HTNBranch>(std::move(Name));

    const std::shared_ptr<HTNConditionBase> PreCondition = ParseCondition(CurrentPosition);
    if (PreCondition)
    {
        // Only set the parent to the root pre-condition
        PreCondition->SetParent(Branch);

        Branch->SetPreCondition(PreCondition);
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTask>> Tasks;
    while (std::shared_ptr<HTNTask> Task = ParseTask(CurrentPosition))
    {
        Task->SetParent(Branch);
        Tasks.emplace_back(Task);
    }

    Branch->SetTasks(Tasks);

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Branch;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseCondition(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS ((AND | OR | ALT) <sub-condition>)* RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::shared_ptr<HTNConditionBase> Condition;

    // TODO salvarez Fix this. if increments, else if does it again. Learn how to handle the ORs
    if (ParseToken(CurrentPosition++, HTNTokenType::AND))
    {
        const std::shared_ptr<HTNConditionAnd> ConditionAnd = std::make_shared<HTNConditionAnd>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubCondition->SetParent(ConditionAnd);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAnd->SetSubConditions(SubConditions);

        Condition = ConditionAnd;
    }
    else if (ParseToken(CurrentPosition++, HTNTokenType::OR))
    {
        const std::shared_ptr<HTNConditionOr> ConditionOr = std::make_shared<HTNConditionOr>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubCondition->SetParent(ConditionOr);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionOr->SetSubConditions(SubConditions);

        Condition = ConditionOr;
    }
    else if (ParseToken(CurrentPosition++, HTNTokenType::ALT))
    {
        const std::shared_ptr<HTNConditionAlt> ConditionAlt = std::make_shared<HTNConditionAlt>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition))
        {
            SubCondition->SetParent(ConditionAlt);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAlt->SetSubConditions(SubConditions);

        Condition = ConditionAlt;
    }

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return Condition;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseSubCondition(unsigned int& inPosition)
{
    // (<condition> | LEFT_PARENTHESIS (<identifier> <argument>*)* RIGHT_PARENTHESIS)

    unsigned int CurrentPosition = inPosition;

    // TODO salvarez Learn how to handle the ORs
    std::shared_ptr<HTNConditionBase> Condition = ParseCondition(CurrentPosition);
    if (!Condition)
    {
        if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
        {
            return nullptr;
        }

        if (ParseToken(CurrentPosition++, HTNTokenType::NOT))
        {
            std::shared_ptr<HTNConditionNot> ConditionNot = std::make_shared<HTNConditionNot>();

            const std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition(CurrentPosition);
            if (!SubCondition)
            {
                return nullptr;
            }

            SubCondition->SetParent(ConditionNot);

            ConditionNot->SetSubCondition(SubCondition);

            Condition = ConditionNot;
        }
        else
        {
            const bool IsAxiom = ParseToken(CurrentPosition++, HTNTokenType::HASH);

            if (std::unique_ptr<const HTNValue> Key = ParseIdentifier(CurrentPosition))
            {
                std::vector<std::shared_ptr<const HTNValue>> Arguments;
                while (std::shared_ptr<const HTNValue> Argument = ParseArgument(CurrentPosition))
                {
                    Arguments.emplace_back(Argument);
                }

                Condition = std::make_shared<HTNCondition>(std::move(Key), Arguments, IsAxiom);
            }
        }

        if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
        {
            return nullptr;
        }
    }

    inPosition = CurrentPosition;

    return Condition;
}

std::shared_ptr<HTNTask> HTNParser::ParseTask(unsigned int& inPosition)
{
    // LEFT_PARENTHESIS EXCLAMATION_MARK* <identifier> <argument>* RIGHT_PARENTHESIS

    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition++, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const HTNTaskType Type = ParseToken(CurrentPosition++, HTNTokenType::EXCLAMATION_MARK) ? HTNTaskType::PRIMITIVE : HTNTaskType::COMPOUND;

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

    if (!ParseToken(CurrentPosition++, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNTask>(Type, std::move(Name), Arguments);
}

std::unique_ptr<HTNValue> HTNParser::ParseArgument(unsigned int& inPosition)
{
    // (QUESTION_MARK <identifier>) | <number> | <string>

    unsigned int CurrentPosition = inPosition;

    std::unique_ptr<HTNValue> Argument;

    // TODO salvarez Learn how to handle the ORs
    if (ParseToken(CurrentPosition++, HTNTokenType::QUESTION_MARK))
    {
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

    const HTNToken* Identifier = ParseToken(CurrentPosition++, HTNTokenType::IDENTIFIER);
    if (!Identifier)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(HTNAtom(Identifier->GetValue()));
}

std::unique_ptr<HTNValue> HTNParser::ParseNumber(unsigned int& inPosition)
{
    // NUMBER

    unsigned int CurrentPosition = inPosition;

    const HTNToken* Number = ParseToken(CurrentPosition++, HTNTokenType::NUMBER);
    if (!Number)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(Number->GetValue());
}

std::unique_ptr<HTNValue> HTNParser::ParseString(unsigned int& inPosition)
{
    // STRING

    unsigned int CurrentPosition = inPosition;

    const HTNToken* String = ParseToken(CurrentPosition++, HTNTokenType::STRING);
    if (!String)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_unique<HTNValue>(String->GetValue());
}

const HTNToken* HTNParser::ParseToken(const unsigned int inPosition, const HTNTokenType inTokenType)
{
    const HTNToken* Token = GetToken(inPosition);
    if (!Token)
    {
        LOG_ERROR("Token at [{}] is out of bounds [{}]", inPosition, mTokens.size());
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
        LOG_HTN_ERROR(Token->GetRow(), Token->GetColumn(), "Token [{}] is of type [{}] instead of [{}]", Token->GetLexeme(),
                      GetTokenTypeString(Token->GetType()), GetTokenTypeString(inTokenType));
        return nullptr;
    }

    return Token;
}
