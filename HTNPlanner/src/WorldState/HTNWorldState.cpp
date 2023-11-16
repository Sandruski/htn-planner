#include "WorldState/HTNWorldState.h"

void HTNFactArgumentsTable::RemoveFactArguments(const size inFactArgumentsIndex)
{
    if (inFactArgumentsIndex >= mFactArguments.size())
    {
        LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArguments.size());
        return;
    }

    const auto It = mFactArguments.begin() + inFactArgumentsIndex;
    mFactArguments.erase(It);
}

void HTNWorldState::RemoveFact(const std::string& inFactID, const size inFactArgumentsSize, const size inFactArgumentsIndex)
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return;
    }

    HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArgumentsSize];
    FactArgumentsTable.RemoveFactArguments(inFactArgumentsIndex);
}

size HTNWorldState::GetFactArgumentsTablesSize(const std::string& inFactID) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return 0;
    }

    size FactArgumentsTablesSize = 0;

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
    {
        const size FactArgumentsSize = FactArgumentsTable.GetFactArgumentsSize();
        if (FactArgumentsSize > 0)
        {
            ++FactArgumentsTablesSize;
        }
    }

    return FactArgumentsTablesSize;
}

bool HTNWorldState::ContainsFactArgumentTable(const std::string& inFactID, const size inFactArgumentsSize) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArgumentsSize];
    const size            FactArgumentsSize  = FactArgumentsTable.GetFactArgumentsSize();
    return (FactArgumentsSize > 0);
}

size HTNWorldState::GetFactArgumentsSize(const std::string& inFactID, const size inFactArgumentsSize) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return 0;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArgumentsSize];
    return FactArgumentsTable.GetFactArgumentsSize();
}
