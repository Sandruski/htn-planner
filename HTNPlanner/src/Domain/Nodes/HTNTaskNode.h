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
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                             const unsigned int inID);
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

private:
    unsigned int GenerateID() const;

    // ID of the task
    // - Globally unique
    const unsigned int mID = 0;
};

/**
 * Node representing a compound task
 */
class HTNCompoundTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const bool inIsTopLevel);
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const unsigned int inID, const bool inIsTopLevel);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

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

inline unsigned int HTNTaskNodeBase::GenerateID() const
{
    static unsigned int mIDGenerator = 0;
    return ++mIDGenerator;
}

inline bool HTNCompoundTaskNode::IsTopLevel() const
{
    return mIsTopLevel;
}
