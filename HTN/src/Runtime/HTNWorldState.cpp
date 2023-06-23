#include "HTNWorldState.h"

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

	const HTNFact& Fact = FactIt->second;
	const std::size_t TableIndex = inArgs.size() - 1;
	const HTNTableBase* Table = Fact[TableIndex];
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

	const HTNFact& Fact = FactIt->second;
	const int TableIndex = inNumArgs - 1;
	const HTNTableBase* Table = Fact[TableIndex];
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

	const HTNFact& Fact = FactIt->second;
	const unsigned int TableIndex = inNumArgs - 1;
	const HTNTableBase* Table = Fact[TableIndex];
	if (!Table)
	{
		return 0;
	}

	return Table->GetNumEntries();
}
