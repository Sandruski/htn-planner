#include "Domain/Parser/HTNDomainParser.h"

#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNToken.h"
#include "Parser/HTNParserHelpers.h"

/*
Backus Naur Form (BNF):
<domain node> ::= '(' ':' 'domain' <identifier expression node> 'top_level_domain'? <axiom node>* <method node>* ')'
<constants node> ::= '(' ':' 'constants' <identifier expression node>? <constant node>* ')'
<constant node> ::= '(' <identifier expression node> <value node>* ')'
<axiom node> ::= '(' ':' 'axiom' '(' <identifier expression node> <parameter node>* ')' <condition node>? ')'
<method node> ::= '(' ':' 'method' '(' <identifier expression node> <parameter node>* 'top_level_method'? ')' <branch node>* ')'
<branch node> ::= '(' <identifier expression node> '(' <condition node>? ')' '(' <task node>* ')' ')'
<condition node> ::= '(' (('and' | 'or' | 'alt') <sub-condition node>)* ')'
<sub-condition node> ::= <condition node> | ('(' ('not' <sub-condition node>)* ')') | ('(' ('#'? <identifier expression node> <argument node>*)* ')')
<task node> ::= '(' '!'? <identifier expression node> <argument node>* ')'
<argument node> ::= <variable expression node> | <constant expression node> | <literal expression node>
<variable expression node> ::= '?' <identifier expression node>
<constant expression node> ::= '@' <identifier expression node>
<identifier expression node> ::= 'identifier'
<literal expression node> ::= <literal>
<literal> ::= ('(' <literal>+ ')') | 'true' | 'false' | 'number' | 'string'
*/

bool HTNDomainParser::Parse(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode)
{
    OPTICK_EVENT("ParseDomain");

    Reset(inTokens);

    unsigned int CurrentPosition = 0;
    if (!ParseDomainNode(outDomainNode, CurrentPosition))
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return false;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, CurrentPosition))
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return false;
    }

    return true;
}

