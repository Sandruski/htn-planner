#include "Domain/Parser/HTNDomainParser.h"

#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNToken.h"
#include "Parser/HTNParserHelpers.h"

/*
Backus Naur Form (BNF):
<domain node> ::= '(' ':' 'domain' <identifier node> 'top_level_domain'? <axiom node>* <method node>* ')'
<constants node> ::= '(' ':' 'constants' <identifier node>? <constant node>* ')'
<constant node> ::= '(' <identifier node> <value node>* ')'
<axiom node> ::= '(' ':' 'axiom' '(' <identifier node> <parameter node>* ')' <condition node>? ')'
<method node> ::= '(' ':' 'method' '(' <identifier node> <parameter node>* 'top_level_method'? ')' <branch node>* ')'
<branch node> ::= '(' <identifier node> '(' <condition node>? ')' '(' <task node>* ')' ')'
<condition node> ::= '(' (('and' | 'or' | 'alt') <sub-condition node>)* ')'
<sub-condition node> ::= <condition node> | ('(' ('not' <sub-condition node>)* ')') | ('(' ('#'? <identifier node> <argument node>*)* ')')
<task node> ::= '(' '!'? <identifier node> <argument node>* ')'
<identifier node> ::= 'identifier'
<parameter node> ::= <variable node>
<argument node> ::= <variable node> | <constant expression node> | <literal node>
<value node> ::= <literal node>
<variable node> ::= '?' <identifier node>
<constant expression node> ::= '@' <identifier node>
<literal node> ::= <literal>
<literal> ::= ('(' <literal>+ ')') | 'true' | 'false' | 'number' | 'string'
*/

bool HTNDomainParser::Parse(std::shared_ptr<const HTNDomainNode>& outDomainNode)
{
    std::shared_ptr<const HTNDomainNode> DomainNode;
    unsigned int                         CurrentPosition = 0;
    if (!ParseDomainNode(DomainNode, CurrentPosition))
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return false;
    }

    if (!ParseToken(HTNTokenType::END_OF_FILE, CurrentPosition))
    {
        LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
        return false;
    }

    outDomainNode = DomainNode;

    return true;
}

bool HTNDomainParser::ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, unsigned int& ioPosition)
{
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

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
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

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
    {
        static constexpr bool IsIdentifier = true;
        IDNode                             = std::make_shared<HTNValueNode>(HTNParserHelpers::kAnonymousNamespaceName, IsIdentifier);
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
    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNValueNodeBase> ValueNode;
    if (!ParseValueNode(ValueNode, CurrentPosition))
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

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ParameterNodes;
    std::shared_ptr<const HTNValueNodeBase>              ParameterNode;
    while (ParseArgumentNode(ParameterNode, CurrentPosition))
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

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ParameterNodes;
    std::shared_ptr<const HTNValueNodeBase>              ParameterNode;
    while (ParseParameterNode(ParameterNode, CurrentPosition))
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
    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
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

            std::shared_ptr<const HTNValueNode> IDNode;
            if (!ParseIdentifierNode(IDNode, CurrentPosition))
            {
                return false;
            }

            std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
            std::shared_ptr<const HTNValueNodeBase>              ArgumentNode;
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
    unsigned int CurrentPosition = ioPosition;

    if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    const bool IsPrimitiveTaskNode = ParseToken(HTNTokenType::EXCLAMATION_MARK, CurrentPosition);

    std::shared_ptr<const HTNValueNode> IDNode;
    if (!ParseIdentifierNode(IDNode, CurrentPosition))
    {
        return false;
    }

    std::vector<std::shared_ptr<const HTNValueNodeBase>> ArgumentNodes;
    std::shared_ptr<const HTNValueNodeBase>              ArgumentNode;
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
        static constexpr bool IsTopLevel = false;
        outTaskNode                      = std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes, IsTopLevel);
    }

    ioPosition = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseIdentifierNode(std::shared_ptr<const HTNValueNode>& outIdentifierNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
    if (!IdentifierToken)
    {
        return false;
    }

    static constexpr bool IsIdentifier = true;
    outIdentifierNode                  = std::make_shared<HTNValueNode>(IdentifierToken->GetValue(), IsIdentifier);
    ioPosition                         = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseParameterNode(std::shared_ptr<const HTNValueNodeBase>& outParameterNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNValueNodeBase> ParameterNode;

    std::shared_ptr<const HTNVariableValueNode> VariableNode;
    if (ParseVariableNode(VariableNode, CurrentPosition))
    {
        ParameterNode = VariableNode;
    }
    else
    {
        return false;
    }

    outParameterNode = ParameterNode;
    ioPosition       = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseArgumentNode(std::shared_ptr<const HTNValueNodeBase>& outArgumentNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNValueNodeBase> ArgumentNode;

    std::shared_ptr<const HTNVariableValueNode> VariableNode;
    std::shared_ptr<const HTNConstantValueNode> ConstantNode;
    if (ParseVariableNode(VariableNode, CurrentPosition))
    {
        ArgumentNode = VariableNode;
    }
    else if (ParseConstantExpressionNode(ConstantNode, CurrentPosition))
    {
        ArgumentNode = ConstantNode;
    }
    else if (!ParseLiteralNode(ArgumentNode, CurrentPosition))
    {
        return false;
    }

    outArgumentNode = ArgumentNode;
    ioPosition      = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseValueNode(std::shared_ptr<const HTNValueNodeBase>& outValueNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNValueNodeBase> ValueNode;

    std::shared_ptr<const HTNValueNodeBase> LiteralNode;
    if (ParseLiteralNode(LiteralNode, CurrentPosition))
    {
        ValueNode = LiteralNode;
    }
    else
    {
        return false;
    }

    outValueNode = ValueNode;
    ioPosition   = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseVariableNode(std::shared_ptr<const HTNVariableValueNode>& outVariableValueNode, unsigned int& ioPosition)
{
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

    outVariableValueNode = std::make_shared<HTNVariableValueNode>(IdentifierToken->GetValue());
    ioPosition           = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseConstantExpressionNode(std::shared_ptr<const HTNConstantValueNode>& outConstantValueNode, unsigned int& ioPosition)
{
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

    outConstantValueNode = std::make_shared<HTNConstantValueNode>(IdentifierToken->GetValue());
    ioPosition           = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseLiteralNode(std::shared_ptr<const HTNValueNodeBase>& outLiteralNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    HTNAtom Literal;
    if (!ParseLiteral(Literal, CurrentPosition))
    {
        return false;
    }

    static constexpr bool IsIdentifier = false;
    outLiteralNode                     = std::make_shared<HTNValueNode>(Literal, IsIdentifier);
    ioPosition                         = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseLiteral(HTNAtom& outLiteral, unsigned int& inPosition)
{
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
