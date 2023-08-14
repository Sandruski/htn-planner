#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNWorldState.h"

#include <array>
#include <memory>
#include <string>
#include <type_traits>

class HTNAtom;
class HTNDecompositionContext;
class HTNValue;
class HTNBranch;

/**
 * Condition world state query
 * - Queries the arguments in the database
 * - Binds unbound arguments
 */
class HTNConditionWorldStateQuery
{
public:
    HTNConditionWorldStateQuery() = default;
    explicit HTNConditionWorldStateQuery(const std::string& inKey, const std::vector<HTNAtom*>& inArguments);

    // Check if the condition is true
    bool Check(const HTNWorldState& inWorldState, const int inIndex) const;

    void SetKey(const std::string& inKey);
    void AddArgument(HTNAtom& inArgument);

private:
    std::string           mKey;
    std::vector<HTNAtom*> mArguments;
};

/**
 * Base condition
 * - HTNCondition
 * - HTNConditionAnd
 * - HTNConditionOr
 * - HTNConditionAlt
 * - HTNConditionNot
 */
class HTNConditionBase : public HTNNodeBase
{
public:
    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;

    virtual bool Check(HTNDecompositionContext& ioDecompositionContext) const = 0;

    void                                    SetParent(const std::weak_ptr<const HTNNodeBase>& inParent);
    const std::weak_ptr<const HTNNodeBase>& GetParent() const;

private:
    // HTNBranch or HTNAxiom or HTNConditionAnd or HTNConditionOr or HTNConditionAlt or HTNConditionNot
    std::weak_ptr<const HTNNodeBase> mParent;
};

/**
 * Condition
 * - Queries the arguments in the database
 * - Binds unbound arguments
 * // TODO salvarez Force to have an AND at least, we cannot have this condition alone
 */
class HTNCondition final : public HTNConditionBase, public std::enable_shared_from_this<HTNCondition>
{
public:
    explicit HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments,
                          const bool inIsAxiom);

    std::string ToString() const final;
    bool        Check(HTNDecompositionContext& ioDecompositionContext) const final;

    std::string GetName() const;

private:
    std::unique_ptr<const HTNValue>              mName;
    std::vector<std::shared_ptr<const HTNValue>> mArguments;
    bool                                         mIsAxiom = false;
};

/**
 * AND condition
 * - sub-condition && sub-condition && ...
 * - In order to succeed, all of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of its sub-conditions
 * - If it binds any unbound arguments of a sub-condition, the sub-condition is considered for backtracking
 * - If it succeeds, the arguments of its sub-conditions will be bound
 */
class HTNConditionAnd final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionAnd>
{
public:
    std::string ToString() const final;
    bool        Check(HTNDecompositionContext& ioDecompositionContext) const final;

    void SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);
    const std::vector<std::shared_ptr<const HTNConditionBase>>& GetSubConditions() const;

private:
    std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * OR condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - The successful sub-condition is not considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect
 * this case during parsing
 */
class HTNConditionOr final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionOr>
{
public:
    std::string ToString() const final;
    bool        Check(HTNDecompositionContext& ioDecompositionContext) const final;

    void SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);
    const std::vector<std::shared_ptr<const HTNConditionBase>>& GetSubConditions() const;

private:
    std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * ALT condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - If it binds any unbound arguments of the successful sub-condition, the successful sub-condition is considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect
 * this case during parsing
 */
class HTNConditionAlt final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionAlt>
{
public:
    std::string ToString() const final;
    bool        Check(HTNDecompositionContext& ioDecompositionContext) const final;

    void SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);
    const std::vector<std::shared_ptr<const HTNConditionBase>>& GetSubConditions() const;

private:
    std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * NOT condition
 * - !sub-condition
 * - In order to succeed, its sub-condition must fail
 * - It does not bind the unbound arguments of its sub-condition
 * - Its sub-condition is not considered for backtracking
 * - If it succeeds, only the arguments that were already bound will still be bound // TODO salvarez Detect this case during parsing
 */
class HTNConditionNot final : public HTNConditionBase
{
public:
    std::string ToString() const final;
    bool        Check(HTNDecompositionContext& ioDecompositionContext) const final;

    void                                           SetSubCondition(const std::shared_ptr<const HTNConditionBase>& inSubCondition);
    const std::shared_ptr<const HTNConditionBase>& GetSubCondition() const;

private:
    std::shared_ptr<const HTNConditionBase> mSubCondition;
};

inline HTNConditionWorldStateQuery::HTNConditionWorldStateQuery(const std::string& inKey, const std::vector<HTNAtom*>& inArguments)
    : mKey(inKey), mArguments(inArguments)
{
}

inline void HTNConditionWorldStateQuery::SetKey(const std::string& inKey)
{
    mKey = inKey;
}

inline void HTNConditionWorldStateQuery::AddArgument(HTNAtom& inArgument)
{
    mArguments.emplace_back(&inArgument);
}

inline void HTNConditionBase::SetParent(const std::weak_ptr<const HTNNodeBase>& inParent)
{
    mParent = inParent;
}

inline const std::weak_ptr<const HTNNodeBase>& HTNConditionBase::GetParent() const
{
    return mParent;
}

inline void HTNConditionAnd::SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions)
{
    mSubConditions = inSubConditions;
}

inline const std::vector<std::shared_ptr<const HTNConditionBase>>& HTNConditionAnd::GetSubConditions() const
{
    return mSubConditions;
}

inline void HTNConditionOr::SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions)
{
    mSubConditions = inSubConditions;
}

inline const std::vector<std::shared_ptr<const HTNConditionBase>>& HTNConditionOr::GetSubConditions() const
{
    return mSubConditions;
}

inline void HTNConditionAlt::SetSubConditions(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions)
{
    mSubConditions = inSubConditions;
}

inline const std::vector<std::shared_ptr<const HTNConditionBase>>& HTNConditionAlt::GetSubConditions() const
{
    return mSubConditions;
}

inline void HTNConditionNot::SetSubCondition(const std::shared_ptr<const HTNConditionBase>& inSubCondition)
{
    mSubCondition = inSubCondition;
}

inline const std::shared_ptr<const HTNConditionBase>& HTNConditionNot::GetSubCondition() const
{
    return mSubCondition;
}
