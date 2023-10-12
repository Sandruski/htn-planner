#pragma once

#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionNodeBase;
class HTNValueNode;
class HTNValueNodeBase;

/**
 * Node representing an axiom
 */
class HTNAxiomNode final : public HTNNodeBase
{
public:
    explicit HTNAxiomNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                          const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                          const std::shared_ptr<const HTNConditionNodeBase>&          inConditionNode);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    std::string GetID() const final;

    const std::shared_ptr<const HTNValueNode>&                  GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& GetArgumentNodes() const;
    const std::shared_ptr<const HTNConditionNodeBase>&          GetConditionNode() const;

private:
    // Node representing the ID of the axiom
    // - Unique within its domain
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the axiom
    std::vector<std::shared_ptr<const HTNValueNodeBase>> mArgumentNodes;

    // Node representing the condition of the axiom
    std::shared_ptr<const HTNConditionNodeBase> mConditionNode;
};

inline HTNAxiomNode::HTNAxiomNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                                  const std::shared_ptr<const HTNConditionNodeBase>&          inConditionNode)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mConditionNode(inConditionNode)
{
}

inline const std::shared_ptr<const HTNValueNode>& HTNAxiomNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNodeBase>>& HTNAxiomNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNAxiomNode::GetConditionNode() const
{
    return mConditionNode;
}
