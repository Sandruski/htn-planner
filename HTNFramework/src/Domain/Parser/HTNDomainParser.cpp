// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

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
#include "Domain/Parser/HTNDomainParserContext.h"
#include "Core/HTNToken.h"
#include "Core/HTNTokenType.h"
#include "Parser/HTNParserHelpers.h"
#include "Domain/HTNDomainHelpers.h"

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

bool HTNDomainParser::Parse(HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseDomain");

    std::shared_ptr<const HTNDomainNode>& DomainNode = ioDomainParserContext.GetDomainNodeMutable();
    if (!ParseDomainNode(DomainNode, ioDomainParserContext))
    {
#ifdef HTN_DEBUG
        HTNParserHelpers::PrintLastErrorMessage(ioDomainParserContext);
#endif
        return false;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, ioDomainParserContext))
    {
#ifdef HTN_DEBUG
        HTNParserHelpers::PrintLastErrorMessage(ioDomainParserContext);
#endif
        return false;
    }

    return true;
}

bool HTNDomainParser::ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseDomainNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_DOMAIN, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const bool IsTopLevelDomain = ParseToken(HTNTokenType::HTN_TOP_LEVEL_DOMAIN, ioDomainParserContext);

    std::vector<std::shared_ptr<const HTNConstantsNode>> ConstantNodes;
    std::shared_ptr<const HTNConstantsNode>              ConstantNodesContainer;
    while (ParseConstantsNode(ConstantNodesContainer, ioDomainParserContext))
    {
        ConstantNodes.emplace_back(ConstantNodesContainer);
    }

    std::vector<std::shared_ptr<const HTNAxiomNode>> AxiomNodes;
    std::shared_ptr<const HTNAxiomNode>              AxiomNode;
    while (ParseAxiomNode(AxiomNode, ioDomainParserContext))
    {
        AxiomNodes.emplace_back(AxiomNode);
    }

    std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
    std::shared_ptr<const HTNMethodNode>              MethodNode;
    while (ParseMethodNode(MethodNode, ioDomainParserContext))
    {
        MethodNodes.emplace_back(MethodNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outDomainNode = std::make_shared<HTNDomainNode>(IDNode, ConstantNodes, AxiomNodes, MethodNodes, IsTopLevelDomain);

    return true;
}

bool HTNDomainParser::ParseConstantsNode(std::shared_ptr<const HTNConstantsNode>& outConstantsNode,
                                         HTNDomainParserContext&                  ioDomainParserContext) const
{
    OPTICK_EVENT("ParseConstantsNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_CONSTANTS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        IDNode = std::make_shared<HTNIdentifierExpressionNode>(HTNDomainHelpers::kAnonymousNamespaceID);
    }

    std::vector<std::shared_ptr<const HTNConstantNode>> ConstantNodes;
    std::shared_ptr<const HTNConstantNode>              ConstantNode;
    while (ParseConstantNode(ConstantNode, ioDomainParserContext))
    {
        ConstantNodes.emplace_back(ConstantNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outConstantsNode = std::make_shared<HTNConstantsNode>(IDNode, ConstantNodes);

    return true;
}

bool HTNDomainParser::ParseConstantNode(std::shared_ptr<const HTNConstantNode>& outConstantNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseConstantNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNLiteralExpressionNode> ValueNode;
    if (!ParseLiteralExpressionNode(ValueNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outConstantNode = std::make_shared<HTNConstantNode>(IDNode, ValueNode);

    return true;
}

bool HTNDomainParser::ParseAxiomNode(std::shared_ptr<const HTNAxiomNode>& outAxiomNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseAxiomNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_AXIOM, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ParameterNode, ioDomainParserContext))
    {
        ParameterNodes.emplace_back(ParameterNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    ParseConditionNode(ConditionNode, ioDomainParserContext);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outAxiomNode = std::make_shared<HTNAxiomNode>(IDNode, ParameterNodes, ConditionNode);

    return true;
}

bool HTNDomainParser::ParseMethodNode(std::shared_ptr<const HTNMethodNode>& outMethodNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseMethodNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::COLON, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::HTN_METHOD, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ParameterNode, ioDomainParserContext))
    {
        ParameterNodes.emplace_back(ParameterNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const bool IsTopLevelMethod = ParseToken(HTNTokenType::HTN_TOP_LEVEL_METHOD, ioDomainParserContext);

    std::vector<std::shared_ptr<const HTNBranchNode>> BranchNodes;
    std::shared_ptr<const HTNBranchNode>              BranchNode;
    while (ParseBranchNode(BranchNode, ioDomainParserContext))
    {
        BranchNodes.emplace_back(BranchNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outMethodNode = std::make_shared<HTNMethodNode>(IDNode, ParameterNodes, BranchNodes, IsTopLevelMethod);

    return true;
}

bool HTNDomainParser::ParseBranchNode(std::shared_ptr<const HTNBranchNode>& outBranchNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseBranchNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> PreConditionNode;
    ParseConditionNode(PreConditionNode, ioDomainParserContext);

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNTaskNodeBase>> TaskNodes;
    std::shared_ptr<const HTNTaskNodeBase>              TaskNode;
    while (ParseTaskNode(TaskNode, ioDomainParserContext))
    {
        TaskNodes.emplace_back(TaskNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outBranchNode = std::make_shared<HTNBranchNode>(IDNode, PreConditionNode, TaskNodes);

    return true;
}

bool HTNDomainParser::ParseConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode,
                                         HTNDomainParserContext&                      ioDomainParserContext) const
{
    OPTICK_EVENT("ParseConditionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;

    if (ParseToken(HTNTokenType::AND, ioDomainParserContext))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, ioDomainParserContext))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }

        ConditionNode = std::make_shared<HTNAndConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::OR, ioDomainParserContext))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, ioDomainParserContext))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }

        ConditionNode = std::make_shared<HTNOrConditionNode>(SubConditionNodes);
    }
    else if (ParseToken(HTNTokenType::ALT, ioDomainParserContext))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(SubConditionNode, ioDomainParserContext))
        {
            SubConditionNodes.emplace_back(SubConditionNode);
        }

        if (SubConditionNodes.empty())
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }

        ConditionNode = std::make_shared<HTNAltConditionNode>(SubConditionNodes);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outConditionNode = ConditionNode;

    return true;
}

