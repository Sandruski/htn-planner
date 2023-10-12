#pragma once

#include "Parser/HTNParserBase.h"

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNTaskNodeBase;
class HTNToken;
class HTNValueNode;
class HTNValueNodeBase;

// Syntactic analysis
// Recursive descent parser
// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNDomainParser final : public HTNParserBase<std::shared_ptr<const HTNDomainNode>>
{
public:
    explicit HTNDomainParser(const std::vector<HTNToken>& inDomainTokens);

    bool Parse(std::shared_ptr<const HTNDomainNode>& outDomainNode) final;

private:
    bool ParseDomainNode(std::shared_ptr<const HTNDomainNode>& outDomainNode, unsigned int& ioPosition);
    bool ParseConstantsNode(std::shared_ptr<const HTNConstantsNode>& outConstantsNode, unsigned int& ioPosition);
    bool ParseConstantNode(std::shared_ptr<const HTNConstantNode>& outConstantNode, unsigned int& ioPosition);
    bool ParseAxiomNode(std::shared_ptr<const HTNAxiomNode>& outAxiomNode, unsigned int& ioPosition);
    bool ParseMethodNode(std::shared_ptr<const HTNMethodNode>& outMethodNode, unsigned int& ioPosition);
    bool ParseBranchNode(std::shared_ptr<const HTNBranchNode>& outBranchNode, unsigned int& ioPosition);
    bool ParseConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outConditionNode, unsigned int& ioPosition);
    bool ParseSubConditionNode(std::shared_ptr<const HTNConditionNodeBase>& outSubConditionNode, unsigned int& ioPosition);
    // HTNPrimitiveTaskNode or HTNCompoundTaskNode
    bool ParseTaskNode(std::shared_ptr<const HTNTaskNodeBase>& outTaskNode, unsigned int& ioPosition);
    bool ParseIdentifierNode(std::shared_ptr<const HTNValueNode>& outIdentifierNode, unsigned int& ioPosition);
    // HTNValueNode, HTNVariableValueNode, or HTNConstantValueNode
    bool ParseArgumentNode(std::shared_ptr<const HTNValueNodeBase>& outArgumentNode, unsigned int& ioPosition);
    bool ParseLiteralNode(std::shared_ptr<const HTNValueNodeBase>& outLiteralNode, unsigned int& ioPosition);

    bool ParseLiteral(HTNAtom& outLiteral, unsigned int& inPosition);
};

inline HTNDomainParser::HTNDomainParser(const std::vector<HTNToken>& inDomainTokens) : HTNParserBase(inDomainTokens)
{
}
