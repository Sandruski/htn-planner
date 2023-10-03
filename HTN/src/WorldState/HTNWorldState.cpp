#include "WorldState/HTNWorldState.h"

#include "HTNLog.h"

#include <cassert>

#ifdef HTN_DEBUG
#include <format>
#endif

namespace
{
unsigned int CountFactArgumentsBound(const std::vector<HTNAtom>& inFactArguments)
{
    unsigned int FactArgumentsBoundNum = 0;

    for (const HTNAtom& FactArgument : inFactArguments)
    {
        if (FactArgument.IsSet())
        {
            ++FactArgumentsBoundNum;
        }
    }

    return FactArgumentsBoundNum;
}
} // namespace

bool HTNFactArgumentsTable::AddUniqueFactArguments(const std::vector<HTNAtom>& inFactArguments)
{
    if (ContainsFactArguments(inFactArguments))
    {
#ifdef HTN_DEBUG
        std::string FactArgumentsDescription;

        for (const HTNAtom& FactArgument : inFactArguments)
        {
            static constexpr bool ShouldDoubleQuoteString = true;
            FactArgumentsDescription.append(std::format("{} ", FactArgument.ToString(ShouldDoubleQuoteString)));
        }

        // Remove last whitespace
        const std::size_t Index = FactArgumentsDescription.find_last_of(" ");
        if (Index != std::string::npos)
        {
            FactArgumentsDescription.erase(Index);
        }
#endif
        LOG_ERROR("Fact arguments [{}] already contained", FactArgumentsDescription);
        return false;
    }

    AddFactArguments(inFactArguments);

    return true;
}

void HTNFactArgumentsTable::RemoveFactArguments(const std::size_t inFactArgumentsIndex)
{
    if (inFactArgumentsIndex >= mFactArguments.size())
    {
        LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArguments.size());
        return;
    }

    const auto It = mFactArguments.begin() + inFactArgumentsIndex;
    mFactArguments.erase(It);
}

bool HTNFactArgumentsTable::Check(const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const
{
    if (inFactArgumentsIndex >= mFactArguments.size())
    {
        LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArguments.size());
        return false;
    }

    const HTNFactArguments& FactArguments     = mFactArguments[inFactArgumentsIndex];
    const size_t            FactArgumentsSize = ioFactArguments.size();
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);

    // Check arguments bound
    for (std::size_t i = 0; i < FactArgumentsSize; ++i)
    {
        const HTNAtom& ioFactArgument = ioFactArguments[i];
        if (!ioFactArgument.IsSet())
        {
            continue;
        }

        const HTNAtom& FactArgument = FactArguments[i];
        if (ioFactArgument != FactArgument)
        {
            return false;
        }
    }

    // Bind arguments unbound
    for (std::size_t i = 0; i < FactArgumentsSize; ++i)
    {
        HTNAtom& ioFactArgument = ioFactArguments[i];
        if (ioFactArgument.IsSet())
        {
            continue;
        }

        const HTNAtom& FactArgument = FactArguments[i];
        ioFactArgument              = FactArgument;
    }

    return true;
}

bool HTNFactArgumentsTable::ContainsFactArguments(const std::vector<HTNAtom>& inFactArguments) const
{
    const std::size_t FactArgumentsSize = inFactArguments.size();
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (const HTNFactArguments& FactArguments : mFactArguments)
    {
        bool Result = true;
        for (std::size_t i = 0; i < FactArgumentsSize; ++i)
        {
            const HTNAtom& FactArgument   = FactArguments[i];
            const HTNAtom& inFactArgument = inFactArguments[i];
            if (FactArgument != inFactArgument)
            {
                Result = false;
                break;
            }
        }

        if (Result)
        {
            return true;
        }
    }

    return false;
}

void HTNFactArgumentsTable::AddFactArguments(const std::vector<HTNAtom>& inFactArguments)
{
    HTNFactArguments FactArguments;
    assert(inFactArguments.size() < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (std::size_t i = 0; i < inFactArguments.size(); ++i)
    {
        FactArguments[i] = inFactArguments[i];
    }

    mFactArguments.emplace_back(FactArguments);
}

void HTNWorldState::AddFact(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments)
{
    HTNFactArgumentsTables& FactArgumentsTables = mFacts[inFactID];
    assert(inFactArguments.size() < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArguments.size()];
    FactArgumentsTable.AddUniqueFactArguments(inFactArguments);
}

void HTNWorldState::RemoveFact(const std::string& inFactID, const std::size_t inFactArgumentsSize, const std::size_t inFactArgumentsIndex)
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

std::size_t HTNWorldState::Query(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments) const
{
    const unsigned int FactArgumentsBoundNum = CountFactArgumentsBound(inFactArguments);
    const std::size_t  FactArgumentsSize     = inFactArguments.size();
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return 1;
    }

    return GetFactArgumentsSize(inFactID, FactArgumentsSize);
}

bool HTNWorldState::QueryIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const
{
    const unsigned int FactArgumentsBoundNum = CountFactArgumentsBound(ioFactArguments);
    if (FactArgumentsBoundNum == ioFactArguments.size())
    {
        // All arguments are bound
        return true;
    }

    return CheckIndex(inFactID, inFactArgumentsIndex, ioFactArguments);
}

bool HTNWorldState::CheckIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(ioFactArguments.size() < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[ioFactArguments.size()];
    return FactArgumentsTable.Check(inFactArgumentsIndex, ioFactArguments);
}

std::size_t HTNWorldState::GetFactArgumentsTablesSize(const std::string& inFactID) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return 0;
    }

    std::size_t FactArgumentsTablesSize = 0;

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    for (const HTNFactArgumentsTable& FactArgumentsTable : FactArgumentsTables)
    {
        const std::size_t FactArgumentsSize = FactArgumentsTable.GetFactArgumentsSize();
        if (FactArgumentsSize > 0)
        {
            ++FactArgumentsTablesSize;
        }
    }

    return FactArgumentsTablesSize;
}

bool HTNWorldState::ContainsFactArgumentTable(const std::string& inFactID, const std::size_t inFactArgumentsSize) const
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
    const std::size_t            FactArgumentsSize  = FactArgumentsTable.GetFactArgumentsSize();
    return (FactArgumentsSize > 0);
}

std::size_t HTNWorldState::GetFactArgumentsSize(const std::string& inFactID, const std::size_t inFactArgumentsSize) const
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

bool HTNWorldState::ContainsFactArguments(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    assert(inFactArguments.size() < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[inFactArguments.size()];
    return FactArgumentsTable.ContainsFactArguments(inFactArguments);
}
