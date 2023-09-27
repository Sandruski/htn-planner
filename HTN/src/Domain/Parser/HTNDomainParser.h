#pragma once

#include "HTNParserBase.h"

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

// Recursive descent parser
// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNDomainParser final : public HTNParserBase
{
public:
    explicit HTNDomainParser(const std::vector<HTNToken>& inDomainTokens);

    std::shared_ptr<const HTNDomainNode> Parse();

private:
    std::shared_ptr<HTNDomainNode>        ParseDomainNode(unsigned int& inPosition);
    std::shared_ptr<HTNConstantsNode>     ParseConstantsNode(unsigned int& inPosition);
    std::shared_ptr<HTNConstantNode>      ParseConstantNode(unsigned int& inPosition);
    std::shared_ptr<HTNAxiomNode>         ParseAxiomNode(unsigned int& inPosition);
    std::shared_ptr<HTNMethodNode>        ParseMethodNode(unsigned int& inPosition);
    std::shared_ptr<HTNBranchNode>        ParseBranchNode(unsigned int& inPosition);
    std::shared_ptr<HTNConditionNodeBase> ParseConditionNode(unsigned int& inPosition);
    std::shared_ptr<HTNConditionNodeBase> ParseSubConditionNode(unsigned int& inPosition);
    // HTNPrimitiveTaskNode or HTNCompoundTaskNode
    std::shared_ptr<HTNTaskNodeBase>      ParseTaskNode(unsigned int& inPosition);
    std::shared_ptr<HTNValueNode>         ParseIdentifierNode(unsigned int& inPosition);
    // HTNValueNode, HTNVariableValueNode, or HTNConstantValueNode
    std::shared_ptr<HTNValueNodeBase>     ParseArgumentNode(unsigned int& inPosition);
};

inline HTNDomainParser::HTNDomainParser(const std::vector<HTNToken>& inDomainTokens) : HTNParserBase(inDomainTokens)
{
}
