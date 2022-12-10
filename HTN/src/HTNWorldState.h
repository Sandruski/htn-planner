#pragma once

#include "HTNAtom.h"

#include <string>
#include <unordered_map>
#include <vector>

// Contains the array with a list of arguments that corresponds to certain HtnWorldState key.
class HTNTable
{
public:
	explicit HTNTable(const int inNumberOfArguments);

	// Adds an entry with 1 argument
	template <typename taArg0>
	void AddEntry(const taArg0& inArg0);

	// Adds an entry with 2 arguments
	template <typename taArg0, typename taArg1>
	void AddEntry(const taArg0& inArg0, const taArg1& inArg1);

	// Returns the number of arguments
	int GetNumArgs() const;

private:
	std::vector<std::vector<HTNAtom>> mEntries;
};

inline HTNTable::HTNTable(const int inNumberOfArguments)
{
	mEntries.resize(inNumberOfArguments);
}

template <typename taArg0>
inline void HTNTable::AddEntry(const taArg0& inArg0)
{
	std::vector<HTNAtom>& Arg0 = mEntries[0];
	Arg0.emplace_back(inArg0);
}

template <typename taArg0, typename taArg1>
inline void HTNTable::AddEntry(const taArg0& inArg0, const taArg1& inArg1)
{
	std::vector<HTNAtom>& Arg0 = mEntries[0];
	Arg0.emplace_back(inArg0);

	std::vector<HTNAtom>& Arg1 = mEntries[1];
	Arg1.emplace_back(inArg1);
}

inline int HTNTable::GetNumArgs() const
{
	return static_cast<int>(mEntries.size());
}

// World state for the HTN planner. 
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
	// Writes a fact with 0 arguments
	void MakeFact(const char* inKey);

	// Writes a fact with 1 argument
	template <typename taArg0>
	void MakeFact(const char* inKey, const taArg0& inArg0);

	// Writes a fact with 2 arguments
	template <typename taArg0, typename taArg1>
	void MakeFact(const char* inKey, const taArg0& inArg0, const taArg1& inArg1);

	// Returns the number of HTNTables registered for the fact inKey.
	int GetNumFactTables(const char* inKey) const;

	// Returns the number of HTNTables registered for the fact inKey and the inNumArguments. This function should only return 0 or 1.
	// If it does returns something different then this will cause problems later on during decomposition.
	int GetNumFactTablesByArgNumber(const char* inKey, const int inNumArguments) const;

private:
	std::unordered_map<std::string, std::vector<HTNTable>> mFacts;
};

inline void HTNWorldState::MakeFact(const char* inKey)
{
	mFacts[inKey];
}

template <typename taArg0>
inline void HTNWorldState::MakeFact(const char* inKey, const taArg0& inArg0)
{
	std::vector<HTNTable>& Tables = mFacts[inKey];
	const std::vector<HTNTable>::iterator TablesIt = std::find_if(Tables.begin(), Tables.end(), [](const HTNTable& _Other)
		{
			return (_Other.GetNumArgs() == 1);
		});
	if (TablesIt == Tables.end())
	{
		HTNTable& Table = Tables.emplace_back(HTNTable(1));
		Table.AddEntry(inArg0);
	}
	else
	{
		TablesIt->AddEntry(inArg0);
	}
}

template <typename taArg0, typename taArg1>
inline void HTNWorldState::MakeFact(const char* inKey, const taArg0& inArg0, const taArg1& inArg1)
{
	std::vector<HTNTable>& Tables = mFacts[inKey];
	const std::vector<HTNTable>::iterator TablesIt = std::find_if(Tables.begin(), Tables.end(), [](const HTNTable& _Other)
		{
			return (_Other.GetNumArgs() == 2);
		});
	if (TablesIt == Tables.end())
	{
		HTNTable& Table = Tables.emplace_back(HTNTable(2));
		Table.AddEntry(inArg0, inArg1);
	}
	else
	{
		TablesIt->AddEntry(inArg0, inArg1);
	}
}

inline int HTNWorldState::GetNumFactTables(const char* inKey) const
{
	const std::unordered_map<std::string, std::vector<HTNTable>>::const_iterator FactsIt = mFacts.find(inKey);
	if (FactsIt == mFacts.end())
	{
		return 0;
	}

	const std::vector<HTNTable>& Tables = FactsIt->second;
	return static_cast<int>(Tables.size());
}

inline int HTNWorldState::GetNumFactTablesByArgNumber(const char* inKey, const int inNumArguments) const
{
	const std::unordered_map<std::string, std::vector<HTNTable>>::const_iterator FactsIt = mFacts.find(inKey);
	if (FactsIt == mFacts.end())
	{
		return 0;
	}

	const std::vector<HTNTable>& Tables = FactsIt->second;
	const std::vector<HTNTable>::const_iterator TablesIt = std::find_if(Tables.begin(), Tables.end(), [&inNumArguments](const HTNTable& _Other)
		{
			return (_Other.GetNumArgs() == inNumArguments);
		});
	if (TablesIt == Tables.end())
	{
		return 0;
	}

	return 1;
}
