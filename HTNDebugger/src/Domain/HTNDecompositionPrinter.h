#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class HTNDomainNode;
class HTNNodeState;

using HTNNodeTitleFunction    = std::function<void(const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState)>;
using HTNNodeBehaviorFunction = std::function<void()>;
using HTNNodeFunction         = std::function<HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot)>;

typedef int ImGuiTreeNodeFlags;

enum class HTNNodeDirection : unsigned char
{
    TOP_DOWN,
    BOTTOM_UP,
    NONE
};

class HTNNodeState
{
public:
    HTNNodeState() = default;
    explicit HTNNodeState(const int inDecompositionStep, const HTNNodeStep inNodeStep, const HTNNodeDirection inNodeDirection);

    void             SetDecompositionStep(const int inDecompositionStep);
    int              GetDecompositionStep() const;
    void             SetNodeStep(const HTNNodeStep inNodeStep);
    HTNNodeStep      GetNodeStep() const;
    void             SetNodeDirection(const HTNNodeDirection inNodeDirection);
    HTNNodeDirection GetNodeDirection() const;

private:
    // Only used if HTNNodeDirection::BOTTOM_UP == mNodeDirection
    int              mDecompositionStep = -1;
    HTNNodeStep      mNodeStep          = HTNNodeStep::NONE;
    HTNNodeDirection mNodeDirection     = HTNNodeDirection::NONE;
};

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                     const HTNDecompositionSnapshotDebug& inDecompositionSnapshot, const bool inShouldPrintFullTooltip,
                                     const bool inShouldResetView);

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
    bool PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const HTNNodeStep inNodeStep, const HTNNodeTitleFunction& inNodeTitleFunction,
                                  const HTNNodeBehaviorFunction* inNodeBehaviorFunction, const HTNNodeFunction& inNodeFunction,
                                  const ImGuiTreeNodeFlags inTreeNodeFlags);

    void SelectNode(const HTNDecompositionNode& inNode);
    bool IsNodeSelected(const HTNNodeSnapshotDebug& inNodeSnapshot) const;

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug& mDecompositionSnapshot;
    bool                                 mShouldPrintFullTooltip = false;
    bool                                 mShouldResetView        = false;

    HTNDecompositionNode mSelectedNode;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

    inline static int         mCurrentDecompositionStep = 0;
    inline static HTNNodeStep mCurrentNodeStep          = HTNNodeStep::NONE;
    inline static bool        mIsChoicePointSelected    = false;

    inline static bool mShouldDisplay            = true;
    inline static bool mShouldDisplayChoicePoint = true;

    // Node path to node state
    static std::unordered_map<std::string, HTNNodeState> mNodeStates;
};

inline HTNNodeState::HTNNodeState(const int inDecompositionStep, const HTNNodeStep inNodeStep, const HTNNodeDirection inNodeDirection)
    : mDecompositionStep(inDecompositionStep), mNodeStep(inNodeStep), mNodeDirection(inNodeDirection)
{
}

inline void HTNNodeState::SetDecompositionStep(const int inDecompositionStep)
{
    mDecompositionStep = inDecompositionStep;
}

inline int HTNNodeState::GetDecompositionStep() const
{
    return mDecompositionStep;
}

inline void HTNNodeState::SetNodeStep(const HTNNodeStep inNodeStep)
{
    mNodeStep = inNodeStep;
}

inline HTNNodeStep HTNNodeState::GetNodeStep() const
{
    return mNodeStep;
}

inline void HTNNodeState::SetNodeDirection(const HTNNodeDirection inNodeDirection)
{
    mNodeDirection = inNodeDirection;
}

inline HTNNodeDirection HTNNodeState::GetNodeDirection() const
{
    return mNodeDirection;
}

inline HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                        const HTNDecompositionSnapshotDebug& inDecompositionSnapshot,
                                                        const bool inShouldPrintFullTooltip, const bool inShouldResetView)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot),
      mShouldPrintFullTooltip(inShouldPrintFullTooltip), mShouldResetView(inShouldResetView)
{
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
