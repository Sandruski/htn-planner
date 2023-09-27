#include "WorldState/HTNWorldState.h"

void HTNWorldState::AddFact(const char* inKey, const std::vector<HTNAtom>& inArgs)
{
    const size_t ArgsSize = inArgs.size();
    assert(ArgsSize <= MaxNumArgs);

    HTNFact&      Fact  = mFacts[inKey];
    HTNTableBase* Table = Fact[ArgsSize];
    if (!Table)
    {
        if (ArgsSize == 0)
        {
            Fact[ArgsSize] = Table = new HTNTable<0>();
        }
        else if (ArgsSize == 1)
        {
            Fact[ArgsSize] = Table = new HTNTable<1>();
        }
        else if (ArgsSize == 2)
        {
            Fact[ArgsSize] = Table = new HTNTable<2>();
        }
        else if (ArgsSize == 3)
        {
            Fact[ArgsSize] = Table = new HTNTable<3>();
        }
        else if (ArgsSize == 4)
        {
            Fact[ArgsSize] = Table = new HTNTable<4>();
        }
        else if (ArgsSize == 5)
        {
            Fact[ArgsSize] = Table = new HTNTable<5>();
        }
        else if (ArgsSize == 6)
        {
            Fact[ArgsSize] = Table = new HTNTable<6>();
        }
        else if (ArgsSize == 7)
        {
            Fact[ArgsSize] = Table = new HTNTable<7>();
        }
        else if (ArgsSize == 8)
        {
            Fact[ArgsSize] = Table = new HTNTable<8>();
        }
    }

    Table->AddEntry(inArgs);
}

void HTNWorldState::RemoveFact(const char* inKey, const int inArgsSize, const int inIndex)
{
    const std::unordered_map<std::string, HTNFact>::const_iterator FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        return;
    }

    const HTNFact& Fact = FactIt->second;
    if (inArgsSize < 0 || inArgsSize >= Fact.size())
    {
        return;
    }

    HTNTableBase* Table = Fact[inArgsSize];
    if (!Table)
    {
        return;
    }

    Table->RemoveEntry(inIndex);
}

void HTNWorldState::RemoveAllFacts()
{
    mFacts.clear();
}

bool HTNWorldState::CheckIndex(const char* inKey, const int inIndex, const std::vector<HTNAtom*>& inArgs) const
{
    const std::unordered_map<std::string, HTNFact>::const_iterator FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return false;
    }

    if (inArgs.empty())
    {
        return true;
    }

    const HTNFact&      Fact  = FactIt->second;
    const HTNTableBase* Table = Fact[inArgs.size()];
    if (!Table)
    {
        // Table not found
        return false;
    }

    return Table->Check(inIndex, inArgs);
}

int HTNWorldState::GetNumFactTables(const char* inKey) const
{
    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return -1;
    }

    int NumFactTables = 0;

    const HTNFact& Fact = FactIt->second;
    for (const HTNTableBase* Table : Fact)
    {
        if (Table)
        {
            ++NumFactTables;
        }
    }

    return NumFactTables;
}

int HTNWorldState::GetNumFactTablesByNumArgs(const char* inKey, const int inNumArgs) const
{
    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return -1;
    }

    if (inNumArgs == 0)
    {
        return 0;
    }

    const HTNFact&      Fact  = FactIt->second;
    const HTNTableBase* Table = Fact[inNumArgs];
    if (!Table)
    {
        return 0;
    }

    return 1;
}

unsigned int HTNWorldState::GetNumFactEntries(const std::string& inKey, const unsigned int inNumArgs) const
{
    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        return 0;
    }

    if (inNumArgs == 0)
    {
        return 1;
    }

    const HTNFact&      Fact  = FactIt->second;
    const HTNTableBase* Table = Fact[inNumArgs];
    if (!Table)
    {
        return 0;
    }

    return Table->GetNumEntries();
}
