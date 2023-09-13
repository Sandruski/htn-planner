#pragma once

#include "Parser/AST/HTNValueNode.h"
#include "Parser/HTNToken.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNTaskNode;
class HTNValueNode;

// Recursive descent parser
// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNParser
{
public:
    explicit HTNParser(const std::vector<HTNToken>& inTokens);

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
    std::shared_ptr<HTNTaskNode>          ParseTaskNode(unsigned int& inPosition);
    std::shared_ptr<HTNValueNode>         ParseArgumentNode(unsigned int& inPosition);
    std::shared_ptr<HTNValueNode>         ParseIdentifierNode(unsigned int& inPosition, const HTNValueType inValueType);
    std::shared_ptr<HTNValueNode>         ParseNumberNode(unsigned int& inPosition);
    std::shared_ptr<HTNValueNode>         ParseStringNode(unsigned int& inPosition);
    const HTNToken*                       ParseToken(unsigned int& inPosition, const HTNTokenType inTokenType);

    const HTNToken* GetToken(const unsigned int inPosition) const;

    std::vector<HTNToken> mTokens;

#if HTN_DEBUG
private:
    std::string mLastErrorMessage;
    int         mLastErrorRow    = -1;
    int         mLastErrorColumn = -1;
#endif
};

inline HTNParser::HTNParser(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

inline const HTNToken* HTNParser::GetToken(const unsigned int inPosition) const
{
    return (inPosition < mTokens.size() ? &mTokens[inPosition] : nullptr);
}
