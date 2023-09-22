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
<sub-condition> ::= <condition> | ('(' ('not' <sub-condition>)* ')') | ('(' ('#'? <identifier> <argument>*)* ')')
<task> ::= '(' '!'? <identifier> <argument>* ')'
<identifier> ::= 'identifier'
<argument> ::= ('?' <identifier>) | ('@' <identifier>) | <identifier> | 'number' | 'string'
*/

std::shared_ptr<const HTNDomainNode> HTNParser::Parse()
{
    unsigned int                               CurrentPosition = 0;
    const std::shared_ptr<const HTNDomainNode> DomainNode      = ParseDomainNode(CurrentPosition);
    if (!DomainNode)
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return nullptr;
    }

    if (!ParseToken(CurrentPosition, HTNTokenType::END_OF_FILE))
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return nullptr;
    }

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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);

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

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNConstantNode>(IDNode, ArgumentNode);
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

    const std::shared_ptr<const HTNValueNode> IDNode = ParseIdentifierNode(CurrentPosition);
    if (!IDNode)
    {
        return nullptr;
    }

    const std::shared_ptr<const HTNConditionNodeBase> PreConditionNode = ParseConditionNode(CurrentPosition);

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    std::vector<std::shared_ptr<const HTNTaskNodeBase>> TaskNodes;
    while (const std::shared_ptr<const HTNTaskNodeBase> TaskNode = ParseTaskNode(CurrentPosition))
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

        ConditionNode = std::make_shared<HTNAndConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::OR))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNOrConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::ALT))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        while (const std::shared_ptr<const HTNConditionNodeBase> SubConditionNode = ParseSubConditionNode(CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        ConditionNode = std::make_shared<HTNAltConditionNode>(SubConditionNodes);
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
            ConditionNode                                                      = std::make_shared<HTNNotConditionNode>(SubConditionNode);
        }
        else
        {
            const bool IsAxiomConditionNode = ParseToken(CurrentPosition, HTNTokenType::HASH);

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

        if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
        {
            return nullptr;
        }
    }

    inPosition = CurrentPosition;

    return ConditionNode;
}

std::shared_ptr<HTNTaskNodeBase> HTNParser::ParseTaskNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    if (!ParseToken(CurrentPosition, HTNTokenType::LEFT_PARENTHESIS))
    {
        return nullptr;
    }

    const bool IsPrimitiveTaskNode = ParseToken(CurrentPosition, HTNTokenType::EXCLAMATION_MARK);

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

    if (!ParseToken(CurrentPosition, HTNTokenType::RIGHT_PARENTHESIS))
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

std::shared_ptr<HTNValueNode> HTNParser::ParseIdentifierNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    const HTNToken* IdentifierToken = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER);
    if (!IdentifierToken)
    {
        return nullptr;
    }

    inPosition = CurrentPosition;

    return std::make_shared<HTNValueNode>(IdentifierToken->GetValue());
}

std::shared_ptr<HTNValueNodeBase> HTNParser::ParseArgumentNode(unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    std::shared_ptr<HTNValueNodeBase> ArgumentNode;

    if (ParseToken(CurrentPosition, HTNTokenType::QUESTION_MARK))
    {
        if (const HTNToken* IdentifierToken = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER))
        {
            ArgumentNode = std::make_shared<HTNVariableValueNode>(IdentifierToken->GetValue());
        }
    }
    else if (ParseToken(CurrentPosition, HTNTokenType::AT))
    {
        if (const HTNToken* IdentifierToken = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER))
        {
            ArgumentNode = std::make_shared<HTNConstantValueNode>(IdentifierToken->GetValue());
        }
    }
    else if (const HTNToken* IdentifierToken = ParseToken(CurrentPosition, HTNTokenType::IDENTIFIER))
    {
        ArgumentNode = std::make_shared<HTNValueNode>(IdentifierToken->GetValue());
    }
    else if (const HTNToken* NumberToken = ParseToken(CurrentPosition, HTNTokenType::NUMBER))
    {
        ArgumentNode = std::make_shared<HTNValueNode>(NumberToken->GetValue());
    }
    else if (const HTNToken* StringToken = ParseToken(CurrentPosition, HTNTokenType::STRING))
    {
        ArgumentNode = std::make_shared<HTNValueNode>(StringToken->GetValue());
    }

    inPosition = CurrentPosition;

    return ArgumentNode;
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
