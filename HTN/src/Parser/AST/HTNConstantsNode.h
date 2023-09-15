#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConstantNode;
class HTNValueNode;

/**
 * Node representing a group of constants
 */
class HTNConstantsNode final : public HTNNodeBase
{
public:
    explicit HTNConstantsNode(const std::shared_ptr<const HTNValueNode>&                 inIDNode,
                              const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes);
    ~HTNConstantsNode();

    void        Accept(const HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&                 GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNConstantNode>>& GetConstantNodes() const;

private:
    // Node representing the ID of the group of constants
    // - Unique within its domain
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing a group of constants
    std::vector<std::shared_ptr<const HTNConstantNode>> mConstantNodes;
};

inline const std::shared_ptr<const HTNValueNode>& HTNConstantsNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNConstantNode>>& HTNConstantsNode::GetConstantNodes() const
{
    return mConstantNodes;
}
