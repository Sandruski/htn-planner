// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Core/HTNPathHandler.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>
#include <vector>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#endif

class HTNDomainNode;
class HTNWorldState;

class HTNDecompositionContext final : public HTNNodeVisitorContextBase
{
public:
    HTNDecompositionContext() = default;
    explicit HTNDecompositionContext(const HTNWorldState& inWorldState, const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                     const std::string& inEntryPointID);

    const HTNWorldState&                        GetWorldState() const;
    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const std::string&                          GetEntryPointID() const;

    void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
    bool RestoreDecomposition();

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

    void                                       SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory);
    const std::vector<HTNDecompositionRecord>& GetDecompositionHistory() const;

    void                  SetCurrentNodePathHandler(const HTNPathHandler& inCurrentNodePathHandler);
    const HTNPathHandler& GetCurrentNodePathHandler() const;
    HTNPathHandler&       GetCurrentNodePathHandlerMutable();

    void                  SetCurrentVariablesPathHandler(const HTNPathHandler& inCurrentVariablesPathHandler);
    const HTNPathHandler& GetCurrentVariablesPathHandler() const;
    HTNPathHandler&       GetCurrentVariablesPathHandlerMutable();

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
    // Current decomposition
    HTNDecompositionRecord mCurrentDecomposition;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    // Record of previous decompositions
    std::vector<HTNDecompositionRecord> mDecompositionHistory;

    // Path from the root node to the current node being processed
    HTNPathHandler mCurrentNodePathHandler;

    // Path from the root node to the current node determining the scope of the variables
    HTNPathHandler mCurrentVariablesPathHandler;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    void RecordNodeSnapshot(const std::string& inNodePath, const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
    void RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    const HTNDecompositionSnapshotDebug& GetDecompositionSnapshot() const;

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNDecompositionSnapshotDebug mDecompositionSnapshot;
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

inline void HTNDecompositionContext::RecordDecomposition(HTNDecompositionRecord& inDecomposition)
{
    mDecompositionHistory.emplace_back(inDecomposition);
}

inline const HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecomposition() const
{
    return mCurrentDecomposition;
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
    return mCurrentDecomposition;
}

inline void HTNDecompositionContext::SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory)
{
    mDecompositionHistory = inDecompositionHistory;
}

inline const std::vector<HTNDecompositionRecord>& HTNDecompositionContext::GetDecompositionHistory() const
{
    return mDecompositionHistory;
}

inline void HTNDecompositionContext::SetCurrentNodePathHandler(const HTNPathHandler& inNodePathHandler)
{
    mCurrentNodePathHandler = inNodePathHandler;
}

inline const HTNPathHandler& HTNDecompositionContext::GetCurrentNodePathHandler() const
{
    return mCurrentNodePathHandler;
}

inline HTNPathHandler& HTNDecompositionContext::GetCurrentNodePathHandlerMutable()
{
    return mCurrentNodePathHandler;
}

inline void HTNDecompositionContext::SetCurrentVariablesPathHandler(const HTNPathHandler& inVariablesPathHandler)
{
    mCurrentVariablesPathHandler = inVariablesPathHandler;
}

inline const HTNPathHandler& HTNDecompositionContext::GetCurrentVariablesPathHandler() const
{
    return mCurrentVariablesPathHandler;
}

inline HTNPathHandler& HTNDecompositionContext::GetCurrentVariablesPathHandlerMutable()
{
    return mCurrentVariablesPathHandler;
}

#ifdef HTN_DEBUG_DECOMPOSITION
inline const HTNDecompositionSnapshotDebug& HTNDecompositionContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}
#endif
