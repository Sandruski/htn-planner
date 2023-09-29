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
#include "HTNLog.h"
#include "HTNToken.h"

/*
Backus Naur Form (BNF):
<domain node> ::= '(' ':' 'domain' <identifier node> 'top_level_domain'? <axiom node>* <method node>* ')'
<constants node> ::= '(' ':' 'constants' <identifier node>? <constant node>* ')'
<constant node> ::= '(' <identifier node> <argument node>* ')'
<axiom node> ::= '(' ':' 'axiom' '(' <identifier node> <argument node>* ')' <condition node>? ')'
<method node> ::= '(' ':' 'method' '(' <identifier node> <argument node>* 'top_level_method'? ')' <branch node>* ')'
<branch node> ::= '(' <identifier node> '(' <condition node>? ')' '(' <task node>* ')' ')'
<condition node> ::= '(' (('and' | 'or' | 'alt') <sub-condition node>)* ')'
<sub-condition node> ::= <condition node> | ('(' ('not' <sub-condition node>)* ')') | ('(' ('#'? <identifier node> <argument node>*)* ')')
<task node> ::= '(' '!'? <identifier node> <argument node>* ')'
<identifier node> ::= 'identifier'
<argument node> ::= ('?' <identifier node>) | ('@' <identifier node>) | <argument>
<argument> ::= ('(' <argument>+ ')') | 'true' | 'false' | 'number' | 'string'
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
    ParseIdentifierNode(IDNode, CurrentPosition);

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

    std::shared_ptr<const HTNValueNodeBase> ArgumentNode;
    if (!ParseArgumentNode(ArgumentNode, CurrentPosition))
    {
        return false;
    }

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outConstantNode = std::make_shared<HTNConstantNode>(IDNode, ArgumentNode);
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

    std::shared_ptr<const HTNConditionNodeBase> ConditionNode;
    ParseConditionNode(ConditionNode, CurrentPosition);

    if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS, CurrentPosition))
    {
        return false;
    }

    outAxiomNode = std::make_shared<HTNAxiomNode>(IDNode, ArgumentNodes, ConditionNode);
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

    outMethodNode = std::make_shared<HTNMethodNode>(IDNode, ArgumentNodes, BranchNodes, IsTopLevelMethod);
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
        outTaskNode = std::make_shared<HTNCompoundTaskNode>(IDNode, ArgumentNodes);
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

bool HTNDomainParser::ParseArgumentNode(std::shared_ptr<const HTNValueNodeBase>& outArgumentNode, unsigned int& ioPosition)
{
    unsigned int CurrentPosition = ioPosition;

    std::shared_ptr<const HTNValueNodeBase> ArgumentNode;

    if (ParseToken(HTNTokenType::QUESTION_MARK, CurrentPosition))
    {
        const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
        if (!IdentifierToken)
        {
            return false;
        }

        ArgumentNode = std::make_shared<HTNVariableValueNode>(IdentifierToken->GetValue());
    }
    else if (ParseToken(HTNTokenType::AT, CurrentPosition))
    {
        const HTNToken* IdentifierToken = ParseToken(HTNTokenType::IDENTIFIER, CurrentPosition);
        if (!IdentifierToken)
        {
            return false;
        }

        ArgumentNode = std::make_shared<HTNConstantValueNode>(IdentifierToken->GetValue());
    }
    else
    {
        HTNAtom Argument;
        if (ParseArgument(Argument, CurrentPosition))
        {
            static constexpr bool IsIdentifier = false;
            ArgumentNode                       = std::make_shared<HTNValueNode>(Argument, IsIdentifier);
        }
        else
        {
            return false;
        }
    }

    outArgumentNode = ArgumentNode;
    ioPosition      = CurrentPosition;

    return true;
}

bool HTNDomainParser::ParseArgument(HTNAtom& outArgument, unsigned int& inPosition)
{
    unsigned int CurrentPosition = inPosition;

    HTNAtom Argument;

    if (ParseToken(HTNTokenType::LEFT_PARENTHESIS, CurrentPosition))
    {
        HTNAtom ArgumentElement;
        while (ParseArgument(ArgumentElement, CurrentPosition))
        {
            Argument.AddListElement(ArgumentElement);
        }

        if (Argument.IsListEmpty())
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
        Argument = TrueToken->GetValue();
    }
    else if (const HTNToken* FalseToken = ParseToken(HTNTokenType::FALSE, CurrentPosition))
    {
        Argument = FalseToken->GetValue();
    }
    else if (const HTNToken* NumberToken = ParseToken(HTNTokenType::NUMBER, CurrentPosition))
    {
        Argument = NumberToken->GetValue();
    }
    else if (const HTNToken* StringToken = ParseToken(HTNTokenType::STRING, CurrentPosition))
    {
        Argument = StringToken->GetValue();
    }
    else
    {
        return false;
    }

    outArgument = Argument;
    inPosition  = CurrentPosition;

    return true;
}
