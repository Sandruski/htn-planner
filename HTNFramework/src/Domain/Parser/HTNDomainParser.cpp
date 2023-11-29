// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Parser/HTNDomainParser.h"

#include "Domain/HTNDomainHelpers.h"
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
#include "Parser/HTNParserHelpers.h"
#include "Parser/HTNToken.h"
#include "Parser/HTNTokenType.h"

// clang-format off
/**
 * Backus Naur Form (BNF)
 * <domain node> ::= '(' ':' 'domain' <identifier expression node> 'top_level_domain'? <constants node>* <axiom node>* <method node>* ')'
 * <constants node> ::= '(' ':' 'constants' <identifier expression node>? <constant node>* ')'
 * <constant node> ::= '(' <identifier expression node> <literal expression node> ')'
 * <axiom node> ::= '(' ':' 'axiom' '(' <identifier expression node> <variable expression node>* ')' '(' <condition node>? ')' ')'
 * <method node> ::= '(' ':' 'method' '(' <identifier expression node> <variable expression node>* 'top_level_method'? ')' <branch node>* ')'
 * <branch node> ::= '(' <identifier expression node> '(' <condition node>? ')' '(' <task node>* ')' ')'
 * <condition node> ::= ('and' | 'or' | 'alt') <sub-condition node>+
 * <sub-condition node> ::= '(' (<condition node> | ('not' <sub-condition node>) | ('#'? <identifier expression node> <argument node>*)) ')'
 * <task node> ::= '(' '!'? <identifier expression node> <argument node>* ')'
 * <argument node> ::= <variable expression node> | <constant expression node> | <literal expression node>
 * <variable expression node> ::= '?' 'identifier'
 * <constant expression node> ::= '@' 'identifier'
 * <identifier expression node> ::= 'identifier'
 * <literal expression node> ::= <literal>
 * <literal> ::= ('(' <literal>+ ')') | 'true' | 'false' | 'number' | 'string'
 */
// clang-format on

bool HTNDomainParser::Parse(HTNDomainParserContext& ioDomainParserContext) const
{
    OPTICK_EVENT("ParseDomain");

    std::shared_ptr<const HTNDomainNode>& DomainNode = ioDomainParserContext.GetDomainNodeMutable();
    if (!ParseDomainNode(ioDomainParserContext, DomainNode))
    {
#ifdef HTN_ENABLE_LOGGING
        HTNParserHelpers::PrintLastError(ioDomainParserContext);
#endif
        return false;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, ioDomainParserContext))
    {
#ifdef HTN_ENABLE_LOGGING
        HTNParserHelpers::PrintLastError(ioDomainParserContext);
#endif
        return false;
    }

    return true;
}

bool HTNDomainParser::ParseDomainNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNDomainNode>& outDomainNode) const
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
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    const bool IsTopLevelDomain = ParseToken(HTNTokenType::HTN_TOP_LEVEL_DOMAIN, ioDomainParserContext);

    std::vector<std::shared_ptr<const HTNConstantsNode>> ConstantsNodes;
    std::shared_ptr<const HTNConstantsNode>              ConstantsNode;
    while (ParseConstantsNode(ioDomainParserContext, ConstantsNode))
    {
        ConstantsNodes.emplace_back(ConstantsNode);
    }

    std::vector<std::shared_ptr<const HTNAxiomNode>> AxiomNodes;
    std::shared_ptr<const HTNAxiomNode>              AxiomNode;
    while (ParseAxiomNode(ioDomainParserContext, AxiomNode))
    {
        AxiomNodes.emplace_back(AxiomNode);
    }

    std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
    std::shared_ptr<const HTNMethodNode>              MethodNode;
    while (ParseMethodNode(ioDomainParserContext, MethodNode))
    {
        MethodNodes.emplace_back(MethodNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outDomainNode = std::make_shared<HTNDomainNode>(IDNode, ConstantsNodes, AxiomNodes, MethodNodes, IsTopLevelDomain);

    return true;
}

bool HTNDomainParser::ParseConstantsNode(HTNDomainParserContext&                  ioDomainParserContext,
                                         std::shared_ptr<const HTNConstantsNode>& outConstantsNode) const
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
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        IDNode = std::make_shared<HTNIdentifierExpressionNode>(HTNDomainHelpers::kAnonymousNamespaceID);
    }

    std::vector<std::shared_ptr<const HTNConstantNode>> ConstantNodes;
    std::shared_ptr<const HTNConstantNode>              ConstantNode;
    while (ParseConstantNode(ioDomainParserContext, ConstantNode))
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

bool HTNDomainParser::ParseConstantNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConstantNode>& outConstantNode) const
{
    OPTICK_EVENT("ParseConstantNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNLiteralExpressionNode> ValueNode;
    if (!ParseLiteralExpressionNode(ioDomainParserContext, ValueNode))
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

bool HTNDomainParser::ParseAxiomNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNAxiomNode>& outAxiomNode) const
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
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ioDomainParserContext, ParameterNode))
    {
        ParameterNodes.emplace_back(ParameterNode);
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    ParseConditionNode(ioDomainParserContext, ConditionNode);

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

    outAxiomNode = std::make_shared<HTNAxiomNode>(IDNode, ParameterNodes, ConditionNode);

    return true;
}

