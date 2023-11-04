#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
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
using HTNNodeFunction         = std::function<HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot)>;

typedef int ImGuiTreeNodeFlags;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const bool inShouldPrintFullTooltip,
                                     const bool inShouldResetNodeStates);

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

    void SelectNode(const std::string& inNodeLabel, const HTNDecompositionNode& inNode);
    void RefreshSelectedNode(const HTNDecompositionNode& inNode);
    bool IsNodeSelected(const std::string& inNodeLabel) const;

    bool IsCurrentDecompositionStepValid() const;

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug& mDecompositionSnapshot;
    bool                                 mShouldPrintFullTooltip = false;

    bool mShouldResetNodeStates  = false;
    bool mShouldUpdateNodeStates = false;
    bool mIsNodeOpenStateDirty   = false;

    HTNDecompositionNode mCurrentSelectedNode;
    bool                 mIsCurrentSelectedNodeSelected = false;
    static std::string   mCurrentSelectedNodeLabel;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

    // Current decomposition step
    // - -1 means all of them
    int mCurrentDecompositionStep = 0;

    // Minimum decomposition step that the current decomposition step can be
    // - Inclusive
    int mMinDecompositionStep = std::numeric_limits<int>::min();

    // Maximum decomposition step that the current decomposition step can be
    // - Exclusive
    int mMaxDecompositionStep = std::numeric_limits<int>::max();

    bool mIsCurrentNodeVisible = true;

    // Node path to node state
    static std::unordered_map<std::string, HTNDecompositionNodeState> mNodeStates;

    // Choice point node path to node state
    static std::unordered_map<std::string, HTNDecompositionChoicePointNodeState> mChoicePointNodeStates;
};

inline HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                        const HTNDecompositionSnapshotDebug& inDecompositionSnapshot,
                                                        const bool inShouldPrintFullTooltip, const bool inShouldResetNodeStates)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot),
      mShouldPrintFullTooltip(inShouldPrintFullTooltip), mShouldResetNodeStates(inShouldResetNodeStates)
{
}

inline void HTNDecompositionPrinter::SelectNode(const std::string& inNodeLabel, const HTNDecompositionNode& inNode)
{
    mCurrentSelectedNodeLabel = inNodeLabel;
    RefreshSelectedNode(inNode);
}

inline void HTNDecompositionPrinter::RefreshSelectedNode(const HTNDecompositionNode& inNode)
{
    mCurrentSelectedNode           = inNode;
    mIsCurrentSelectedNodeSelected = true;
}

inline bool HTNDecompositionPrinter::IsNodeSelected(const std::string& inNodeLabel) const
{
    return (inNodeLabel == mCurrentSelectedNodeLabel);
}
#endif
