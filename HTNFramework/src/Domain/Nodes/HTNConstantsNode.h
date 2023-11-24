// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConstantNode;
class HTNIdentifierExpressionNode;

/**
 * Node representing a group of constants
 */
class HTNConstantsNode final : public HTNNodeBase
{
public:
    explicit HTNConstantsNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                              const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    HTN_NODISCARD std::string GetID() const final;

    const std::shared_ptr<const HTNIdentifierExpressionNode>&  GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNConstantNode>>& GetConstantNodes() const;

private:
    // Node representing the ID of the group of constants
    // - Unique within its domain
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing a group of constants
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
