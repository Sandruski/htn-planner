// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <vector>

class HTNIdentifierExpressionNode;
class HTNValueExpressionNodeBase;

/**
 * Base class for a node representing a condition of a branch
 */
class HTNConditionNodeBase : public HTNNodeBase
{
public:
    HTNConditionNodeBase();

    // Returns the ID of the node
    HTN_NODISCARD std::string GetID() const final;

private:
    // Generates a new globally unique ID
    HTN_NODISCARD uint32 GenerateGUID() const;

    // ID of the node
    // - Globally unique
    const uint32 mID = 0;
};

/**
 * Node representing a condition of a branch
 * - Queries the fact in the world state, binding unbound arguments
 */
class HTNConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                              const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the node representing the ID of the condition
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the arguments of the condition
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetArgumentNodes() const;

private:
    // Node representing the ID of the condition
    // - ID of the fact in the world state
    // - Not unique
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the arguments of the condition
    // - Arguments of the fact in the world state
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mArgumentNodes;
};

/**
 * Node representing an axiom condition of a branch
 * - Checks the condition of an axiom
 */
class HTNAxiomConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAxiomConditionNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&             inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inArgumentNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the node representing the ID of the condition
    HTN_NODISCARD const std::shared_ptr<const HTNIdentifierExpressionNode>& GetIDNode() const;

    // Returns the nodes representing the arguments of the condition
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& GetArgumentNodes() const;

private:
    // Node representing the ID of the condition
    // - ID of the axiom in the domain
    // - Not unique
    std::shared_ptr<const HTNIdentifierExpressionNode> mIDNode;

    // Nodes representing the arguments of the condition
    // - Arguments of the axiom in the domain
    std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>> mArgumentNodes;
};

/**
 * Node representing an AND condition of a branch
 * - In order to succeed, all of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of its sub-conditions
 * - If it binds any unbound arguments of a sub-condition, the sub-condition is considered for backtracking
 * - If it succeeds, the arguments of its sub-conditions will be bound
 */
class HTNAndConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the nodes representing the sub-conditions of the condition
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetSubConditionNodes() const;

private:
    // Nodes representing the sub-conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mSubConditionNodes;
};

/**
 * Node representing an OR condition of a branch
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - The successful sub-condition is not considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound
 */
class HTNOrConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the nodes representing the sub-conditions of the condition
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetSubConditionNodes() const;

private:
    // Nodes representing the sub-conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mSubConditionNodes;
};

/**
 * Node representing an ALT condition of a branch
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - If it binds any unbound arguments of the successful sub-condition, the successful sub-condition is considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound
 */
class HTNAltConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the nodes representing the sub-conditions of the condition
    HTN_NODISCARD const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetSubConditionNodes() const;

private:
    // Nodes representing the sub-conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mSubConditionNodes;
};

/**
 * Node representing a NOT condition of a branch
 * - In order to succeed, its sub-condition must fail
 * - It does not bind the unbound arguments of its sub-condition
 * - Its sub-condition is not considered for backtracking
 * - If it succeeds, only the arguments that were already bound will still be bound
 */
class HTNNotConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inSubConditionNode);

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;

    // Returns the node representing the sub-conditions of the condition
    HTN_NODISCARD const std::shared_ptr<const HTNConditionNodeBase>& GetSubConditionNode() const;

private:
    // Node representing the sub-condition of the condition
    std::shared_ptr<const HTNConditionNodeBase> mSubConditionNode;
};

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNConditionNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNConditionNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::shared_ptr<const HTNIdentifierExpressionNode>& HTNAxiomConditionNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& HTNAxiomConditionNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNAndConditionNode::GetSubConditionNodes() const
{
    return mSubConditionNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNOrConditionNode::GetSubConditionNodes() const
{
    return mSubConditionNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNAltConditionNode::GetSubConditionNodes() const
{
    return mSubConditionNodes;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNNotConditionNode::GetSubConditionNode() const
{
    return mSubConditionNode;
}
