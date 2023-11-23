// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNIdentifierExpressionNode;
class HTNMethodNode;

/**
 * Node representing a domain
 */
class HTNDomainNode final : public HTNNodeBase
{
public:
    explicit HTNDomainNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&   inIDNode,
                           const std::vector<std::shared_ptr<const HTNConstantsNode>>& inConstantNodes,
                           const std::vector<std::shared_ptr<const HTNAxiomNode>>&     inAxiomNodes,
                           const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, const bool inIsTopLevel);

    HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    std::string GetID() const final;

    std::shared_ptr<const HTNConstantNode> FindConstantNodeByID(const std::string& inConstantNodeID) const;
    std::shared_ptr<const HTNAxiomNode>    FindAxiomNodeByID(const std::string& inAxiomNodeID) const;
    std::shared_ptr<const HTNMethodNode>   FindMethodNodeByID(const std::string& inMethodNodeID) const;

    const std::shared_ptr<const HTNIdentifierExpressionNode>&   GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& GetConstantNodes() const;
    const std::vector<std::shared_ptr<const HTNAxiomNode>>&     GetAxiomNodes() const;
    const std::vector<std::shared_ptr<const HTNMethodNode>>&    GetMethodNodes() const;
    bool                                                        IsTopLevel() const;

private:
    // Node representing the ID of the domain
    // - Globally unique // TODO salvarez Unique within its project
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the constants of the domain
    std::vector<std::shared_ptr<const HTNConstantsNode>> mConstantNodes;

    // Nodes representing the axioms of the domain
    std::vector<std::shared_ptr<const HTNAxiomNode>> mAxiomNodes;

    // Nodes representing the methods of the domain
    std::vector<std::shared_ptr<const HTNMethodNode>> mMethodNodes;

    // Whether the domain serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNDomainNode::GetIDNode() const
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
