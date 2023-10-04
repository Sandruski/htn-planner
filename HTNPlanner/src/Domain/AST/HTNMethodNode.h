#pragma once

#include "Domain/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNBranchNode;
class HTNValueNode;
class HTNValueNodeBase;

/**
 * Node representing a method
 */
class HTNMethodNode final : public HTNNodeBase
{
public:
    explicit HTNMethodNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                           const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                           const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel);
    ~HTNMethodNode();

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    std::string GetID() const final;

    const std::shared_ptr<const HTNValueNode>&                  GetIDNode() const;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& GetArgumentNodes() const;
    const std::vector<std::shared_ptr<const HTNBranchNode>>&    GetBranchNodes() const;
    bool                                                        IsTopLevel() const;

private:
    // Node representing the ID of the method
    // - Unique within its domain
    std::shared_ptr<const HTNValueNode> mIDNode;

    // Nodes representing the arguments of the method
    std::vector<std::shared_ptr<const HTNValueNodeBase>> mArgumentNodes;

    // Nodes representing the branches of the method
    std::vector<std::shared_ptr<const HTNBranchNode>> mBranchNodes;

    // Whether the method serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::shared_ptr<const HTNValueNode>& HTNMethodNode::GetIDNode() const
{
    return mIDNode;
}

inline const std::vector<std::shared_ptr<const HTNValueNodeBase>>& HTNMethodNode::GetArgumentNodes() const
{
    return mArgumentNodes;
}

inline const std::vector<std::shared_ptr<const HTNBranchNode>>& HTNMethodNode::GetBranchNodes() const
{
    return mBranchNodes;
}

inline bool HTNMethodNode::IsTopLevel() const
{
    return mIsTopLevel;
}
