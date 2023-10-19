#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <functional>
#include <memory>
#include <string>

class HTNDecompositionSnapshotDebug;
class HTNDomainNode;
class HTNNodeSnapshotDebug;

using HTNNodeTitleFunction    = std::function<void(const HTNNodeSnapshotDebug& inNodeSnapshot)>;
using HTNNodeBehaviorFunction = std::function<void()>;
using HTNNodeFunction         = std::function<HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot)>;

typedef int ImGuiTreeNodeFlags;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const bool inShouldPrintFullTooltip);

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
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

private:
    bool PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const HTNNodeTitleFunction& inNodeTitleFunction,
                                  const HTNNodeBehaviorFunction* inNodeBehaviorFunction, const HTNNodeFunction& inNodeFunction,
                                  const ImGuiTreeNodeFlags inTreeNodeFlags);

    void SetDecompositionStepRange(const int inMinDecompositionStep, const std::size_t inMaxDecompositionStep);
    bool IsDecompositionStepBetweenRange(const std::size_t inDecompositionStep) const;

    void SelectNode(const HTNDecompositionNode& inNode);
    bool IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const;

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug& mDecompositionSnapshot;
    bool                 mShouldPrintFullTooltip = false;

    HTNDecompositionNode mSelectedNode;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

    int         mMinDecompositionStep = -1;
    std::size_t mMaxDecompositionStep = std::numeric_limits<std::size_t>::max();
};

inline HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                        const HTNDecompositionSnapshotDebug& inDecompositionSnapshot,
                                                        const bool                           inShouldPrintFullTooltip)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot), mShouldPrintFullTooltip(inShouldPrintFullTooltip)
{
}

inline void HTNDecompositionPrinter::SetDecompositionStepRange(const int inMinDecompositionStep, const std::size_t inMaxDecompositionStep)
{
    mMinDecompositionStep = inMinDecompositionStep;
    mMaxDecompositionStep = inMaxDecompositionStep;
}

inline bool HTNDecompositionPrinter::IsDecompositionStepBetweenRange(const std::size_t inDecompositionStep) const
{
    return (static_cast<int>(inDecompositionStep) > mMinDecompositionStep) && (inDecompositionStep <= mMaxDecompositionStep);
}

inline void HTNDecompositionPrinter::SelectNode(const HTNDecompositionNode& inNode)
{
    mSelectedNode = inNode;
}

inline bool HTNDecompositionPrinter::IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const
{
    return mSelectedNode.GetNodeSnapshot() == &inNodeSnapshot;
}
#endif
