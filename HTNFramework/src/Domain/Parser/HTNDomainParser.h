// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNParserBase.h"

#include <memory>

class HTNAtom;
class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantExpressionNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNDomainParserContext;
class HTNIdentifierExpressionNode;
class HTNLiteralExpressionNode;
class HTNMethodNode;
class HTNTaskNodeBase;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

/**
 * Recursive descent parser
 * Builds a domain from a series of tokens
 */
class HTNDomainParser final : public HTNParserBase
{
public:
    bool Parse(HTNDomainParserContext& ioDomainParserContext) const;

private:
    bool ParseDomainNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNDomainNode>& outDomainNode) const;
    bool ParseConstantsNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConstantsNode>& outConstantsNode) const;
    bool ParseConstantNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConstantNode>& outConstantNode) const;
    bool ParseAxiomNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNAxiomNode>& outAxiomNode) const;
    bool ParseMethodNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNMethodNode>& outMethodNode) const;
    bool ParseBranchNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNBranchNode>& outBranchNode) const;
    bool ParseConditionNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConditionNodeBase>& outConditionNode) const;
    bool ParseSubConditionNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConditionNodeBase>& outSubConditionNode) const;
    bool ParseTaskNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNTaskNodeBase>& outTaskNode) const;
    bool ParseArgumentNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode) const;
    bool ParseVariableExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                     std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode) const;
    bool ParseConstantExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                     std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode) const;
    bool ParseIdentifierExpressionNode(HTNDomainParserContext&                             ioDomainParserContext,
                                       std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode) const;
    bool ParseLiteralExpressionNode(HTNDomainParserContext&                          ioDomainParserContext,
                                    std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode) const;

    bool ParseLiteral(HTNDomainParserContext& ioDomainParserContext, HTNAtom& outLiteral) const;
};