bool HTNDomainParser::ParseMethodNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNMethodNode>& outMethodNode) const
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
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNVariableExpressionNode>> ParameterNodes;
    std::shared_ptr<const HTNVariableExpressionNode>              ParameterNode;
    while (ParseVariableExpressionNode(ioDomainParserContext, ParameterNode))
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
    while (ParseBranchNode(ioDomainParserContext, BranchNode))
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

bool HTNDomainParser::ParseBranchNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNBranchNode>& outBranchNode) const
{
    OPTICK_EVENT("ParseBranchNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNIdentifierExpressionNode> IDNode;
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> PreConditionNode;
    ParseConditionNode(ioDomainParserContext, PreConditionNode);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNTaskNodeBase>> TaskNodes;
    std::shared_ptr<const HTNTaskNodeBase>              TaskNode;
    while (ParseTaskNode(ioDomainParserContext, TaskNode))
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

bool HTNDomainParser::ParseConditionNode(HTNDomainParserContext&                      ioDomainParserContext,
                                         std::shared_ptr<const HTNConditionNodeBase>& outConditionNode) const
{
    OPTICK_EVENT("ParseConditionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;

    if (ParseToken(HTNTokenType::AND, ioDomainParserContext))
    {
        std::vector<std::shared_ptr<const HTNConditionNodeBase>> SubConditionNodes;
        std::shared_ptr<const HTNConditionNodeBase>              SubConditionNode;
        while (ParseSubConditionNode(ioDomainParserContext, SubConditionNode))
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
        while (ParseSubConditionNode(ioDomainParserContext, SubConditionNode))
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
        while (ParseSubConditionNode(ioDomainParserContext, SubConditionNode))
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

    outConditionNode = ConditionNode;

    return nullptr != outConditionNode;
}

bool HTNDomainParser::ParseSubConditionNode(HTNDomainParserContext&                      ioDomainParserContext,
                                            std::shared_ptr<const HTNConditionNodeBase>& outConditionNode) const
{
    OPTICK_EVENT("ParseSubConditionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    if (!ParseConditionNode(ioDomainParserContext, ConditionNode))
    {
        if (ParseToken(HTNTokenType::NOT, ioDomainParserContext))
        {
            std::shared_ptr<const HTNConditionNodeBase> SubConditionNode;
            if (!ParseSubConditionNode(ioDomainParserContext, SubConditionNode))
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
            if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
            {
                ioDomainParserContext.SetPosition(StartPosition);
                return false;
            }

            std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
            std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
            while (ParseArgumentNode(ioDomainParserContext, ArgumentNode))
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
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, ioDomainParserContext))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outConditionNode = ConditionNode;

    return true;
}

bool HTNDomainParser::ParseTaskNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNTaskNodeBase>& outTaskNode) const
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
    if (!ParseIdentifierExpressionNode(ioDomainParserContext, IDNode))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> ArgumentNodes;
    std::shared_ptr<const HTNValueExpressionNodeBase>              ArgumentNode;
    while (ParseArgumentNode(ioDomainParserContext, ArgumentNode))
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
        outTaskNode                      = std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes, IsTopLevel);
    }

    return true;
}

bool HTNDomainParser::ParseArgumentNode(HTNDomainParserContext&                            ioDomainParserContext,
                                        std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode) const
{
    OPTICK_EVENT("ParseArgumentNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    std::shared_ptr<const HTNValueExpressionNodeBase> ArgumentNode;

    std::shared_ptr<const HTNVariableExpressionNode> VariableExpressionNode;
    std::shared_ptr<const HTNConstantExpressionNode> ConstantExpressionNode;
    std::shared_ptr<const HTNLiteralExpressionNode>  LiteralExpressionNode;
    if (ParseVariableExpressionNode(ioDomainParserContext, VariableExpressionNode))
    {
        ArgumentNode = VariableExpressionNode;
    }
    else if (ParseConstantExpressionNode(ioDomainParserContext, ConstantExpressionNode))
    {
        ArgumentNode = ConstantExpressionNode;
    }
    else if (ParseLiteralExpressionNode(ioDomainParserContext, LiteralExpressionNode))
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

bool HTNDomainParser::ParseVariableExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                                  std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode) const
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

bool HTNDomainParser::ParseConstantExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                                  std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode) const
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

bool HTNDomainParser::ParseIdentifierExpressionNode(HTNDomainParserContext&                             ioDomainParserContext,
                                                    std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode) const
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

bool HTNDomainParser::ParseLiteralExpressionNode(HTNDomainParserContext&                          ioDomainParserContext,
                                                 std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode) const
{
    OPTICK_EVENT("ParseLiteralExpressionNode");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    HTNAtom Literal;
    if (!ParseLiteral(ioDomainParserContext, Literal))
    {
        ioDomainParserContext.SetPosition(StartPosition);
        return false;
    }

    outLiteralExpressionNode = std::make_shared<HTNLiteralExpressionNode>(Literal);

    return true;
}

bool HTNDomainParser::ParseLiteral(HTNDomainParserContext& ioDomainParserContext, HTNAtom& outLiteral) const
{
    OPTICK_EVENT("ParseLiteral");

    const uint32 StartPosition = ioDomainParserContext.GetPosition();

    HTNAtom Literal;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, ioDomainParserContext))
    {
        HTNAtom LiteralElement;
        while (ParseLiteral(ioDomainParserContext, LiteralElement))
        {
            Literal.PushBackElementToList(LiteralElement);
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
