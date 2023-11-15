#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

class HTNDecompositionSnapshotDebug;
class HTNDomainNode;

class HTNDecompositionPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                            const HTNDecompositionSnapshotDebug&                                         inDecompositionSnapshot,
                                            const std::unordered_map<std::string, HTNDecompositionNodeState>&            inNodeStates,
                                            const std::unordered_map<std::string, HTNDecompositionChoicePointNodeState>& inChoicePointNodeStates,
                                            const HTNDecompositionNode& inSelectedNode, const HTNDecompositionTooltipMode inTooltipMode,
                                            const bool inShouldIgnoreNewNodeOpen);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const std::string&                          GetEntryPointID() const;
    const HTNDecompositionSnapshotDebug*        GetDecompositionSnapshot() const;
    HTNDecompositionTooltipMode                 GetTooltipMode() const;
    bool                                        ShouldIgnoreNewNodeOpen() const;

    void                             AddNodeState(const std::string& inNodePath, const HTNDecompositionNodeState& inNodeState);
    const HTNDecompositionNodeState* FindNodeState(const std::string& inNodePath) const;
    HTNDecompositionNodeState*       FindNodeStateMutable(const std::string& inNodePath);
    void AddChoicePointNodeState(const std::string& inNodePath, const HTNDecompositionChoicePointNodeState& inChoicePointNodeState);
    const HTNDecompositionChoicePointNodeState* FindChoicePointNodeState(const std::string& inNodePath) const;
    HTNDecompositionChoicePointNodeState*       FindChoicePointNodeStateMutable(const std::string& inNodePath);

    const HTNNodePath& GetCurrentNodePath() const;
    HTNNodePath&       GetCurrentNodePathMutable();
    const HTNNodePath& GetCurrentVariableScopeNodePath() const;
    HTNNodePath&       GetCurrentVariableScopeNodePathMutable();

    void                        SelectNode(const HTNDecompositionNode& inNode);
    void                        UnselectSelectedNode();
    bool                        IsSelectedNodeSelected() const;
    bool                        IsNodeSelected(const std::string& inNodeLabel) const;
    const HTNDecompositionNode& GetSelectedNode() const;

    void SetCurrentDecompositionStep(const int inCurrentDecompositionStep);
    int  GetCurrentDecompositionStep() const;
    bool IsCurrentDecompositionStepValid() const;
    void SetMinDecompositionStep(const int inMinDecompositionStep);
    int  GetMinDecompositionStep() const;
    void SetMaxDecompositionStep(const int inMaxDecompositionStep);
    int  GetMaxDecompositionStep() const;

    HTNNodeStep GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    int         GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    bool        IsNodeOpen(const std::string& inNodePath, const int inDecompositionStep, const bool inIsChoicePoint) const;

    void SetIsCurrentNodeVisible(bool inIsCurrentNodeVisible);
    bool IsCurrentNodeVisible() const;
    void RefreshNodeStates();
    bool ShouldRefreshNodeStates() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    const HTNDecompositionSnapshotDebug* mDecompositionSnapshot   = nullptr;
    HTNDecompositionTooltipMode          mTooltipMode             = HTNDecompositionTooltipMode::NONE;
    bool                                 mShouldIgnoreNewNodeOpen = false;

    // Node path to node state
    std::unordered_map<std::string, HTNDecompositionNodeState> mNodeStates;

    // Node path to choice point node state
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState> mChoicePointNodeStates;

    // Selected node
    HTNDecompositionNode mSelectedNode;

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
};

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionPrinterContext::GetDomainNode() const
{
    return mDomainNode;
}

inline const std::string& HTNDecompositionPrinterContext::GetEntryPointID() const
{
    return mEntryPointID;
}

inline const HTNDecompositionSnapshotDebug* HTNDecompositionPrinterContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}

inline HTNDecompositionTooltipMode HTNDecompositionPrinterContext::GetTooltipMode() const
{
    return mTooltipMode;
}

inline bool HTNDecompositionPrinterContext::ShouldIgnoreNewNodeOpen() const
{
    return mShouldIgnoreNewNodeOpen;
}

