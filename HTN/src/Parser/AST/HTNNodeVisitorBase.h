#pragma once

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNTaskNode;
class HTNTaskNode;
class HTNValueNode;

// TODO salvarez Revisit this class and implement planning and printing the AST tree through it
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNDomainNode& inDomainNode) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNConstantsNode& inConstantsNode) const     = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNConstantNode& inConstantNode) const       = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNAxiomNode& inAxiomNode) const             = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNMethodNode& inMethodNode) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNBranchNode& inBranchNode) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNConditionNodeBase& inConditionNode) const = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNTaskNode& inTaskNode) const               = 0;
    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Visit(const HTNValueNode& inValueNode) const             = 0;
};
