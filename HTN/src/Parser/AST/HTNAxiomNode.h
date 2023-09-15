#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNValueNode;

/**
 * Node representing an axiom
 */
class HTNAxiomNode final : public HTNNodeBase
{
public:
    explicit HTNAxiomNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                          const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes,
                          const std::shared_ptr<const HTNConditionNodeBase>&      inConditionNode);
    ~HTNAxiomNode();

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&              GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNode>>& GetArgumentNodes() const;
    const std::shared_ptr<const HTNConditionNodeBase>&      GetConditionNode() const;

private:
    // Node representing the ID of the axiom
    // - Unique within its domain
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the axiom
    std::vector<std::shared_ptr<const HTNValueNode>> mArgumentNodes;

    // Node representing the condition of the axiom
    std::shared_ptr<const HTNConditionNodeBase> mConditionNode;
};

inline const std::shared_ptr<const HTNValueNode>& HTNAxiomNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNode>>& HTNAxiomNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNAxiomNode::GetConditionNode() const
{
    return mConditionNode;
}
