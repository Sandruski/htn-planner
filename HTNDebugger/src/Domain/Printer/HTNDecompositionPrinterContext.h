// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNPathManager.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Domain/Printer/HTNDecompositionTooltipMode.h"
#include "Domain/Printer/HTNNodeState.h"
#include "HTNCoreMinimal.h"

#include <limits>
#include <memory>
#include <string>

class HTNChoicePointNodeState;
class HTNNodeInstance;
class HTNNodeState;
class HTNDecompositionSnapshot;
class HTNDomainNode;
enum class HTNNodeStep : uint8;

class HTNDecompositionPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                            const HTNDecompositionSnapshot& inDecompositionSnapshot, const bool inIsDecompositionSuccessful,
                                            const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreImGuiState,
                                            HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                            HTNNodeInstance& ioSelectedNodeInstance);

    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    HTN_NODISCARD const std::string&              GetEntryPointID() const;
    HTN_NODISCARD const HTNDecompositionSnapshot& GetDecompositionSnapshot() const;
    HTN_NODISCARD bool                            IsDecompositionSuccessful() const;
    HTN_NODISCARD HTNDecompositionTooltipMode     GetTooltipMode() const;
    HTN_NODISCARD bool                            ShouldIgnoreImGuiState() const;

    void               SelectNodeInstance(const HTNNodeInstance& inNodeInstance);
    HTN_NODISCARD bool IsNodeSelected(const std::string& inNodeLabel) const;

    void                              AddNodeState(const std::string& inNodePath, const HTNNodeState& inNodeState);
    HTN_NODISCARD const HTNNodeState& GetNodeState(const std::string& inNodePath) const;
    HTN_NODISCARD HTNNodeState&       GetNodeStateMutable(const std::string& inNodePath);
    HTN_NODISCARD const HTNNodeState* FindNodeState(const std::string& inNodePath) const;
    HTN_NODISCARD HTNNodeState*       FindNodeStateMutable(const std::string& inNodePath);
    void                              AddChoicePointNodeState(const std::string& inNodePath, const HTNChoicePointNodeState& inChoicePointNodeState);
    HTN_NODISCARD const HTNChoicePointNodeState& GetChoicePointNodeState(const std::string& inNodePath) const;
    HTN_NODISCARD HTNChoicePointNodeState&       GetChoicePointNodeStateMutable(const std::string& inNodePath);
    HTN_NODISCARD const HTNChoicePointNodeState* FindChoicePointNodeState(const std::string& inNodePath) const;
    HTN_NODISCARD HTNChoicePointNodeState*       FindChoicePointNodeStateMutable(const std::string& inNodePath);

    HTN_NODISCARD HTNNodeStep GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    HTN_NODISCARD int32       GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const;
    HTN_NODISCARD bool        IsNodeOpen(const std::string& inNodePath, const int32 inDecompositionStep, const bool inIsChoicePoint) const;

    HTN_NODISCARD const HTNPathManager& GetCurrentNodePathManager() const;
    HTN_NODISCARD HTNPathManager&       GetCurrentNodePathManagerMutable();
    HTN_NODISCARD const HTNPathManager& GetCurrentVariablesPathManager() const;
    HTN_NODISCARD HTNPathManager&       GetCurrentVariablesPathManagerMutable();

    void                SetCurrentDecompositionStep(const int32 inCurrentDecompositionStep);
    HTN_NODISCARD int32 GetCurrentDecompositionStep() const;
    HTN_NODISCARD bool  IsCurrentDecompositionStepValid() const;
    void                SetMinDecompositionStep(const int32 inMinDecompositionStep);
    HTN_NODISCARD int32 GetMinDecompositionStep() const;
    void                SetMaxDecompositionStep(const int32 inMaxDecompositionStep);
    HTN_NODISCARD int32 GetMaxDecompositionStep() const;

    void               SetIsCurrentNodeVisible(const bool inIsCurrentNodeVisible);
    HTN_NODISCARD bool IsCurrentNodeVisible() const;
    void               RefreshNodeStates();
    HTN_NODISCARD bool ShouldRefreshNodeStates() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const std::string&                          mEntryPointID;
    const HTNDecompositionSnapshot&             mDecompositionSnapshot;
    const bool                                  mIsDecompositionSuccessful = false;
    const HTNDecompositionTooltipMode           mTooltipMode               = HTNDecompositionTooltipMode::NONE;
    const bool                                  mShouldIgnoreImGuiState    = false;

    //----------------------------------------------------------------------//
    // Input/Output
    //----------------------------------------------------------------------//
    // Node path to node state
    HTNNodeStates& mNodeStates;

    // Node path to choice point node state
    HTNChoicePointNodeStates& mChoicePointNodeStates;

    HTNNodeInstance& mSelectedNodeInstance;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    // Path from the root node to the current node being processed
    HTNPathManager mCurrentNodePathManager;

    // Path from the root node to the current node determining the scope of the variables
    HTNPathManager mCurrentVariablesPathManager;

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

inline const HTNDecompositionSnapshot& HTNDecompositionPrinterContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}

inline bool HTNDecompositionPrinterContext::IsDecompositionSuccessful() const
{
    return mIsDecompositionSuccessful;
}

inline HTNDecompositionTooltipMode HTNDecompositionPrinterContext::GetTooltipMode() const
{
    return mTooltipMode;
}

inline bool HTNDecompositionPrinterContext::ShouldIgnoreImGuiState() const
{
    return mShouldIgnoreImGuiState;
}

inline const HTNPathManager& HTNDecompositionPrinterContext::GetCurrentNodePathManager() const
{
    return mCurrentNodePathManager;
}

inline HTNPathManager& HTNDecompositionPrinterContext::GetCurrentNodePathManagerMutable()
{
    return mCurrentNodePathManager;
}

inline const HTNPathManager& HTNDecompositionPrinterContext::GetCurrentVariablesPathManager() const
{
    return mCurrentVariablesPathManager;
}

inline HTNPathManager& HTNDecompositionPrinterContext::GetCurrentVariablesPathManagerMutable()
{
    return mCurrentVariablesPathManager;
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

inline void HTNDecompositionPrinterContext::SetIsCurrentNodeVisible(const bool inIsCurrentNodeVisible)
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
