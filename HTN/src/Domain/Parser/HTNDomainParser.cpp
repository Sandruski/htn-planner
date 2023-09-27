#include "Domain/Parser/HTNDomainParser.h"

#include "Domain/AST/HTNAxiomNode.h"
#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNConstantNode.h"
#include "Domain/AST/HTNConstantsNode.h"
#include "Domain/AST/HTNDomainNode.h"
#include "Domain/AST/HTNMethodNode.h"
#include "Domain/AST/HTNTaskNode.h"
#include "Domain/AST/HTNValueNode.h"
#include "HTNToken.h"

/*
Backus Naur Form (BNF):
<domain> ::= '(' ':' 'domain' <identifier> 'top_level_domain'? <axiom>* <method>* ')'
<constants> ::= '(' ':' 'constants' <identifier>? <constant>* ')'
<constant> ::= '(' <identifier> <argument>* ')'
<axiom> ::= '(' ':' 'axiom' '(' <identifier> <argument>* ')' <condition>? ')'
<method> ::= '(' ':' 'method' '(' <identifier> <argument>* 'top_level_method'? ')' <branch>* ')'
<branch> ::= '(' <identifier> '(' <condition>? ')' '(' <task>* ')' ')'
<condition> ::= '(' (('and' | 'or' | 'alt') <sub-condition>)* ')'
<sub-condition> ::= <condition> | ('(' ('not' <sub-condition>)* ')') | ('(' ('#'? <identifier> <argument>*)* ')')
<task> ::= '(' '!'? <identifier> <argument>* ')'
<identifier> ::= 'identifier'
<argument> ::= ('?' <identifier>) | ('@' <identifier>) | 'number' | 'string'
*/

std::shared_ptr<const HTNDomainNode> HTNDomainParser::Parse()
{
    unsigned int                               CurrentPosition = 0;
    const std::shared_ptr<const HTNDomainNode> DomainNode      = ParseDomainNode(CurrentPosition);
    if (!DomainNode)
    {
#if HTN_DEBUG
        LogLastError();
#endif
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, CurrentPosition))
    {
#if HTN_DEBUG
        LogLastError();
#endif
        return nullptr;
    }

    return DomainNode;
}

std::shared_ptr<HTNDomainNode> HTNDomainParser::ParseDomainNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_DOMAIN, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    const bool IsTopLevelDomain = ParseToken(HTNTokenType::HTN_TOP_LEVEL_DOMAIN, CurrentPosition);

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

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNDomainNode>(IDNode, ConstantNodes, AxiomNodes, MethodNodes, IsTopLevelDomain);
}

std::shared_ptr<HTNConstantsNode> HTNDomainParser::ParseConstantsNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_CONSTANTS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);

    std::vector<std::shared_ptr<const HTNConstantNode>> ConstantNodes;
    while (const std::shared_ptr<const HTNConstantNode> ConstantNode = ParseConstantNode(CurrentPosition))
    {
        ConstantNodes.emplace_back(ConstantNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstantsNode>(IDNode, ConstantNodes);
}

std::shared_ptr<HTNConstantNode> HTNDomainParser::ParseConstantNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNodeBase> ArgumentNode = ParseArgumentNode(CurrentPosition);
    if (!ArgumentNode)
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstantNode>(IDNode, ArgumentNode);
}

std::shared_ptr<HTNAxiomNode> HTNDomainParser::ParseAxiomNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_AXIOM, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNodeBase> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionNodeBase> ConditionNode = ParseConditionNode(CurrentPosition);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNAxiomNode>(IDNode, ArgumentNodes, ConditionNode);
}

