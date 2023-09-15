#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNMethodNode;
class HTNValueNode;
class HTNAxiomNode;
class HTNConstantNode;
class HTNConstantsNode;

/**
 * Node representing a domain
 */
class HTNDomainNode final : public HTNNodeBase
{
public:
    explicit HTNDomainNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                           const std::vector<std::shared_ptr<const HTNConstantsNode>>& inConstantNodes,
                           const std::vector<std::shared_ptr<const HTNAxiomNode>>&     inAxiomNodes,
                           const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, const bool inIsTopLevel);
    ~HTNDomainNode();

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    std::shared_ptr<const HTNConstantNode> FindConstantNodeByID(const std::string& inID) const;
    std::shared_ptr<const HTNAxiomNode>    FindAxiomNodeByID(const std::string& inID) const;
    std::shared_ptr<const HTNMethodNode>   FindMethodNodeByID(const std::string& inID) const;

    const std::shared_ptr<const HTNValueNode>&                  GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& GetConstantNodes() const;
    const std::vector<std::shared_ptr<const HTNAxiomNode>>&     GetAxiomNodes() const;
    const std::vector<std::shared_ptr<const HTNMethodNode>>&    GetMethodNodes() const;
    bool                                                        IsTopLevel() const;

private:
    // Node representing the ID of the domain
    // - Globally unique // TODO salvarez Unique within its project
    std::shared_ptr<const HTNValueNode> mIDNode;

    // TODO salvarez mIncludes with HTNIncludeNode
    // TODO salvarez HTNProjectStructure node to store all domains

    // Nodes representing the constants of the domain
    std::vector<std::shared_ptr<const HTNConstantsNode>> mConstantNodes;

    // Nodes representing the axioms of the domain
    std::vector<std::shared_ptr<const HTNAxiomNode>> mAxiomNodes;

    // Nodes representing the methods of the domain
    std::vector<std::shared_ptr<const HTNMethodNode>> mMethodNodes;

    // Whether the domain serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::shared_ptr<const HTNValueNode>& HTNDomainNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNConstantsNode>>& HTNDomainNode::GetConstantNodes() const
{
    return mConstantNodes;
}

inline const std::vector<std::shared_ptr<const HTNAxiomNode>>& HTNDomainNode::GetAxiomNodes() const
{
    return mAxiomNodes;
}

inline const std::vector<std::shared_ptr<const HTNMethodNode>>& HTNDomainNode::GetMethodNodes() const
{
    return mMethodNodes;
}

inline bool HTNDomainNode::IsTopLevel() const
{
    return mIsTopLevel;
}
