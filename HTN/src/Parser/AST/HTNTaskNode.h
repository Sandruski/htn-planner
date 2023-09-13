#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNValueNode;

enum class HTNTaskType
{
    PRIMITIVE,
    COMPOUND,

    INVALID
};

/**
 * Node representing a task
 */
class HTNTaskNode : public HTNNodeBase
{
public:
    explicit HTNTaskNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes,
                         const HTNTaskType inType);
    ~HTNTaskNode();

    std::vector<std::shared_ptr<const HTNTaskNode>> Accept(const HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string                                     GetID() const final;
    std::string                                     ToString() const final;

    const std::shared_ptr<const HTNValueNode>&              GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNode>>& GetArgumentNodes() const;
    HTNTaskType                                             GetType() const;

private:
    // Node representing the ID of the task
    // - Not unique
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the task
    std::vector<std::shared_ptr<const HTNValueNode>> mArgumentNodes;

    // Type of the task
    // - Primitive or compound
    HTNTaskType mType = HTNTaskType::INVALID;
};

inline const std::shared_ptr<const HTNValueNode>& HTNTaskNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNode>>& HTNTaskNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline HTNTaskType HTNTaskNode::GetType() const
{
    return mType;
}
