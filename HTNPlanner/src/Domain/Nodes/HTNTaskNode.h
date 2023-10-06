#pragma once

#include "Domain/Nodes/HTNNodeBase.h"

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
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes);
    ~HTNTaskNodeBase();

    std::string GetID() const final;

    const std::shared_ptr<const HTNValueNode>&                  GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& GetArgumentNodes() const;

protected:
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
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const bool inIsTopLevel);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    const std::string& GetMethodNodeID() const;

    bool IsTopLevel() const;

private:
    // Whether the compound task serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

/**
 * Node representing a primitive task
 */
class HTNPrimitiveTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNPrimitiveTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;
};

inline const std::shared_ptr<const HTNValueNode>& HTNTaskNodeBase::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNodeBase>>& HTNTaskNodeBase::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline bool HTNCompoundTaskNode::IsTopLevel() const
{
    return mIsTopLevel;
}
