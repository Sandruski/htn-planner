// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

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
    // Main parse member method
    bool Parse(HTNDomainParserContext& ioDomainParserContext) const;

private:
    // Parses a domain node
    bool ParseDomainNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNDomainNode>& outDomainNode) const;

    // Parses a constants node
    bool ParseConstantsNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConstantsNode>& outConstantsNode) const;

    // Parses a constant node
    bool ParseConstantNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConstantNode>& outConstantNode) const;

    // Parses an axiom node
    bool ParseAxiomNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNAxiomNode>& outAxiomNode) const;

    // Parses a method node
    bool ParseMethodNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNMethodNode>& outMethodNode) const;

    // Parses a branch node
    bool ParseBranchNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNBranchNode>& outBranchNode) const;

    // Parses a condition node
    bool ParseConditionNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConditionNodeBase>& outConditionNode) const;

    // Parses a sub-condition node
    bool ParseSubConditionNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNConditionNodeBase>& outSubConditionNode) const;

    // Parses a task node
    bool ParseTaskNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNTaskNodeBase>& outTaskNode) const;

    // Parses an argument node
    bool ParseArgumentNode(HTNDomainParserContext& ioDomainParserContext, std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode) const;

    // Parses a variable expression node
    bool ParseVariableExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                     std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode) const;

    // Parses a constant expression node
    bool ParseConstantExpressionNode(HTNDomainParserContext&                           ioDomainParserContext,
                                     std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode) const;

    // Parses an identifier expression node
    bool ParseIdentifierExpressionNode(HTNDomainParserContext&                             ioDomainParserContext,
                                       std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode) const;

    // Parses a literal expression node
    bool ParseLiteralExpressionNode(HTNDomainParserContext&                          ioDomainParserContext,
                                    std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode) const;

    // Parses a literal
    bool ParseLiteral(HTNDomainParserContext& ioDomainParserContext, HTNAtom& outLiteral) const;
};
