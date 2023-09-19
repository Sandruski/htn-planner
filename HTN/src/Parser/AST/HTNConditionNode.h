#pragma once

#include "Parser/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNValueNode;

/**
 * Base node representing a condition
 */
class HTNConditionNodeBase : public HTNNodeBase
{
public:
    HTNConditionNodeBase();

    std::string GetID() const final;

private:
    // Generator of IDs
    static inline unsigned int mIDGenerator = 0;

    // ID of the condition
    // - Globally unique
    const unsigned int mID = 0;
};

/**
 * Node representing a condition
 * - Queries the arguments in the database
 * - Binds unbound arguments
 */
class HTNConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNConditionNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                              const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&              GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNode>>& GetArgumentNodes() const;

private:
    // Node representing the ID of the condition
    // - ID of the fact in the world state
    // - Not unique
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the condition
    // - Arguments of the fact in the world state
    std::vector<std::shared_ptr<const HTNValueNode>> mArgumentNodes;
};

/**
 * Node representing an axiom condition
 * - Checks the condition of an axiom
 */
class HTNAxiomConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAxiomConditionNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNValueNode>&              GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNode>>& GetArgumentNodes() const;

private:
    // Node representing the ID of the condition
    // - ID of the axiom node in the domain node
    // - Not unique
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the condition
    // - Arguments of the axiom node
    std::vector<std::shared_ptr<const HTNValueNode>> mArgumentNodes;
};

/**
 * Node representing an AND condition
 * - sub-condition && sub-condition && ...
 * - In order to succeed, all of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of its sub-conditions
 * - If it binds any unbound arguments of a sub-condition, the sub-condition is considered for backtracking
 * - If it succeeds, the arguments of its sub-conditions will be bound
 */
class HTNAndConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetConditionNodes() const;

private:
    // Nodes representing the conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mConditionNodes;
};

/**
 * Node representing an OR condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - The successful sub-condition is not considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect
 * this case during parsing
 */
class HTNOrConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetConditionNodes() const;

private:
    // Nodes representing the conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mConditionNodes;
};

/**
 * Node representing an ALT condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - If it binds any unbound arguments of the successful sub-condition, the successful sub-condition is considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect
 * this case during parsing
 */
class HTNAltConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& GetConditionNodes() const;

private:
    // Nodes representing the conditions of the condition
    std::vector<std::shared_ptr<const HTNConditionNodeBase>> mConditionNodes;
};

/**
 * Node representing a NOT condition
 * - !sub-condition
 * - In order to succeed, its sub-condition must fail
 * - It does not bind the unbound arguments of its sub-condition
 * - Its sub-condition is not considered for backtracking
 * - If it succeeds, only the arguments that were already bound will still be bound // TODO salvarez Detect this case during parsing
 */
class HTNNotConditionNode final : public HTNConditionNodeBase
{
public:
    explicit HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inConditionNode);

    HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string ToString() const final;

    const std::shared_ptr<const HTNConditionNodeBase>& GetConditionNode() const;

private:
    // Node representing the condition of the condition
    std::shared_ptr<const HTNConditionNodeBase> mConditionNode;
};

inline HTNConditionNodeBase::HTNConditionNodeBase() : mID(mIDGenerator++)
{
}

inline const std::shared_ptr<const HTNValueNode>& HTNConditionNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNode>>& HTNConditionNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::shared_ptr<const HTNValueNode>& HTNAxiomConditionNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNode>>& HTNAxiomConditionNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNAndConditionNode::GetConditionNodes() const
{
    return mConditionNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNOrConditionNode::GetConditionNodes() const
{
    return mConditionNodes;
}

inline const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& HTNAltConditionNode::GetConditionNodes() const
{
    return mConditionNodes;
}

inline const std::shared_ptr<const HTNConditionNodeBase>& HTNNotConditionNode::GetConditionNode() const
{
    return mConditionNode;
}
