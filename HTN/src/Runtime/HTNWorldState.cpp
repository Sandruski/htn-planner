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
