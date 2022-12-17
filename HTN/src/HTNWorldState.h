#pragma once

#include "HTNAtom.h"

#include <algorithm>
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class HTNTableBase
{
public:
	virtual ~HTNTableBase() = 0;
};

inline HTNTableBase::~HTNTableBase() {}

// Contains the array with a list of arguments that corresponds to certain HtnWorldState key.
template <int NumArgs>
class HTNTable final : public HTNTableBase
{
public:
	typedef std::array<HTNAtom, NumArgs> HTNEntry;

	// Adds an entry with n arguments
	template <typename... Args>
	void AddEntry(Args&&... inArgs);

	int Query(int inIndex, HTNAtom& ioArg1) const;

	// Returns the number of HTNEntry
	int GetNumEntries() const;

private:
	std::vector<HTNEntry> mEntries;
};

template <int NumArgs>
template <typename... Args>
inline void HTNTable<NumArgs>::AddEntry(Args&&... inArgs)
{
	static constexpr int ArgsSize = sizeof...(Args);
	static_assert(ArgsSize == NumArgs);
	
	HTNEntry& Entry = mEntries.emplace_back();
	Entry = { std::forward<Args>(inArgs)... };
}

template <int NumArgs>
inline int HTNTable<NumArgs>::Query(int inIndex, HTNAtom& ioArg1) const
{
	if (inIndex < 0 || inIndex >= mEntries.size())
	{
		return 0;
	}

	const HTNEntry& Entry = mEntries[inIndex];
	ioArg1 = Entry[0];

	return 1;
}

template <int NumArgs>
inline int HTNTable<NumArgs>::GetNumEntries() const
{
	return static_cast<int>(mEntries.size());
}

static constexpr int MaxNumArgs = 8;

// World state for the HTN planner. 
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
	typedef std::array<HTNTableBase*, MaxNumArgs> HTNFact;

	// Writes a fact with n arguments
	template <typename... Args>
	void MakeFact(const char* inKey, Args&&... inArgs);

	// It checks if there is an existing entry for the inKey + arguments.
	// If all the arguments are binded then the result is like a binary operation, that query results in true or false.
	// If not all the arguments are binded then the result might return more than 1.
	// Return the number of possible results in the database.
	int Query(const char* inKey, HTNAtom& ioArg1);

	// It checks if there is an existing entry for the inKey + arguments.
	// If all the arguments are binded then the result is like a binary operation, that query results in true or false.
	// If not all the arguments are binded then we need to bind it according to the row that correspond to inIndex, the result will always be 0 or 1.
	// Return either 0 or 1 because we are pointing to a specific row withing a table.
	int QueryIndex(int inIndex, const char* inKey, HTNAtom& ioArg1);

	// Returns the number of HTNTables registered for the fact inKey.
	int GetNumFactTables(const char* inKey) const;

	// Returns the number of HTNTables registered for the fact inKey and the inNumArgs. This function should only return 0 or 1.
	// If it does returns something different then this will cause problems later on during decomposition.
	int GetNumFactTablesByNumArgs(const char* inKey, const int inNumArgs) const;

private:
	std::unordered_map<std::string, HTNFact> mFacts;
};

template <typename... Args>
inline void HTNWorldState::MakeFact(const char* inKey, Args&&... inArgs)
{
	static constexpr int ArgsSize = sizeof...(Args);
	static_assert(ArgsSize <= MaxNumArgs);

	HTNFact& Fact = mFacts[inKey];
	if constexpr (ArgsSize == 0)
	{
		// Skip creating a table if there are no args
		return;
	}

	static constexpr int TableIndex = ArgsSize - 1;
	HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[TableIndex]);
	if (!Table)
	{
		Fact[TableIndex] = Table = new HTNTable<ArgsSize>();
	}

	Table->AddEntry(std::forward<Args>(inArgs)...);
}

inline int HTNWorldState::Query(const char* inKey, HTNAtom& ioArg1)
{
	static constexpr int ArgsSize = 1;

	if (ioArg1.IsSet())
	{
		// All arguments are bound
		return 1;
	}

	const auto FactIt = mFacts.find(inKey);
	if (FactIt == mFacts.end())
	{
		// Fact not found
		return 0;
	}

	const HTNFact& Fact = FactIt->second;
	static constexpr int TableIndex = ArgsSize - 1;
	const HTNTable<1>* Table = static_cast<HTNTable<1>*>(Fact[TableIndex]);
	if (!Table)
	{
		// Table not found
		return 0;
	}

	return Table->GetNumEntries();
}

inline int HTNWorldState::QueryIndex(int inIndex, const char* inKey, HTNAtom& ioArg1)
{
	static constexpr int ArgsSize = 1;

	if (ioArg1.IsSet())
	{
		// All arguments are bound
		return 1;
	}

	const auto FactIt = mFacts.find(inKey);
	if (FactIt == mFacts.end())
	{
		// Fact not found
		return 0;
	}

	const HTNFact& Fact = FactIt->second;
	static constexpr int TableIndex = ArgsSize - 1;
	const HTNTable<1>* Table = static_cast<HTNTable<1>*>(Fact[TableIndex]);
	if (!Table)
	{
		// Table not found
		return 0;
	}

	return Table->Query(inIndex, ioArg1);
}

inline int HTNWorldState::GetNumFactTables(const char* inKey) const
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

inline int HTNWorldState::GetNumFactTablesByNumArgs(const char* inKey, const int inNumArgs) const
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
