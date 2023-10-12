#pragma once

#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <functional>
#include <memory>
#include <string>

class HTNDecompositionSnapshotDebug;
class HTNDomainNode;
class HTNNodeSnapshotDebug;

using HTNNodeSelectionFunction = std::function<void(const HTNNodeSnapshotDebug& inNodeSnapshot)>;
using HTNNodeBehaviorFunction = std::function<void()>;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     const HTNDecompositionSnapshotDebug& inDecompositionSnapshot);

    bool Print(HTNDecompositionNode& ioSelectedNode);

    HTNAtom Visit(const HTNDomainNode& inDomainNode) final;

    HTNAtom Visit(const HTNAxiomNode& inAxiomNode) final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode) final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode) final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode) final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode) final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode) final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode) final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode) final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode) final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode) final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode) final;
    HTNAtom Visit(const HTNValueNode& inValueNode) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    bool PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const std::string& inNodeDescription,
                                  const HTNNodeSelectionFunction& inNodeSelectionFunction,
                                  const HTNNodeBehaviorFunction* inNodeBehaviorFunction);

    void SetDecompositionStepRange(const size_t inMinDecompositionStep, const size_t inMaxDecompositionStep);
    bool IsDecompositionStepBetweenRange(const size_t inDecompositionStep) const;

    void SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot);
    void SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments);
    bool IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const;

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug& mDecompositionSnapshot;

    HTNDecompositionNode mSelectedNode;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

    size_t mMinDecompositionStep = 0;
    size_t mMaxDecompositionStep = std::numeric_limits<std::size_t>::max();
};

inline void HTNDecompositionPrinter::SetDecompositionStepRange(const size_t inMinDecompositionStep, const size_t inMaxDecompositionStep)
{
    mMinDecompositionStep = inMinDecompositionStep;
    mMaxDecompositionStep = inMaxDecompositionStep;
}

inline bool HTNDecompositionPrinter::IsDecompositionStepBetweenRange(const size_t inDecompositionStep) const
{
    return (inDecompositionStep >= mMinDecompositionStep) && (inDecompositionStep < mMaxDecompositionStep);
}

inline void HTNDecompositionPrinter::SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot)
{
    mSelectedNode = HTNDecompositionNode(inNodeSnapshot);
}

inline void HTNDecompositionPrinter::SelectNode(const HTNNodeSnapshotDebug&                                 inNodeSnapshot,
                                                const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments)
{
    mSelectedNode = HTNDecompositionNode(inNodeSnapshot, inNodeArguments, mCurrentVariableScopeNodePath);
}

inline bool HTNDecompositionPrinter::IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const
{
    return mSelectedNode.GetNodeSnapshot() == &inNodeSnapshot;
}
