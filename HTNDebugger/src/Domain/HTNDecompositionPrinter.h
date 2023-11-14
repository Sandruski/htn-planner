#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionWatchTooltipPrinter.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

class HTNDecompositionSnapshotDebug;
class HTNDomainNode;
class HTNNodeSnapshotDebug;
enum class HTNNodeStep : unsigned char;

using HTNNodeTitleFunction    = std::function<void(const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep)>;
using HTNNodeBehaviorFunction = std::function<void()>;
using HTNNodeFunction         = std::function<HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel)>;

typedef int ImGuiTreeNodeFlags;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    bool Print(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
               const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const HTNDecompositionTooltipMode inTooltipMode,
               const bool inShouldIgnoreNewNodeOpen, const bool inShouldReset, HTNDecompositionNode& ioSelectedNode);

    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) final;

private:
    bool PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const HTNNodeTitleFunction& inNodeTitleFunction,
                                  const HTNNodeBehaviorFunction* inNodeBehaviorFunction, const HTNNodeFunction& inNodeFunction,
                                  const ImGuiTreeNodeFlags inTreeNodeFlags);

    void SelectNode(const HTNDecompositionNode& inNode);
    void UnselectSelectedNode();
    bool IsSelectedNodeSelected() const;
    bool IsNodeSelected(const std::string& inNodeLabel) const;

    bool IsCurrentDecompositionStepValid() const;

    HTNNodeStep GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    int         GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    bool        IsNodeOpen(const std::string& inNodePath, const int inDecompositionStep, const bool inIsChoicePoint) const;

    void Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
               const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const HTNDecompositionTooltipMode inTooltipMode,
               const bool inShouldIgnoreNewNodeOpen, const bool inShouldReset, HTNDecompositionNode& ioSelectedNode);

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug* mDecompositionSnapshot   = nullptr;
    HTNDecompositionTooltipMode          mTooltipMode             = HTNDecompositionTooltipMode::NONE;
    bool                                 mShouldIgnoreNewNodeOpen = false;
    HTNDecompositionNode*                mSelectedNode            = nullptr;

    // Node path to node state
    std::unordered_map<std::string, HTNDecompositionNodeState> mNodeStates;

    // Node path to choice point node state
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState> mChoicePointNodeStates;

    // Whether the selected node is still selected
    bool mIsSelectedNodeSelected = false;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

    // Current decomposition step
    int mCurrentDecompositionStep = 0;

    // Minimum decomposition step that the current decomposition step can be
    // - Inclusive
    int mMinDecompositionStep = std::numeric_limits<int>::min();

    // Maximum decomposition step that the current decomposition step can be
    // - Exclusive
    int mMaxDecompositionStep = std::numeric_limits<int>::max();

    // Whether the current node is shown or hidden
    bool mIsCurrentNodeVisible = true;

    // Whether to refresh the node states
    bool mShouldRefreshNodeStates = false;

    HTNDecompositionWatchTooltipPrinter mDecompositionWatchTooltipPrinter;
};

inline void HTNDecompositionPrinter::SelectNode(const HTNDecompositionNode& inNode)
{
    *mSelectedNode          = inNode;
    mIsSelectedNodeSelected = true;
}

inline void HTNDecompositionPrinter::UnselectSelectedNode()
{
    *mSelectedNode          = HTNDecompositionNode();
    mIsSelectedNodeSelected = false;
}

inline bool HTNDecompositionPrinter::IsSelectedNodeSelected() const
{
    return mIsSelectedNodeSelected;
}

inline bool HTNDecompositionPrinter::IsNodeSelected(const std::string& inNodeLabel) const
{
    const std::string& SelectedNodeLabel = mSelectedNode->GetNodeLabel();
    return (inNodeLabel == SelectedNodeLabel);
}
#endif
