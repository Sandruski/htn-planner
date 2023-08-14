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
    return ParseDomain();
}

std::shared_ptr<HTNDomain> HTNParser::ParseDomain()
{
    // LEFT_PARENTHESIS COLON DOMAIN <identifier> <method>* TOP_LEVEL_DOMAIN* RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_DOMAIN))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier();
    if (!Name)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(HTNTokenType::HTN_TOP_LEVEL_DOMAIN);

    const std::shared_ptr<HTNDomain> Domain = std::make_shared<HTNDomain>(std::move(Name), IsTopLevelDomain);

    std::vector<std::shared_ptr<const HTNAxiom>> Axioms;
    while (std::shared_ptr<HTNAxiom> Axiom = ParseAxiom())
    {
        Axiom->SetParent(Domain);
        Axioms.emplace_back(Axiom);
    }

    Domain->SetAxioms(Axioms);

    std::vector<std::shared_ptr<const HTNMethod>> Methods;
    while (std::shared_ptr<HTNMethod> Method = ParseMethod())
    {
        Method->SetParent(Domain);
        Methods.emplace_back(Method);
    }

    Domain->SetMethods(Methods);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Domain;
}

std::shared_ptr<HTNAxiom> HTNParser::ParseAxiom()
{
    // LEFT_PARENTHESIS COLON AXIOM LEFT_PARENTHESIS <identifier> <argument>* RIGHT_PARENTHESIS <condition>* RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_AXIOM))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier();
    if (!Name)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument())
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    const std::shared_ptr<HTNAxiom> Axiom = std::make_shared<HTNAxiom>(std::move(Name));

    const std::shared_ptr<HTNConditionBase> Condition = ParseCondition();

    // Only set the parent to the root condition
    Condition->SetParent(Axiom);

    Axiom->SetCondition(Condition);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Axiom;
}

std::shared_ptr<HTNMethod> HTNParser::ParseMethod()
{
    // LEFT_PARENTHESIS COLON METHOD LEFT_PARENTHESIS <identifier> <argument>* TOP_LEVEL_METHOD* RIGHT_PARENTHESIS <branch>* RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_METHOD))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier();
    if (!Name)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument())
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(HTNTokenType::HTN_TOP_LEVEL_METHOD);

    const std::shared_ptr<HTNMethod> Method = std::make_shared<HTNMethod>(std::move(Name), Arguments, IsTopLevelMethod);

    std::vector<std::shared_ptr<const HTNBranch>> Branches;
    while (std::shared_ptr<HTNBranch> Branch = ParseBranch())
    {
        Branch->SetParent(Method);
        Branches.emplace_back(Branch);
    }

    Method->SetBranches(Branches);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Method;
}

