// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNPathManager.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>
#include <vector>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionSnapshot.h"
#endif

class HTNDecompositionRecord;
class HTNDomainNode;
class HTNWorldState;

#ifdef HTN_DEBUG_DECOMPOSITION
enum class HTNNodeStep : uint8;
#endif

// Previous records of a decomposition
using HTNDecompositionRecordHistory = std::vector<HTNDecompositionRecord>;

/**
 * Contextual data of a decomposition
 */
class HTNDecompositionContext final : public HTNNodeVisitorContextBase
{
public:
    HTNDecompositionContext() = default;
    explicit HTNDecompositionContext(const HTNWorldState& inWorldState, const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                     const std::string& inEntryPointID);

    // Returns the world state
    HTN_NODISCARD const HTNWorldState& GetWorldState() const;

    // Returns the domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

    // Returns the entry point ID
    HTN_NODISCARD const std::string& GetEntryPointID() const;

    // Pushes the given decomposition record to the decomposition record history
    void RecordDecomposition(const HTNDecompositionRecord& inDecompositionRecord);

    // Pops a decomposition record from the decomposition record history and sets it as the current decomposition
    HTN_NODISCARD bool RestoreDecomposition();

    // Returns the current decomposition
    HTN_NODISCARD const HTNDecompositionRecord& GetCurrentDecomposition() const;

    // Returns the current decomposition
    HTN_NODISCARD HTNDecompositionRecord& GetCurrentDecompositionMutable();

    // Sets the decomposition record history to the given one
    void SetDecompositionRecordHistory(const HTNDecompositionRecordHistory& inDecompositionRecordHistory);

    // Returns the decomposition record history
    HTN_NODISCARD const HTNDecompositionRecordHistory& GetDecompositionRecordHistory() const;

    // Sets the current node path manager to the given one
    void SetCurrentNodePathManager(const HTNPathManager& inPathManager);

    // Returns the current node path manager
    HTN_NODISCARD const HTNPathManager& GetCurrentNodePathManager() const;

    // Returns the current node path manager
    HTN_NODISCARD HTNPathManager& GetCurrentNodePathManagerMutable();

    // Sets the current variables path manager to the given one
    void SetCurrentVariablesPathManager(const HTNPathManager& inPathManager);

    // Returns the current variables path manager
    HTN_NODISCARD const HTNPathManager& GetCurrentVariablesPathManager() const;

    // Returns the current variables path manager
    HTN_NODISCARD HTNPathManager& GetCurrentVariablesPathManagerMutable();

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const HTNWorldState&                        mWorldState;
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const std::string&                          mEntryPointID;

    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNDecompositionRecord mCurrentDecomposition;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    HTNDecompositionRecordHistory mDecompositionRecordHistory;

    // Manager of the path from the root node to the current node being processed
    HTNPathManager mCurrentNodePathManager;

    // Manager of the path from the root node to the current node that determines the scope of the variables
    HTNPathManager mCurrentVariablesPathManager;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    // Records a snapshot of the given node
    void RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    // Records a snapshot of the given node
    void RecordNodeSnapshot(const std::string& inNodePath, const bool inNodeResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    // Returns the decomposition snapshot
    HTN_NODISCARD const HTNDecompositionSnapshot& GetDecompositionSnapshot() const;

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNDecompositionSnapshot mDecompositionSnapshot;
#endif
};

inline const HTNWorldState& HTNDecompositionContext::GetWorldState() const
{
    return mWorldState;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionContext::GetDomainNode() const
{
    return mDomainNode;
}

inline const std::string& HTNDecompositionContext::GetEntryPointID() const
{
    return mEntryPointID;
}

inline void HTNDecompositionContext::RecordDecomposition(const HTNDecompositionRecord& inDecompositionRecord)
{
    mDecompositionRecordHistory.emplace_back(inDecompositionRecord);
}

inline const HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecomposition() const
{
    return mCurrentDecomposition;
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
    return mCurrentDecomposition;
}

inline void HTNDecompositionContext::SetDecompositionRecordHistory(const HTNDecompositionRecordHistory& inDecompositionHistory)
{
    mDecompositionRecordHistory = inDecompositionHistory;
}

inline const HTNDecompositionRecordHistory& HTNDecompositionContext::GetDecompositionRecordHistory() const
{
    return mDecompositionRecordHistory;
}

inline void HTNDecompositionContext::SetCurrentNodePathManager(const HTNPathManager& inPathManager)
{
    mCurrentNodePathManager = inPathManager;
}

inline const HTNPathManager& HTNDecompositionContext::GetCurrentNodePathManager() const
{
    return mCurrentNodePathManager;
}

inline HTNPathManager& HTNDecompositionContext::GetCurrentNodePathManagerMutable()
{
    return mCurrentNodePathManager;
}

inline void HTNDecompositionContext::SetCurrentVariablesPathManager(const HTNPathManager& inPathManager)
{
    mCurrentVariablesPathManager = inPathManager;
}

inline const HTNPathManager& HTNDecompositionContext::GetCurrentVariablesPathManager() const
{
    return mCurrentVariablesPathManager;
}

inline HTNPathManager& HTNDecompositionContext::GetCurrentVariablesPathManagerMutable()
{
    return mCurrentVariablesPathManager;
}

#ifdef HTN_DEBUG_DECOMPOSITION
inline const HTNDecompositionSnapshot& HTNDecompositionContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}
#endif
