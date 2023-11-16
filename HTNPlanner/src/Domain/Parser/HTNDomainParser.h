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
 * Syntactic analysis
 * Recursive descent parser
 * Builds a domain from a series of tokens
 * Reports the first syntax error of the tokens
 */
class HTNDomainParser final : public HTNParserBase
{
public:
    bool Parse(HTNDomainParserContext& ioDomainParserContext) const;

private:
    bool ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseConstantsNode(std::shared_ptr<const HTNConstantsNode>& outConstantsNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseConstantNode(std::shared_ptr<const HTNConstantNode>& outConstantNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseAxiomNode(std::shared_ptr<const HTNAxiomNode>& outAxiomNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseMethodNode(std::shared_ptr<const HTNMethodNode>& outMethodNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseBranchNode(std::shared_ptr<const HTNBranchNode>& outBranchNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseSubConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outSubConditionNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseTaskNode(std::shared_ptr<const HTNTaskNodeBase>& outTaskNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseArgumentNode(std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode, HTNDomainParserContext& ioDomainParserContext) const;
    bool ParseVariableExpressionNode(std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode,
                                     HTNDomainParserContext&                           ioDomainParserContext) const;
    bool ParseConstantExpressionNode(std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode,
                                     HTNDomainParserContext&                           ioDomainParserContext) const;
    bool ParseIdentifierExpressionNode(std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode,
                                       HTNDomainParserContext&                             ioDomainParserContext) const;
    bool ParseLiteralExpressionNode(std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode,
                                    HTNDomainParserContext&                          ioDomainParserContext) const;

    bool ParseLiteral(HTNAtom& outLiteral, HTNDomainParserContext& ioDomainParserContext) const;
};
