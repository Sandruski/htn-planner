// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNPathManager.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Domain/Printer/HTNDecompositionTooltipMode.h"
#include "Domain/Printer/HTNNodeStateFwd.h"
#include "HTNCoreMinimal.h"

#include <limits>
#include <memory>
#include <string>

class HTNDecompositionSnapshot;
class HTNDomainNode;
class HTNNodeInstance;
enum class HTNNodeStep : uint8;

/**
 * Contextual data of a decomposition printer
 */
class HTNDecompositionPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                            const HTNDecompositionSnapshot& inDecompositionSnapshot, const bool inIsDecompositionSuccessful,
                                            const HTNDecompositionTooltipMode inTooltipMode, const bool inShouldIgnoreImGuiState,
                                            HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                            HTNNodeInstance& ioSelectedNodeInstance);

    // Returns the domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

    // Returns the entry point ID
    HTN_NODISCARD const std::string& GetEntryPointID() const;

    // Returns the decomposition snapshot
    HTN_NODISCARD const HTNDecompositionSnapshot& GetDecompositionSnapshot() const;

    // Returns whether the decomposition is successful
    HTN_NODISCARD bool IsDecompositionSuccessful() const;

    // Returns the tooltip mode
    HTN_NODISCARD HTNDecompositionTooltipMode GetTooltipMode() const;

    // Returns whether the state of ImGui should be ignored
    HTN_NODISCARD bool ShouldIgnoreImGuiState() const;

    // Selects the given node instance
    void SelectNodeInstance(const HTNNodeInstance& inNodeInstance);

    // Returns whether the node of the given label is selected
    HTN_NODISCARD bool IsNodeSelected(const std::string& inNodeLabel) const;

    // Adds new state for the node at the given path
    void AddNodeState(const std::string& inNodePath, const HTNNodeState& inNodeState);

    // Returns the state of the node at the given path
    HTN_NODISCARD const HTNNodeState& GetNodeState(const std::string& inNodePath) const;

    // Returns the state of the node at the given path
    HTN_NODISCARD HTNNodeState& GetNodeStateMutable(const std::string& inNodePath);

    // Returns the state of the node at the given path
    HTN_NODISCARD const HTNNodeState* FindNodeState(const std::string& inNodePath) const;

    // Returns the state of the node at the given path
    HTN_NODISCARD HTNNodeState* FindNodeStateMutable(const std::string& inNodePath);

    // Adds new state for the choice point node at the given path
    void AddChoicePointNodeState(const std::string& inNodePath, const HTNChoicePointNodeState& inChoicePointNodeState);

    // Returns the state of the choice point node at the given path
    HTN_NODISCARD const HTNChoicePointNodeState& GetChoicePointNodeState(const std::string& inNodePath) const;

    // Returns the state of the choice point node at the given path
    HTN_NODISCARD HTNChoicePointNodeState& GetChoicePointNodeStateMutable(const std::string& inNodePath);

    // Returns the state of the choice point node at the given path
    HTN_NODISCARD const HTNChoicePointNodeState* FindChoicePointNodeState(const std::string& inNodePath) const;

    // Returns the state of the choice point node at the given path
    HTN_NODISCARD HTNChoicePointNodeState* FindChoicePointNodeStateMutable(const std::string& inNodePath);

    // Returns the node step of the node at the given path
    HTN_NODISCARD HTNNodeStep GetNodeStep(const std::string& inNodePath, const bool inIsChoicePoint) const;

    // Returns the decomposition step of the node at the given path
    HTN_NODISCARD int32 GetNodeDecompositionStep(const std::string& inNodePath, const bool inIsChoicePoint) const;

    // Returns whether the node at the given path is open at the given decomposition step
    HTN_NODISCARD bool IsNodeOpen(const std::string& inNodePath, const int32 inDecompositionStep, const bool inIsChoicePoint) const;

    // Returns the current node path manager
    HTN_NODISCARD const HTNPathManager& GetCurrentNodePathManager() const;

    // Returns the current node path manager
    HTN_NODISCARD HTNPathManager& GetCurrentNodePathManagerMutable();

    // Returns the current variables path manager
    HTN_NODISCARD const HTNPathManager& GetCurrentVariablesPathManager() const;

    // Returns the current variables path manager
    HTN_NODISCARD HTNPathManager& GetCurrentVariablesPathManagerMutable();

    // Sets the current decomposition step to the given one
    void SetCurrentDecompositionStep(const int32 inDecompositionStep);

    // Returns the current decomposition step
    HTN_NODISCARD int32 GetCurrentDecompositionStep() const;

    // Returns whether the current decomposition step is valid
    HTN_NODISCARD bool IsCurrentDecompositionStepValid() const;

    // Sets the minimum decomposition step to the given one
    void SetMinDecompositionStep(const int32 inMinDecompositionStep);

    // Returns the minimum decomposition step
    HTN_NODISCARD int32 GetMinDecompositionStep() const;

    // Sets the maximum decomposition step to the given one
    void SetMaxDecompositionStep(const int32 inMaxDecompositionStep);

    // Returns the maximum decomposition step
    HTN_NODISCARD int32 GetMaxDecompositionStep() const;

    // Sets whether the current node is visible
    void SetIsCurrentNodeVisible(const bool inIsNodeVisible);

    // Returns whether the current node is visible
    HTN_NODISCARD bool IsCurrentNodeVisible() const;

    // Sets the states of the nodes to be refreshed
    void RefreshNodeStates();

    // Returns whether the states of the nodes should be refreshed
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
    // Manager of the path from the root node to the current node being processed
    HTNPathManager mCurrentNodePathManager;

    // Manager of the path from the root node to the current node that determines the scope of the variables
    HTNPathManager mCurrentVariablesPathManager;

    int32 mCurrentDecompositionStep = 0;

    // Minimum decomposition step that the current decomposition step can be
    // - Inclusive
    int32 mMinDecompositionStep = std::numeric_limits<int32>::min();

    // Maximum decomposition step that the current decomposition step can be
    // - Exclusive
    int32 mMaxDecompositionStep = std::numeric_limits<int32>::max();

    // Whether the current node is shown or hidden
    bool mIsCurrentNodeVisible = true;

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

inline void HTNDecompositionPrinterContext::SetCurrentDecompositionStep(const int32 inDecompositionStep)
{
    mCurrentDecompositionStep = inDecompositionStep;
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

inline void HTNDecompositionPrinterContext::SetIsCurrentNodeVisible(const bool inIsNodeVisible)
{
    mIsCurrentNodeVisible = inIsNodeVisible;
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
