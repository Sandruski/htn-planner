#pragma once

#include <string>
#include <vector>

class HTNAtom;
class HTNWorldState;

class HTNConditionQueryBase
{
};

/**
 * Condition world state query
 * - Queries the arguments in the database
 * - Binds unbound arguments
 */
class HTNConditionQueryWorldState final : public HTNConditionQueryBase
{
public:
    HTNConditionQueryWorldState() = default;
    explicit HTNConditionQueryWorldState(const std::string& inKey, const std::vector<HTNAtom*>& inArguments);

    // Check if the condition is true
    bool Check(const HTNWorldState& inWorldState, const int inIndex) const;

    void SetKey(const std::string& inKey);
    void AddArgument(HTNAtom& inArgument);

private:
    std::string           mKey;
    std::vector<HTNAtom*> mArguments;
};

inline HTNConditionQueryWorldState::HTNConditionQueryWorldState(const std::string& inKey, const std::vector<HTNAtom*>& inArguments)
    : mKey(inKey), mArguments(inArguments)
{
}

inline void HTNConditionQueryWorldState::SetKey(const std::string& inKey)
{
    mKey = inKey;
}

inline void HTNConditionQueryWorldState::AddArgument(HTNAtom& inArgument)
{
    mArguments.emplace_back(&inArgument);
}
