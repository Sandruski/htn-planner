#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionSnapshotDebug;
class HTNDomainNode;
class HTNNodeSnapshotDebug;
class HTNValueNodeBase;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     const HTNDecompositionSnapshotDebug& inDecompositionSnapshot);

    bool Print(const HTNNodeSnapshotDebug*& ioSelectedNodeSnapshot, std::vector<std::shared_ptr<const HTNValueNodeBase>>& ioSelectedNodeArguments);

    HTNAtom Visit(const HTNDomainNode& inDomainNode) final;
    HTNAtom Visit(const HTNConstantsNode& inConstantsNode) final;
    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
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
    void SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot);
    void SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments);
    bool IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const;

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug& mDecompositionSnapshot;

    const HTNNodeSnapshotDebug*                          mSelectedNodeSnapshot = nullptr;
    std::vector<std::shared_ptr<const HTNValueNodeBase>> mSelectedNodeArguments;

    HTNDecompositionContext mDecompositionContext;
};

inline void HTNDecompositionPrinter::SelectNode(const HTNNodeSnapshotDebug& inNodeSnapshot)
{
    mSelectedNodeSnapshot = &inNodeSnapshot;
    mSelectedNodeArguments.clear();
}

inline void HTNDecompositionPrinter::SelectNode(const HTNNodeSnapshotDebug&                                 inNodeSnapshot,
                                                const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inNodeArguments)
{
    mSelectedNodeSnapshot  = &inNodeSnapshot;
    mSelectedNodeArguments = inNodeArguments;
}

inline bool HTNDecompositionPrinter::IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const
{
    return mSelectedNodeSnapshot == &inNodeSnapshot;
}
