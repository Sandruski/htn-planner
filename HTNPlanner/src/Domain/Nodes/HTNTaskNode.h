#pragma once

#include "Domain/Nodes/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNIdentifierExpressionNode;
class HTNValueExpressionNodeBase;

/**
 * Node representing a task
 */
class HTNTaskNodeBase : public HTNNodeBase
{
public:
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID);

    std::string GetID() const final;

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetArgumentNodes() const;

protected:
    // Node representing the ID of the task
    // - Not unique
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the arguments of the task
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mArgumentNodes;

private:
    uint32 GenerateID() const;

    // ID of the task
    // - Globally unique
    const uint32 mID = 0;
};

/**
 * Node representing a compound task
 */
class HTNCompoundTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const bool inIsTopLevel);
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID,
                                 const bool inIsTopLevel);

    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;

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
    explicit HTNPrimitiveTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);

    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNTaskNodeBase::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNTaskNodeBase::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline uint32 HTNTaskNodeBase::GenerateID() const
{
    static uint32 mIDGenerator = 0;
    return ++mIDGenerator;
}

inline bool HTNCompoundTaskNode::IsTopLevel() const
{
    return mIsTopLevel;
}