bool HTNDomainParser::ParseSubConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode,
                                            HTNDomainParserContext&                      ioDomainParserContext) const
{
    OPTICK_EVENT("ParseSubConditionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    if (!ParseConditionNode(ConditionNode, ioDomainParserContext))
    {
        if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }

        if (ParseToken(HTNTokenType::NOT, ioDomainParserContext))
        {
            std::shared_ptr<const HTNConditionNodeBase> SubConditionNode;
            if (!ParseSubConditionNode(SubConditionNode, ioDomainParserContext))
            {
                ioDomainParserContext.SetPosition(StartPosition);
                return false;
            }

            ConditionNode = std::make_shared<HTNNotConditionNode>(SubConditionNode);
        }
        else
        {
            const bool IsAxiomConditionNode = ParseToken(HTNTokenType::HASH, ioDomainParserContext);

            std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
            if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
            {
                ioDomainParserContext.SetPosition(StartPosition);
                return false;
            }

            std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
            std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
            while (ParseArgumentNode(ArgumentNode, ioDomainParserContext))
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

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }
    }

    outConditionNode = ConditionNode;

    return true;
}

bool HTNDomainParser::ParseTaskNode(std::shared_ptr<const HTNTaskNodeBase>& outTaskNode, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseTaskNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const bool IsPrimitiveTaskNode = ParseToken(HTNTokenType::EXCLAMATION_MARK, ioDomainParserContext);

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(IDNode, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
    std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
    while (ParseArgumentNode(ArgumentNode, ioDomainParserContext))
    {
        ArgumentNodes.emplace_back(ArgumentNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (IsPrimitiveTaskNode)
    {
        outTaskNode = std::make_shared<HTNPrimitiveTaskNode>(IDNode, ArgumentNodes);
    }
    else
    {
        static constexpr bool IsTopLevel = false;
        outTaskNode               = std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes, IsTopLevel);
    }

    return true;
}

bool HTNDomainParser::ParseArgumentNode(std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode,
                                        HTNDomainParserContext&                            ioDomainParserContext) const
{
    OPTICK_EVENT("ParseArgumentNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    std::shared_ptr<const HTNValueExpressionNodeBase> ArgumentNode;

    std::shared_ptr<const HTNVariableExpressionNode> VariableExpressionNode;
    std::shared_ptr<const HTNConstantExpressionNode> ConstantExpressionNode;
    std::shared_ptr<const HTNLiteralExpressionNode>  LiteralExpressionNode;
    if (ParseVariableExpressionNode(VariableExpressionNode, ioDomainParserContext))
    {
        ArgumentNode = VariableExpressionNode;
    }
    else if (ParseConstantExpressionNode(ConstantExpressionNode, ioDomainParserContext))
    {
        ArgumentNode = ConstantExpressionNode;
    }
    else if (ParseLiteralExpressionNode(LiteralExpressionNode, ioDomainParserContext))
    {
        ArgumentNode = LiteralExpressionNode;
    }
    else
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outArgumentNode = ArgumentNode;

    return true;
}

bool HTNDomainParser::ParseVariableExpressionNode(std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode,
                                                  HTNDomainParserContext&                           ioDomainParserContext) const
{
    OPTICK_EVENT("ParseVariableExpressionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::QUESTION_MARK, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, ioDomainParserContext);
    if (!IdentifierToken)
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outVariableExpressionNode = std::make_shared<HTNVariableExpressionNode>(IdentifierToken->GetValue());

    return true;
}

bool HTNDomainParser::ParseConstantExpressionNode(std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode,
                                                  HTNDomainParserContext&                           ioDomainParserContext) const
{
    OPTICK_EVENT("ParseConstantExpressionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::AT, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, ioDomainParserContext);
    if (!IdentifierToken)
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outConstantExpressionNode = std::make_shared<HTNConstantExpressionNode>(IdentifierToken->GetValue());

    return true;
}

bool HTNDomainParser::ParseIdentifierExpressionNode(std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode,
                                                    HTNDomainParserContext&                             ioDomainParserContext) const
{
    OPTICK_EVENT("ParseIdentifierExpressionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, ioDomainParserContext);
    if (!IdentifierToken)
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outIdentifierExpressionNode = std::make_shared<HTNIdentifierExpressionNode>(IdentifierToken->GetValue());

    return true;
}

bool HTNDomainParser::ParseLiteralExpressionNode(std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode,
                                                 HTNDomainParserContext&                          ioDomainParserContext) const
{
    OPTICK_EVENT("ParseLiteralExpressionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    HTNAtom Literal;
    if (!ParseLiteral(Literal, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outLiteralExpressionNode = std::make_shared<HTNLiteralExpressionNode>(Literal);

    return true;
}

bool HTNDomainParser::ParseLiteral(HTNAtom& outLiteral, HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseLiteral");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    HTNAtom Literal;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        HTNAtom LiteralElement;
        while (ParseLiteral(LiteralElement, ioDomainParserContext))
        {
            Literal.AddListElement(LiteralElement);
        }

        if (Literal.IsListEmpty())
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }

        if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
        {
            ioDomainParserContext.SetPosition(StartPosition);
            return false;
        }
    }
    else if (const HTNToken* TrueToken = ParseToken(HTNTokenType::TRUE, ioDomainParserContext))
    {
        Literal = TrueToken->GetValue();
    }
    else if (const HTNToken* FalseToken = ParseToken(HTNTokenType::FALSE, ioDomainParserContext))
    {
        Literal = FalseToken->GetValue();
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, ioDomainParserContext))
    {
        Literal = NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, ioDomainParserContext))
    {
        Literal = StringToken->GetValue();
    }
    else
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outLiteral = Literal;

    return true;
}
