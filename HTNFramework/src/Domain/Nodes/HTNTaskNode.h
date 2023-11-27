// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNIdentifierExpressionNode;
class HTNValueExpressionNodeBase;

/**
 * Node representing a task of a branch
 */
class HTNTaskNodeBase : public HTNNodeBase
{
public:
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);
    explicit HTNTaskNodeBase(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID);

    // Returns the ID of the node
    HTN_NODISCARD std::string GetID() const final;

    // Returns the node representing the ID of the task
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the arguments of the task
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetArgumentNodes() const;

protected:
    // Node representing the ID of the task
    // - Not unique
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the arguments of the task
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mArgumentNodes;

private:
    // Generates a new globally unique ID
    HTN_NODISCARD uint32 GenerateGUID() const;

    // ID of the task
    // - Globally unique
    const uint32 mID = 0;
};

/**
 * Node representing a compound task of a branch
 */
class HTNCompoundTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const bool inIsTopLevel);
    explicit HTNCompoundTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes, const uint32 inID,
                                 const bool inIsTopLevel);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns whether the compound task serves as an entry point for a decomposition
    HTN_NODISCARD bool IsTopLevel() const;

private:
    // Whether the compound task serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

/**
 * Node representing a primitive task of a branch
 */
class HTNPrimitiveTaskNode final : public HTNTaskNodeBase
{
public:
    explicit HTNPrimitiveTaskNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                  const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNTaskNodeBase::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNTaskNodeBase::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline bool HTNCompoundTaskNode::IsTopLevel() const
{
    return mIsTopLevel;
}
