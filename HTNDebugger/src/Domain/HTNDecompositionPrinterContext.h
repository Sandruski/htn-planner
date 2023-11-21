// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionTooltipMode.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNFrameworkMinimal.h"
#include "HTNPathHandler.h"

#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

class HTNDecompositionChoicePointNodeState;
class HTNDecompositionNode;
class HTNDecompositionNodeState;
class HTNDecompositionSnapshotDebug;
class HTNDomainNode;
enum class HTNNodeStep : uint8;

class HTNDecompositionPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                            const HTNDecompositionSnapshotDebug& inDecompositionSnapshot,
                                            const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreImGuiState,
                                            std::unordered_map<std::string, HTNDecompositionNodeState>&            ioNodeStates,
                                            std::unordered_map<std::string, HTNDecompositionChoicePointNodeState>& ioChoicePointNodeStates,
                                            HTNDecompositionNode&                                                  ioSelectedNode);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const std::string&                          GetEntryPointID() const;
    const HTNDecompositionSnapshotDebug&        GetDecompositionSnapshot() const;
    HTNDecompositionTooltipMode                 GetTooltipMode() const;
    bool                                        ShouldIgnoreImGuiState() const;

    void SelectNode(const HTNDecompositionNode& inNode);
    void UnselectSelectedNode();
    bool IsNodeSelected(const std::string& inNodeLabel) const;

    void                             AddNodeState(const std::string& inNodePath, const HTNDecompositionNodeState& inNodeState);
    const HTNDecompositionNodeState& GetNodeState(const std::string& inNodePath) const;
    HTNDecompositionNodeState&       GetNodeStateMutable(const std::string& inNodePath);
    const HTNDecompositionNodeState* FindNodeState(const std::string& inNodePath) const;
    HTNDecompositionNodeState*       FindNodeStateMutable(const std::string& inNodePath);
    void AddChoicePointNodeState(const std::string& inNodePath, const HTNDecompositionChoicePointNodeState& inChoicePointNodeState);
    const HTNDecompositionChoicePointNodeState& GetChoicePointNodeState(const std::string& inNodePath) const;
    HTNDecompositionChoicePointNodeState&       GetChoicePointNodeStateMutable(const std::string& inNodePath);
    const HTNDecompositionChoicePointNodeState* FindChoicePointNodeState(const std::string& inNodePath) const;
    HTNDecompositionChoicePointNodeState*       FindChoicePointNodeStateMutable(const std::string& inNodePath);

    HTNNodeStep GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    int32       GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    bool        IsNodeOpen(const std::string& inNodePath, const int32 inDecompositionStep, const bool inIsChoicePoint) const;

    const HTNPathHandler& GetCurrentNodePathHandler() const;
    HTNPathHandler&       GetCurrentNodePathHandlerMutable();
    const HTNPathHandler& GetCurrentVariablesPathHandler() const;
    HTNPathHandler&       GetCurrentVariablesPathHandlerMutable();

    void  SetCurrentDecompositionStep(const int32 inCurrentDecompositionStep);
    int32 GetCurrentDecompositionStep() const;
    bool  IsCurrentDecompositionStepValid() const;
    void  SetMinDecompositionStep(const int32 inMinDecompositionStep);
    int32 GetMinDecompositionStep() const;
    void  SetMaxDecompositionStep(const int32 inMaxDecompositionStep);
    int32 GetMaxDecompositionStep() const;

    void SetIsCurrentNodeVisible(bool inIsCurrentNodeVisible);
    bool IsCurrentNodeVisible() const;
    bool IsSelectedNodeSelected() const;
    void RefreshNodeStates();
    bool ShouldRefreshNodeStates() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const std::string&                          mEntryPointID;
    const HTNDecompositionSnapshotDebug&        mDecompositionSnapshot;
    const HTNDecompositionTooltipMode           mTooltipMode            = HTNDecompositionTooltipMode::NONE;
    const bool                                  mShouldIgnoreImGuiState = false;

    //----------------------------------------------------------------------//
    // Input/Output
    //----------------------------------------------------------------------//
    // Node path to node state
    std::unordered_map<std::string, HTNDecompositionNodeState>& mNodeStates;

    // Node path to choice point node state
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState>& mChoicePointNodeStates;

    HTNDecompositionNode& mSelectedNode;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    // Path from the root node to the current node being processed
    HTNPathHandler mCurrentNodePathHandler;

    // Path from the root node to the current node determining the scope of the variables
    HTNPathHandler mCurrentVariablesPathHandler;

    // Current decomposition step
    int32 mCurrentDecompositionStep = 0;

    // Minimum decomposition step that the current decomposition step can be
    // - Inclusive
    int32 mMinDecompositionStep = std::numeric_limits<int32>::min();

    // Maximum decomposition step that the current decomposition step can be
    // - Exclusive
    int32 mMaxDecompositionStep = std::numeric_limits<int32>::max();

    // Whether the current node is shown or hidden
    bool mIsCurrentNodeVisible = true;

    // Whether the selected node is still selected
    bool mIsSelectedNodeSelected = false;

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

inline const HTNDecompositionSnapshotDebug& HTNDecompositionPrinterContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}

inline HTNDecompositionTooltipMode HTNDecompositionPrinterContext::GetTooltipMode() const
{
    return mTooltipMode;
}

inline bool HTNDecompositionPrinterContext::ShouldIgnoreImGuiState() const
{
    return mShouldIgnoreImGuiState;
}

inline const HTNPathHandler& HTNDecompositionPrinterContext::GetCurrentNodePathHandler() const
{
    return mCurrentNodePathHandler;
}

inline HTNPathHandler& HTNDecompositionPrinterContext::GetCurrentNodePathHandlerMutable()
{
    return mCurrentNodePathHandler;
}

inline const HTNPathHandler& HTNDecompositionPrinterContext::GetCurrentVariablesPathHandler() const
{
    return mCurrentVariablesPathHandler;
}

inline HTNPathHandler& HTNDecompositionPrinterContext::GetCurrentVariablesPathHandlerMutable()
{
    return mCurrentVariablesPathHandler;
}

inline void HTNDecompositionPrinterContext::SetCurrentDecompositionStep(const int32 inCurrentDecompositionStep)
{
    mCurrentDecompositionStep = inCurrentDecompositionStep;
}

inline int32 HTNDecompositionPrinterContext::GetCurrentDecompositionStep() const
{
    return mCurrentDecompositionStep;
}

inline void HTNDecompositionPrinterContext::SetMinDecompositionStep(const int32 inMinDecompositionStep)
{
    mMinDecompositionStep = inMinDecompositionStep;
}

inline int32 HTNDecompositionPrinterContext::GetMinDecompositionStep() const
{
    return mMinDecompositionStep;
}

inline void HTNDecompositionPrinterContext::SetMaxDecompositionStep(const int32 inMaxDecompositionStep)
{
    mMaxDecompositionStep = inMaxDecompositionStep;
}

inline int32 HTNDecompositionPrinterContext::GetMaxDecompositionStep() const
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

inline bool HTNDecompositionPrinterContext::IsSelectedNodeSelected() const
{
    return mIsSelectedNodeSelected;
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