bool HTNDomainParser::ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseDomainNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_DOMAIN, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    const bool IsTopLevelDomain = ParseToken(HTNTokenType::HTN_TOP_LEVEL_DOMAIN, CurrentPosition);

    std::vector<std::shared_ptr<const HTNConstantsNode>> ConstantNodes;
    std::shared_ptr<const HTNConstantsNode>              ConstantNodesContainer;
    while (ParseConstantsNode(ConstantNodesContainer, CurrentPosition))
    {
        ConstantNodes.emplace_back(ConstantNodesContainer);
    }

    std::vector<std::shared_ptr<const HTNAxiomNode>> AxiomNodes;
    std::shared_ptr<const HTNAxiomNode>              AxiomNode;
    while (ParseAxiomNode(AxiomNode, CurrentPosition))
    {
        AxiomNodes.emplace_back(AxiomNode);
    }

    std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
    std::shared_ptr<const HTNMethodNode>              MethodNode;
    while (ParseMethodNode(MethodNode, CurrentPosition))
    {
        MethodNodes.emplace_back(MethodNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outDomainNode = std::make_shared<HTNDomainNode>(IDNode, ConstantNodes, AxiomNodes, MethodNodes, IsTopLevelDomain);
    ioPosition    = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseConstantsNode(std::shared_ptr<const HTNConstantsNode>& outConstantsNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseConstantsNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_CONSTANTS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        IDNode = std::make_shared<HTNIdentifierExpressionNode>(HTNParserHelpers::kAnonymousNamespaceName);
    }

    std::vector<std::shared_ptr<const HTNConstantNode>> ConstantNodes;
    std::shared_ptr<const HTNConstantNode>              ConstantNode;
    while (ParseConstantNode(ConstantNode, CurrentPosition))
    {
        ConstantNodes.emplace_back(ConstantNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outConstantsNode = std::make_shared<HTNConstantsNode>(IDNode, ConstantNodes);
    ioPosition       = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseConstantNode(std::shared_ptr<const HTNConstantNode>& outConstantNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseConstantNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNLiteralExpressionNode> ValueNode;
    if (!ParseLiteralExpressionNode(ValueNode, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outConstantNode = std::make_shared<HTNConstantNode>(IDNode, ValueNode);
    ioPosition      = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseAxiomNode(std::shared_ptr<const HTNAxiomNode>& outAxiomNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseAxiomNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_AXIOM, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ParameterNode, CurrentPosition))
    {
        ParameterNodes.emplace_back(ParameterNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    ParseConditionNode(ConditionNode, CurrentPosition);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outAxiomNode = std::make_shared<HTNAxiomNode>(IDNode, ParameterNodes, ConditionNode);
    ioPosition   = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseMethodNode(std::shared_ptr<const HTNMethodNode>& outMethodNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseMethodNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_METHOD, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ParameterNode, CurrentPosition))
    {
        ParameterNodes.emplace_back(ParameterNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    const bool IsTopLevelMethod = ParseToken(HTNTokenType::HTN_TOP_LEVEL_METHOD, CurrentPosition);

    std::vector<std::shared_ptr<const HTNBranchNode>> BranchNodes;
    std::shared_ptr<const HTNBranchNode>              BranchNode;
    while (ParseBranchNode(BranchNode, CurrentPosition))
    {
        BranchNodes.emplace_back(BranchNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outMethodNode = std::make_shared<HTNMethodNode>(IDNode, ParameterNodes, BranchNodes, IsTopLevelMethod);
    ioPosition    = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseBranchNode(std::shared_ptr<const HTNBranchNode>& outBranchNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseBranchNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> PreConditionNode;
    ParseConditionNode(PreConditionNode, CurrentPosition);

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNTaskNodeBase>> TaskNodes;
    std::shared_ptr<const HTNTaskNodeBase>              TaskNode;
    while (ParseTaskNode(TaskNode, CurrentPosition))
    {
        TaskNodes.emplace_back(TaskNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outBranchNode = std::make_shared<HTNBranchNode>(IDNode, PreConditionNode, TaskNodes);
    ioPosition    = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseConditionNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;

    if (ParseToken(HTNTokenType::AND, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            return false;
        }

        ConditionNode = std::make_shared<HTNAndConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::OR, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            return false;
        }

        ConditionNode = std::make_shared<HTNOrConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::ALT, CurrentPosition))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, CurrentPosition))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            return false;
        }

        ConditionNode = std::make_shared<HTNAltConditionNode>(SubConditionNodes);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outConditionNode = ConditionNode;
    ioPosition       = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseSubConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseSubConditionNode");

    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    if (!ParseConditionNode(ConditionNode, CurrentPosition))
    {
        if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
        {
            return false;
        }

        if (ParseToken(HTNTokenType::NOT, CurrentPosition))
        {
            std::shared_ptr<const HTNConditionNodeBase> SubConditionNode;
            if (!ParseSubConditionNode(SubConditionNode, CurrentPosition))
            {
                return false;
            }

            ConditionNode = std::make_shared<HTNNotConditionNode>(SubConditionNode);
        }
        else
        {
            const bool IsAxiomConditionNode = ParseToken(HTNTokenType::HASH, CurrentPosition);

            std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
            if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
            {
                return false;
            }

            std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
            std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
            while (ParseArgumentNode(ArgumentNode, CurrentPosition))
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
            return false;
        }
    }

    outConditionNode = ConditionNode;
    ioPosition       = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseTaskNode(std::shared_ptr<const HTNTaskNodeBase>& outTaskNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseTaskNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    const bool IsPrimitiveTaskNode = ParseToken(HTNTokenType::EXCLAMATION_MARK, CurrentPosition);

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
    std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
    while (ParseArgumentNode(ArgumentNode, CurrentPosition))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    if (IsPrimitiveTaskNode)
    {
        outTaskNode = std::make_shared<HTNPrimitiveTaskNode>(IDNode, ArgumentNodes);
    }
    else
    {
        constexpr bool IsTopLevel = false;
        outTaskNode               = std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes, IsTopLevel);
    }

    ioPosition = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseArgumentNode(std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseArgumentNode");

    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNValueExpressionNodeBase> ArgumentNode;

    std::shared_ptr<const HTNVariableExpressionNode> VariableExpressionNode;
    std::shared_ptr<const HTNConstantExpressionNode> ConstantExpressionNode;
    std::shared_ptr<const HTNLiteralExpressionNode>  LiteralExpressionNode;
    if (ParseVariableExpressionNode(VariableExpressionNode, CurrentPosition))
    {
        ArgumentNode = VariableExpressionNode;
    }
    else if (ParseConstantExpressionNode(ConstantExpressionNode, CurrentPosition))
    {
        ArgumentNode = ConstantExpressionNode;
    }
    else if (ParseLiteralExpressionNode(LiteralExpressionNode, CurrentPosition))
    {
        ArgumentNode = LiteralExpressionNode;
    }
    else
    {
        return false;
    }

    outArgumentNode = ArgumentNode;
    ioPosition      = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseVariableExpressionNode(std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode,
                                                  unsigned int&                                     ioPosition)
{
    OPTICK_EVENT("ParseVariableExpressionNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::QUESTION_MARK, CurrentPosition))
    {
        return false;
    }

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return false;
    }

    outVariableExpressionNode = std::make_shared<HTNVariableExpressionNode>(IdentifierToken->GetValue());
    ioPosition                = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseConstantExpressionNode(std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode,
                                                  unsigned int&                                     ioPosition)
{
    OPTICK_EVENT("ParseConstantExpressionNode");

    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::AT, CurrentPosition))
    {
        return false;
    }

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return false;
    }

    outConstantExpressionNode = std::make_shared<HTNConstantExpressionNode>(IdentifierToken->GetValue());
    ioPosition                = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseIdentifierExpressionNode(std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode,
                                                    unsigned int&                                       ioPosition)
{
    OPTICK_EVENT("ParseIdentifierExpressionNode");

    unsigned int CurrentPosition = ioPosition;

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return false;
    }

    outIdentifierExpressionNode = std::make_shared<HTNIdentifierExpressionNode>(IdentifierToken->GetValue());
    ioPosition                  = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseLiteralExpressionNode(std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode, unsigned int& ioPosition)
{
    OPTICK_EVENT("ParseLiteralExpressionNode");

    unsigned int CurrentPosition = ioPosition;

    HTNAtom Literal;
    if (!ParseLiteral(Literal, CurrentPosition))
    {
        return false;
    }

    outLiteralExpressionNode = std::make_shared<HTNLiteralExpressionNode>(Literal);
    ioPosition               = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseLiteral(HTNAtom& outLiteral, unsigned int& inPosition)
{
    OPTICK_EVENT("ParseLiteral");

    unsigned int CurrentPosition = inPosition;

    HTNAtom Literal;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        HTNAtom LiteralElement;
        while (ParseLiteral(LiteralElement, CurrentPosition))
        {
            Literal.AddListElement(LiteralElement);
        }

        if (Literal.IsListEmpty())
        {
            return false;
        }

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
        {
            return false;
        }
    }
    else if (const HTNToken* TrueToken = ParseToken(HTNTokenType::TRUE, CurrentPosition))
    {
        Literal = TrueToken->GetValue();
    }
    else if (const HTNToken* FalseToken = ParseToken(HTNTokenType::FALSE, CurrentPosition))
    {
        Literal = FalseToken->GetValue();
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, CurrentPosition))
    {
        Literal = NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, CurrentPosition))
    {
        Literal = StringToken->GetValue();
    }
    else
    {
        return false;
    }

    outLiteral = Literal;
    inPosition = CurrentPosition;

    return true;
}
