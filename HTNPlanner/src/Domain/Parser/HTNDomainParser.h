#pragma once

#include "Parser/HTNParserBase.h"

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantExpressionNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNIdentifierExpressionNode;
class HTNLiteralExpressionNode;
class HTNMethodNode;
class HTNTaskNodeBase;
class HTNToken;
class HTNValueExpressionNodeBase;
class HTNVariableExpressionNode;

// Syntactic analysis
// Recursive descent parser
// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNDomainParser final : public HTNParserBase<std::shared_ptr<const HTNDomainNode>>
{
public:
    bool Parse(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode) final;

private:
    bool ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, unsigned int& ioPosition);
    bool ParseConstantsNode(std::shared_ptr<const HTNConstantsNode>& outConstantsNode, unsigned int& ioPosition);
    bool ParseConstantNode(std::shared_ptr<const HTNConstantNode>& outConstantNode, unsigned int& ioPosition);
    bool ParseAxiomNode(std::shared_ptr<const HTNAxiomNode>& outAxiomNode, unsigned int& ioPosition);
    bool ParseMethodNode(std::shared_ptr<const HTNMethodNode>& outMethodNode, unsigned int& ioPosition);
    bool ParseBranchNode(std::shared_ptr<const HTNBranchNode>& outBranchNode, unsigned int& ioPosition);
    bool ParseConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode, unsigned int& ioPosition);
    bool ParseSubConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outSubConditionNode, unsigned int& ioPosition);
    bool ParseTaskNode(std::shared_ptr<const HTNTaskNodeBase>& outTaskNode, unsigned int& ioPosition);
    bool ParseArgumentNode(std::shared_ptr<const HTNValueExpressionNodeBase>& outArgumentNode, unsigned int& ioPosition);
    bool ParseVariableExpressionNode(std::shared_ptr<const HTNVariableExpressionNode>& outVariableExpressionNode, unsigned int& ioPosition);
    bool ParseConstantExpressionNode(std::shared_ptr<const HTNConstantExpressionNode>& outConstantExpressionNode, unsigned int& ioPosition);
    bool ParseIdentifierExpressionNode(std::shared_ptr<const HTNIdentifierExpressionNode>& outIdentifierExpressionNode, unsigned int& ioPosition);
    bool ParseLiteralExpressionNode(std::shared_ptr<const HTNLiteralExpressionNode>& outLiteralExpressionNode, unsigned int& ioPosition);

    bool ParseLiteral(HTNAtom& outLiteral, unsigned int& inPosition);
};
