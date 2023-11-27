// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNConstantNode;
class HTNIdentifierExpressionNode;

/**
 * Node representing a group of constants of a domain
 */
class HTNConstantsNode final : public HTNNodeBase
{
public:
    explicit HTNConstantsNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                              const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the ID of the node, which is the same as the ID of the group of constants because it is already unique within its domain
    HTN_NODISCARD std::string GetID() const final;

    // Returns the node representing the ID of the group of constants
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the group of constants
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNConstantNode>>& GetConstantNodes() const;

private:
    // Node representing the ID of the group of constants
    // - Unique within its domain
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the group of constants
    std::vector<std::shared_ptr<const HTNConstantNode>> mConstantNodes;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNConstantsNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNConstantNode>>& HTNConstantsNode::GetConstantNodes() const
{
    return mConstantNodes;
}