std::shared_ptr<HTNBranch> HTNParser::ParseBranch()
{
    // LEFT_PARENTHESIS <identifier> LEFT_PARENTHESIS <condition>* RIGHT_PARENTHESIS LEFT_PARENTHESIS <task>* RIGHT_PARENTHESIS RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::unique_ptr<const HTNValue> Name = ParseIdentifier();
    if (!Name)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    const std::shared_ptr<HTNBranch> Branch = std::make_shared<HTNBranch>(std::move(Name));

    const std::shared_ptr<HTNConditionBase> PreCondition = ParseCondition();

    // Only set the parent to the root pre-condition
    PreCondition->SetParent(Branch);

    Branch->SetPreCondition(PreCondition);

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTask>> Tasks;
    while (std::shared_ptr<HTNTask> Task = ParseTask())
    {
        Task->SetParent(Branch);
        Tasks.emplace_back(Task);
    }

    Branch->SetTasks(Tasks);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Branch;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseCondition()
{
    // LEFT_PARENTHESIS ((AND | OR | ALT) <sub-condition>)* RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::shared_ptr<HTNConditionBase> Condition;

    if (ParseToken(HTNTokenType::AND))
    {
        const std::shared_ptr<HTNConditionAnd> ConditionAnd = std::make_shared<HTNConditionAnd>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition())
        {
            SubCondition->SetParent(ConditionAnd);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAnd->SetSubConditions(SubConditions);

        Condition = ConditionAnd;
    }
    else if (ParseToken(HTNTokenType::OR))
    {
        const std::shared_ptr<HTNConditionOr> ConditionOr = std::make_shared<HTNConditionOr>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition())
        {
            SubCondition->SetParent(ConditionOr);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionOr->SetSubConditions(SubConditions);

        Condition = ConditionOr;
    }
    else if (ParseToken(HTNTokenType::ALT))
    {
        const std::shared_ptr<HTNConditionAlt> ConditionAlt = std::make_shared<HTNConditionAlt>();

        std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
        while (std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition())
        {
            SubCondition->SetParent(ConditionAlt);
            SubConditions.emplace_back(SubCondition);
        }

        ConditionAlt->SetSubConditions(SubConditions);

        Condition = ConditionAlt;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Condition;
}

std::shared_ptr<HTNConditionBase> HTNParser::ParseSubCondition()
{
    // (<condition> | LEFT_PARENTHESIS (<identifier> <argument>*)* RIGHT_PARENTHESIS)

    const unsigned int InitialPosition = mPosition;

    std::shared_ptr<HTNConditionBase> Condition = ParseCondition();
    if (Condition)
    {
        return Condition;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    if (ParseToken(HTNTokenType::NOT))
    {
        std::shared_ptr<HTNConditionNot> ConditionNot = std::make_shared<HTNConditionNot>();

        const std::shared_ptr<HTNConditionBase> SubCondition = ParseSubCondition();
        if (!SubCondition)
        {
            SetPosition(InitialPosition);
            return nullptr;
        }

        SubCondition->SetParent(ConditionNot);

        ConditionNot->SetSubCondition(SubCondition);

        Condition = ConditionNot;
    }
    else
    {
        const bool IsAxiom = ParseToken(HTNTokenType::HASH);

        if (std::unique_ptr<const HTNValue> Key = ParseIdentifier())
        {
            std::vector<std::shared_ptr<const HTNValue>> Arguments;
            while (std::shared_ptr<const HTNValue> Argument = ParseArgument())
            {
                Arguments.emplace_back(Argument);
            }

            Condition = std::make_shared<HTNCondition>(std::move(Key), Arguments, IsAxiom);
        }
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return Condition;
}

std::shared_ptr<HTNTask> HTNParser::ParseTask()
{
    // LEFT_PARENTHESIS EXCLAMATION_MARK* <identifier> <argument>* RIGHT_PARENTHESIS

    const unsigned int InitialPosition = mPosition;

    // TODO salvarez Change
    int Index = mPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    const HTNTaskType Type = ParseToken(HTNTokenType::EXCLAMATION_MARK) ? HTNTaskType::PRIMITIVE : HTNTaskType::COMPOUND;

    std::unique_ptr<const HTNValue> Name = ParseIdentifier();
    if (!Name)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValue>> Arguments;
    while (std::shared_ptr<const HTNValue> Argument = ParseArgument())
    {
        Arguments.emplace_back(Argument);
    }

    if (!ParseToken(++Index, HTNTokenType::RIGHT_PARENTHESIS))
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    AdvancePosition(Index);

    return std::make_shared<HTNTask>(Type, std::move(Name), Arguments);
}

std::unique_ptr<HTNValue> HTNParser::ParseArgument()
{
    // (QUESTION_MARK <identifier>) | <number> | <string>

    const unsigned int InitialPosition = mPosition;

    if (ParseToken(HTNTokenType::QUESTION_MARK))
    {
        if (std::unique_ptr<HTNValue> Identifier = ParseIdentifier())
        {
            return Identifier;
        }
    }
    else if (std::unique_ptr<HTNValue> Number = ParseNumber())
    {
        return Number;
    }
    else if (std::unique_ptr<HTNValue> String = ParseString())
    {
        return String;
    }

    SetPosition(InitialPosition);

    return nullptr;
}

std::unique_ptr<HTNValue> HTNParser::ParseIdentifier()
{
    // IDENTIFIER

    const unsigned int InitialPosition = mPosition;

    const HTNToken* Identifier = ParseToken(HTNTokenType::IDENTIFIER);
    if (!Identifier)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return std::make_unique<HTNValue>(HTNAtom(Identifier->GetValue()));
}

std::unique_ptr<HTNValue> HTNParser::ParseNumber()
{
    // NUMBER

    const unsigned int InitialPosition = mPosition;

    const HTNToken* Number = ParseToken(HTNTokenType::NUMBER);
    if (!Number)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return std::make_unique<HTNValue>(Number->GetValue());
}

std::unique_ptr<HTNValue> HTNParser::ParseString()
{
    // STRING

    const unsigned int InitialPosition = mPosition;

    const HTNToken* String = ParseToken(HTNTokenType::STRING);
    if (!String)
    {
        SetPosition(InitialPosition);
        return nullptr;
    }

    return std::make_unique<HTNValue>(String->GetValue());
}

const HTNToken* HTNParser::ParseToken(const HTNTokenType inTokenType)
{
    const HTNToken* Token = GetToken();
    if (!Token)
    {
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
        return nullptr;
    }

    AdvancePosition();

    return Token;
}
