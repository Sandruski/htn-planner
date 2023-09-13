#include "Parser/HTNParser.h"

#include "HTNAtom.h"
#include "HTNLog.h"
#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNConstantsNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"

/*
Backus Naur Form (BNF):
<domain> ::= '(' ':' 'domain' <identifier> 'top_level_domain'? <axiom>* <method>* ')'
<constants> ::= '(' ':' 'constants' <identifier>? <constant>* ')'
<constant> ::= '(' <identifier> <argument>* ')'
<axiom> ::= '(' ':' 'axiom' '(' <identifier> <argument>* ')' <condition>? ')'
<method> ::= '(' ':' 'method' '(' <identifier> <argument>* 'top_level_method'? ')' <branch>* ')'
<branch> ::= '(' <identifier> '(' <condition>? ')' '(' <task>* ')' ')'
<condition> ::= '(' (('and' | 'or' | 'alt') <sub-condition>)* ')'
<subcondition> ::= <condition> | ('(' (<identifier> <argument>*)* ')')
<task> ::= '(' '!'? <identifier> <argument>* ')'
<argument> ::= ('?' <identifier>) | <number> | <string>
<identifier> ::= 'identifier'
<number> ::= 'number'
<string> ::= 'string'
*/

std::shared_ptr<const HTNDomainNode> HTNParser::Parse()
{
    unsigned int                               InitialPosition = 0;
    const std::shared_ptr<const HTNDomainNode> DomainNode      = ParseDomainNode(InitialPosition);
    CLOG_HTN_ERROR(!DomainNode, mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
    return DomainNode;
}

std::shared_ptr<HTNDomainNode> HTNParser::ParseDomainNode(unsigned int& inPosition)
{
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_DOMAIN);

    std::vector<std::shared_ptr<const HTNConstantsNode>> ConstantNodes;
    while (const std::shared_ptr<const HTNConstantsNode> ConstantNodesContainer = ParseConstantsNode(CurrentPosition))
    {
        ConstantNodes.emplace_back(ConstantNodesContainer);
    }

    std::vector<std::shared_ptr<const HTNAxiomNode>> AxiomNodes;
    while (const std::shared_ptr<const HTNAxiomNode> AxiomNode = ParseAxiomNode(CurrentPosition))
    {
        AxiomNodes.emplace_back(AxiomNode);
    }

    std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
    while (const std::shared_ptr<const HTNMethodNode> MethodNode = ParseMethodNode(CurrentPosition))
    {
        MethodNodes.emplace_back(MethodNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNDomainNode>(IDNode, ConstantNodes, AxiomNodes, MethodNodes, IsTopLevelDomain);
}

std::shared_ptr<HTNConstantsNode> HTNParser::ParseConstantsNode(unsigned int& inPosition)
{
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);

    std::vector<std::shared_ptr<const HTNConstantNode>> ConstantNodes;
    while (const std::shared_ptr<const HTNConstantNode> ConstantNode = ParseConstantNode(CurrentPosition))
    {
        ConstantNodes.emplace_back(ConstantNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstantsNode>(IDNode, ConstantNodes);
}

std::shared_ptr<HTNConstantNode> HTNParser::ParseConstantNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> ValueNode = ParseArgumentNode(CurrentPosition);
    if (!ValueNode)
    {
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstantNode>(IDNode, ValueNode);
}

std::shared_ptr<HTNAxiomNode> HTNParser::ParseAxiomNode(unsigned int& inPosition)
{
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNode>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNode> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionNodeBase> ConditionNode = ParseConditionNode(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNAxiomNode>(IDNode, ArgumentNodes, ConditionNode);
}

std::shared_ptr<HTNMethodNode> HTNParser::ParseMethodNode(unsigned int& inPosition)
{
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNode>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNode> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(CurrentPosition, HTNTokenType::HTN_TOP_LEVEL_METHOD);

    std::vector<std::shared_ptr<const HTNBranchNode>> BranchNodes;
    while (const std::shared_ptr<const HTNBranchNode> BranchNode = ParseBranchNode(CurrentPosition))
    {
        BranchNodes.emplace_back(BranchNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNMethodNode>(IDNode, ArgumentNodes, BranchNodes, IsTopLevelMethod);
}

std::shared_ptr<HTNBranchNode> HTNParser::ParseBranchNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionNodeBase> PreConditionNode = ParseConditionNode(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTaskNode>> TaskNodes;
    while (const std::shared_ptr<const HTNTaskNode> TaskNode = ParseTaskNode(CurrentPosition))
    {
        TaskNodes.emplace_back(TaskNode);
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

    return std::make_shared<HTNBranchNode>(IDNode, PreConditionNode, TaskNodes);
}

std::shared_ptr<HTNConditionNodeBase> HTNParser::ParseConditionNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::shared_ptr<HTNConditionNodeBase> ConditionNode;

    if (ParseToken(CurrentPosition, HTNTokenType::AND))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNConditionAndNode>(SubConditionNodes);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::OR))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNConditionOrNode>(SubConditionNodes);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::ALT))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNConditionAltNode>(SubConditionNodes);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return ConditionNode;
}

std::shared_ptr<HTNConditionNodeBase> HTNParser::ParseSubConditionNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNConditionNodeBase> ConditionNode = ParseConditionNode(CurrentPosition);
    if (!ConditionNode)
    {
        if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
        {
            return nullptr;
        }

        if (ParseToken(CurrentPosition, HTNTokenType::NOT))
        {
            const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition);
            ConditionNode                                                      = std::make_shared<HTNConditionNotNode>(SubConditionNode);
        }
        else
        {
            const bool IsAxiom = ParseToken(CurrentPosition, HTNTokenType::HASH);

            if (const std::shared_ptr<const HTNValueNode> IDNode =
                    ParseIdentifierNode(CurrentPosition, IsAxiom ? HTNValueType::AXIOM : HTNValueType::SYMBOL))
            {
                std::vector<std::shared_ptr<const HTNValueNode>> ArgumentNodes;
                while (const std::shared_ptr<const HTNValueNode> ArgumentNode = ParseArgumentNode(CurrentPosition))
                {
                    ArgumentNodes.emplace_back(ArgumentNode);
                }

                if (IsAxiom)
                {
                    ConditionNode = std::make_shared<HTNConditionAxiomNode>(IDNode, ArgumentNodes);
                }
                else
                {
                    ConditionNode = std::make_shared<HTNConditionNode>(IDNode, ArgumentNodes);
                }
            }
        }

        if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
        {
            return nullptr;
        }
    }

    inPosition = CurrentPosition;

    return ConditionNode;
}

