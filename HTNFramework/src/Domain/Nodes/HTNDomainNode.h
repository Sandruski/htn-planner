// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

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
                           const std::vector<std::shared_ptr<const HTNConstantsNode>>& inConstantsNodes,
                           const std::vector<std::shared_ptr<const HTNAxiomNode>>&     inAxiomNodes,
                           const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, const bool inIsTopLevel);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the ID of the node, which is the same as the ID of the domain because it is already globally unique
    HTN_NODISCARD std::string GetID() const final;

    // Returns a constant node by a given ID
    HTN_NODISCARD std::shared_ptr<const HTNConstantNode> FindConstantNodeByID(const std::string& inConstantNodeID) const;

    // Returns an axiom node by a given ID
    HTN_NODISCARD std::shared_ptr<const HTNAxiomNode> FindAxiomNodeByID(const std::string& inAxiomNodeID) const;

    // Returns a method node by a given ID
    HTN_NODISCARD std::shared_ptr<const HTNMethodNode> FindMethodNodeByID(const std::string& inMethodNodeID) const;

    // Returns the node representing the ID of the domain
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the constants of the domain
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNConstantsNode>>& GetConstantsNodes() const;

    // Returns the nodes representing the axioms of the domain
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNAxiomNode>>& GetAxiomNodes() const;

    // Returns the nodes representing the methods of the domain
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNMethodNode>>& GetMethodNodes() const;

    // Returns whether the domain serves as an entry point for a decomposition
    HTN_NODISCARD bool IsTopLevel() const;

private:
    // Node representing the ID of the domain
    // - Globally unique
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the constants of the domain
    std::vector<std::shared_ptr<const HTNConstantsNode>> mConstantsNodes;

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

inline const std::vector<std::shared_ptr<const HTNConstantsNode>>& HTNDomainNode::GetConstantsNodes() const
{
    return mConstantsNodes;
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
