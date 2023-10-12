#pragma once

#include "Domain/HTNNodePath.h"

#include <memory>
#include <vector>

class HTNNodeSnapshotDebug;
class HTNValueNodeBase;

class HTNDecompositionNode
{
public:
    HTNDecompositionNode() = default;
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot);
    explicit HTNDecompositionNode(const HTNNodeSnapshotDebug&                                 inNodeSnapshot,
                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments,
                                  const HTNNodePath&                                          inNodeVariableScopeNodePath);
    const HTNNodeSnapshotDebug*                                 GetNodeSnapshot() const;
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& GetNodeArguments() const;
    const HTNNodePath&                                          GetNodeVariableScopeNodePath() const;

private:
    // TODO salvarez Store variables but still select node
    // TODO salvarez And rename class HTNSelectedNode?
    const HTNNodeSnapshotDebug*                          mNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNValueNodeBase>> mNodeArguments;
    HTNNodePath                                          mNodeVariableScopeNodePath;
};

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot) : mNodeSnapshot(&inNodeSnapshot)
{
}

inline HTNDecompositionNode::HTNDecompositionNode(const HTNNodeSnapshotDebug&                                 inNodeSnapshot,
                                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments,
                                                  const HTNNodePath&                                          inNodeVariableScopeNodePath)
    : mNodeSnapshot(&inNodeSnapshot), mNodeArguments(inNodeArguments), mNodeVariableScopeNodePath(inNodeVariableScopeNodePath)
{
}

inline const HTNNodeSnapshotDebug* HTNDecompositionNode::GetNodeSnapshot() const
{
    return mNodeSnapshot;
}

inline const std::vector<std::shared_ptr<const HTNValueNodeBase>>& HTNDecompositionNode::GetNodeArguments() const
{
    return mNodeArguments;
}

inline const HTNNodePath& HTNDecompositionNode::GetNodeVariableScopeNodePath() const
{
    return mNodeVariableScopeNodePath;
}