std::shared_ptr<HTNTaskNode> HTNParser::ParseTaskNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const HTNTaskType Type = ParseToken(CurrentPosition, HTNTokenType::EXCLAMATION_MARK) ? HTNTaskType::PRIMITIVE : HTNTaskType::COMPOUND;

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition, HTNValueType::SYMBOL);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNode>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNode> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNTaskNode>(IDNode, ArgumentNodes, Type);
}

std::shared_ptr<HTNValueNode> HTNParser::ParseArgumentNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNValueNode> ArgumentNode;

    if (ParseToken(CurrentPosition, HTNTokenType::QUESTION_MARK))
    {
        ArgumentNode = ParseIdentifierNode(CurrentPosition, HTNValueType::VARIABLE);
        if (!ArgumentNode)
        {
            return nullptr;
        }
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::AT))
    {
        ArgumentNode = ParseIdentifierNode(CurrentPosition, HTNValueType::CONSTANT);
        if (!ArgumentNode)
        {
            return nullptr;
        }
    }
    else
    {
        ArgumentNode = ParseNumberNode(CurrentPosition);
        if (!ArgumentNode)
        {
            ArgumentNode = ParseStringNode(CurrentPosition);
            if (!ArgumentNode)
            {
                return nullptr;
            }
        }
    }

    inPosition = CurrentPosition;

    return ArgumentNode;
}

// TODO salvarez Revisit this. It shouldn't be allowed to pass any value type
std::shared_ptr<HTNValueNode> HTNParser::ParseIdentifierNode(unsigned int& inPosition, const HTNValueType inValueType)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* IdentifierToken = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER);
    if (!IdentifierToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValueNode>(IdentifierToken->GetValue(), inValueType);
}

std::shared_ptr<HTNValueNode> HTNParser::ParseNumberNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* NumberToken = ParseToken(CurrentPosition, HTNTokenType::NUMBER);
    if (!NumberToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValueNode>(NumberToken->GetValue(), HTNValueType::LITERAL);
}

std::shared_ptr<HTNValueNode> HTNParser::ParseStringNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* StringToken = ParseToken(CurrentPosition, HTNTokenType::STRING);
    if (!StringToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValueNode>(StringToken->GetValue(), HTNValueType::LITERAL);
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
