// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "WorldState/HTNWorldState.h"

void HTNFactArgumentsTable::RemoveFactArguments(const size inFactArgumentsIndex)
{
    if (inFactArgumentsIndex >= mFactArgumentsCollection.size())
    {
        HTN_LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArgumentsCollection.size());
        return;
    }

    auto It = mFactArgumentsCollection.cbegin();
    std::advance(It, inFactArgumentsIndex);
    mFactArgumentsCollection.erase(It);
}

void HTNWorldState::RemoveFact(const std::string& inFactID, const size inFactArgumentsSize, const size inFactArgumentsIndex)
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return;
    }

    HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArgumentsSize];
    FactArgumentsTable.RemoveFactArguments(inFactArgumentsIndex);
}

size HTNWorldState::GetFactArgumentsTablesSize(const std::string& inFactID) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return 0;
    }

    size FactArgumentsTablesSize = 0;

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
    {
        const size FactArgumentsCollectionSize = FactArgumentsTable.GetFactArgumentsCollectionSize();
        if (FactArgumentsCollectionSize > 0)
        {
            ++FactArgumentsTablesSize;
        }
    }

    return FactArgumentsTablesSize;
}

bool HTNWorldState::ContainsFactArgumentTable(const std::string& inFactID, const size inFactArgumentsSize) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    const HTNFactArgumentsTable& FactArgumentsTable          = FactArgumentsTables[inFactArgumentsSize];
    const size                   FactArgumentsCollectionSize = FactArgumentsTable.GetFactArgumentsCollectionSize();
    return FactArgumentsCollectionSize > 0;
}

size HTNWorldState::GetFactArgumentsCollectionSize(const std::string& inFactID, const size inFactArgumentsSize) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return 0;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArgumentsSize];
    return FactArgumentsTable.GetFactArgumentsCollectionSize();
}
