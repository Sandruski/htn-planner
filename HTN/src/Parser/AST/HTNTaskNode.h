#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNValueNode;
class HTNValueNodeBase;

/**
 * Node representing a task
 */
class HTNTaskNodeBase : public HTNNodeBase
{
public:
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes);
    ~HTNTaskNodeBase();

    std::string GetID() const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&              GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& GetArgumentNodes() const;

private:
    // Node representing the ID of the task
    // - Not unique
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the task
    std::vector<std::shared_ptr<const HTNValueNodeBase>> mArgumentNodes;
};

/**
 * Node representing a compound task
 */
class HTNCompoundTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes);

    HTNAtom Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
};

/**
 * Node representing a primitive task
 */
class HTNPrimitiveTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNPrimitiveTaskNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes);

    HTNAtom Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
};

inline const std::shared_ptr<const HTNValueNode>& HTNTaskNodeBase::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNodeBase>>& HTNTaskNodeBase::GetArgumentNodes() const
{
    return mArgumentNodes;
}