inline void HTNDecompositionPrinterContext::AddNodeState(const std::string& inNodePath, const HTNDecompositionNodeState& inNodeState)
{
    mNodeStates[inNodePath] = inNodeState;
}

inline const HTNDecompositionNodeState* HTNDecompositionPrinterContext::FindNodeState(const std::string& inNodePath) const
{
    const auto It = mNodeStates.find(inNodePath);
    return ((It != mNodeStates.end()) ? &It->second : nullptr);
}

inline HTNDecompositionNodeState* HTNDecompositionPrinterContext::FindNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mNodeStates.find(inNodePath);
    return ((It != mNodeStates.end()) ? &It->second : nullptr);
}

inline void HTNDecompositionPrinterContext::AddChoicePointNodeState(const std::string&                          inNodePath,
                                                                    const HTNDecompositionChoicePointNodeState& inChoicePointNodeState)
{
    mChoicePointNodeStates[inNodePath] = inChoicePointNodeState;
}

inline const HTNDecompositionChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeState(const std::string& inNodePath) const
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return ((It != mChoicePointNodeStates.end()) ? &It->second : nullptr);
}

inline HTNDecompositionChoicePointNodeState* HTNDecompositionPrinterContext::FindChoicePointNodeStateMutable(const std::string& inNodePath)
{
    const auto It = mChoicePointNodeStates.find(inNodePath);
    return ((It != mChoicePointNodeStates.end()) ? &It->second : nullptr);
}

inline const HTNNodePath& HTNDecompositionPrinterContext::GetCurrentNodePath() const
{
    return mCurrentNodePath;
}

inline HTNNodePath& HTNDecompositionPrinterContext::GetCurrentNodePathMutable()
{
    return mCurrentNodePath;
}

inline const HTNNodePath& HTNDecompositionPrinterContext::GetCurrentVariableScopeNodePath() const
{
    return mCurrentVariableScopeNodePath;
}

inline HTNNodePath& HTNDecompositionPrinterContext::GetCurrentVariableScopeNodePathMutable()
{
    return mCurrentVariableScopeNodePath;
}

inline bool HTNDecompositionPrinterContext::IsSelectedNodeSelected() const
{
    return mIsSelectedNodeSelected;
}

inline const HTNDecompositionNode& HTNDecompositionPrinterContext::GetSelectedNode() const
{
    return mSelectedNode;
}

inline void HTNDecompositionPrinterContext::SetCurrentDecompositionStep(const int inCurrentDecompositionStep)
{
    mCurrentDecompositionStep = inCurrentDecompositionStep;
}

inline int HTNDecompositionPrinterContext::GetCurrentDecompositionStep() const
{
    return mCurrentDecompositionStep;
}

inline void HTNDecompositionPrinterContext::SetMinDecompositionStep(const int inMinDecompositionStep)
{
    mMinDecompositionStep = inMinDecompositionStep;
}

inline int HTNDecompositionPrinterContext::GetMinDecompositionStep() const
{
    return mMinDecompositionStep;
}

inline void HTNDecompositionPrinterContext::SetMaxDecompositionStep(const int inMaxDecompositionStep)
{
    mMaxDecompositionStep = inMaxDecompositionStep;
}

inline int HTNDecompositionPrinterContext::GetMaxDecompositionStep() const
{
    return mMaxDecompositionStep;
}

inline void HTNDecompositionPrinterContext::SetIsCurrentNodeVisible(bool inIsCurrentNodeVisible)
{
    mIsCurrentNodeVisible = inIsCurrentNodeVisible;
}

inline bool HTNDecompositionPrinterContext::IsCurrentNodeVisible() const
{
    return mIsCurrentNodeVisible;
}

inline void HTNDecompositionPrinterContext::RefreshNodeStates()
{
    mShouldRefreshNodeStates = true;
}

inline bool HTNDecompositionPrinterContext::ShouldRefreshNodeStates() const
{
    return mShouldRefreshNodeStates;
}
#endif