std::shared_ptr<HTNMethodNode> HTNDomainParser::ParseMethodNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::HTN_METHOD, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNodeBase> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const bool IsTopLevelMethod = ParseToken(HTNTokenType::HTN_TOP_LEVEL_METHOD, CurrentPosition);

    std::vector<std::shared_ptr<const HTNBranchNode>> BranchNodes;
    while (const std::shared_ptr<const HTNBranchNode> BranchNode = ParseBranchNode(CurrentPosition))
    {
        BranchNodes.emplace_back(BranchNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNMethodNode>(IDNode, ArgumentNodes, BranchNodes, IsTopLevelMethod);
}

std::shared_ptr<HTNBranchNode> HTNDomainParser::ParseBranchNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionNodeBase> PreConditionNode = ParseConditionNode(CurrentPosition);

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTaskNodeBase>> TaskNodes;
    while (const std::shared_ptr<const HTNTaskNodeBase> TaskNode = ParseTaskNode(CurrentPosition))
    {
        TaskNodes.emplace_back(TaskNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNBranchNode>(IDNode, PreConditionNode, TaskNodes);
}

std::shared_ptr<HTNConditionNodeBase> HTNDomainParser::ParseConditionNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    std::shared_ptr<HTNConditionNodeBase> ConditionNode;

    if (ParseToken(HTNTokenType::AND, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNAndConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::OR, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNOrConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::ALT, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNAltConditionNode>(SubConditionNodes);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return ConditionNode;
}

std::shared_ptr<HTNConditionNodeBase> HTNDomainParser::ParseSubConditionNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNConditionNodeBase> ConditionNode = ParseConditionNode(CurrentPosition);
    if (!ConditionNode)
    {
        if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
        {
            return nullptr;
        }

        if (ParseToken(HTNTokenType::NOT, CurrentPosition))
        {
            const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition);
            ConditionNode                                                      = std::make_shared<HTNNotConditionNode>(SubConditionNode);
        }
        else
        {
            const bool IsAxiomConditionNode = ParseToken(HTNTokenType::HASH, CurrentPosition);

            const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
            if (!IDNode)
            {
                return nullptr;
            }

            std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
            while (const std::shared_ptr<const HTNValueNodeBase> ArgumentNode = ParseArgumentNode(CurrentPosition))
            {
                ArgumentNodes.emplace_back(ArgumentNode);
            }

            if (IsAxiomConditionNode)
            {
                ConditionNode = std::make_shared<HTNAxiomConditionNode>(IDNode, ArgumentNodes);
            }
            else
            {
                ConditionNode = std::make_shared<HTNConditionNode>(IDNode, ArgumentNodes);
            }
        }

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
        {
            return nullptr;
        }
    }

    inPosition = CurrentPosition;

    return ConditionNode;
}

std::shared_ptr<HTNTaskNodeBase> HTNDomainParser::ParseTaskNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    const bool IsPrimitiveTaskNode = ParseToken(HTNTokenType::EXCLAMATION_MARK, CurrentPosition);

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
    while (const std::shared_ptr<const HTNValueNodeBase> ArgumentNode = ParseArgumentNode(CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    if (IsPrimitiveTaskNode)
    {
        return std::make_shared<HTNPrimitiveTaskNode>(IDNode, ArgumentNodes);
    }

    return std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes);
}

std::shared_ptr<HTNValueNode> HTNDomainParser::ParseIdentifierNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValueNode>(IdentifierToken->GetValue());
}

std::shared_ptr<HTNValueNodeBase> HTNDomainParser::ParseArgumentNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNValueNodeBase> ArgumentNode;

    if (ParseToken(HTNTokenType::QUESTION_MARK, CurrentPosition))
    {
        if (const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition))
        {
            ArgumentNode = std::make_shared<HTNVariableValueNode>(IdentifierToken->GetValue());
        }
    }
    else if (ParseToken(HTNTokenType::AT, CurrentPosition))
    {
        if (const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition))
        {
            ArgumentNode = std::make_shared<HTNConstantValueNode>(IdentifierToken->GetValue());
        }
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, CurrentPosition))
    {
        ArgumentNode = std::make_shared<HTNValueNode>(NumberToken->GetValue());
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, CurrentPosition))
    {
        ArgumentNode = std::make_shared<HTNValueNode>(StringToken->GetValue());
    }

    inPosition = CurrentPosition;

    return ArgumentNode;
}
