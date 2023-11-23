// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNPathHandler.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>
#include <vector>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionResult.h"
#endif

class HTNDecompositionRecord;
class HTNDomainNode;
class HTNWorldState;

#ifdef HTN_DEBUG_DECOMPOSITION
enum class HTNNodeStep : uint8;
#endif

using HTNDecompositionRecordHistory = std::vector<HTNDecompositionRecord>;

class HTNDecompositionContext final : public HTNNodeVisitorContextBase
{
public:
    HTNDecompositionContext() = default;
    explicit HTNDecompositionContext(const HTNWorldState& inWorldState, const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                     const std::string& inEntryPointID);

    const HTNWorldState&                        GetWorldState() const;
    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const std::string&                          GetEntryPointID() const;

    void RecordDecomposition(const HTNDecompositionRecord& inDecomposition);
    bool RestoreDecomposition();

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

    void                                 SetDecompositionRecordHistory(const HTNDecompositionRecordHistory& inDecompositionRecordHistory);
    const HTNDecompositionRecordHistory& GetDecompositionRecordHistory() const;

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
    HTNDecompositionRecordHistory mDecompositionRecordHistory;

    // Path from the root node to the current node being processed
    HTNPathHandler mCurrentNodePathHandler;

    // Path from the root node to the current node determining the scope of the variables
    HTNPathHandler mCurrentVariablesPathHandler;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    void RecordNodeResult(const std::string& inNodePath, const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
    void RecordNodeResult(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    const HTNDecompositionResult& GetDecompositionResult() const;

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNDecompositionResult mDecompositionResult;
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

inline void HTNDecompositionContext::RecordDecomposition(const HTNDecompositionRecord& inDecomposition)
{
    mDecompositionRecordHistory.emplace_back(inDecomposition);
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
inline const HTNDecompositionResult& HTNDecompositionContext::GetDecompositionResult() const
{
    return mDecompositionResult;
}
#endif
