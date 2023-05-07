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
	// Adds an entry with arguments
	template <typename... Args>
	void AddEntry(const Args&... inArgs);

	// Binds the arguments of the entry at the specified index to the unbound arguments passed
	template <typename... Args>
	int Query(const int inIndex, Args&... inArgs) const;

	template <typename... Args>
	bool Check(const int inIndex, Args&... inArgs) const;

	// Returns the number of entries
	int GetNumEntries() const;

private:
	typedef std::array<HTNAtom, NumArgs> HTNEntry;

	// Helper method for fold expression
	template <typename T>
	static void BindArgsUnbound(const HTNEntry& inEntry, T& inArg, int& ioIndex);

	// Helper method for fold expression
	template <typename T>
	static bool CheckArgsBound(const HTNEntry& inEntry, const T& inArg, int& ioIndex);

	std::vector<HTNEntry> mEntries;
};

template <int NumArgs>
template <typename... Args>
inline void HTNTable<NumArgs>::AddEntry(const Args&... inArgs)
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize == NumArgs);
	
	HTNEntry& Entry = mEntries.emplace_back();
	Entry = { inArgs... };
}

template <int NumArgs>
template <typename... Args>
inline int HTNTable<NumArgs>::Query(const int inIndex, Args&... inArgs) const
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize == NumArgs);

	if (inIndex < 0 || inIndex >= mEntries.size())
	{
		return 0;
	}

	const HTNEntry& Entry = mEntries[inIndex];
	int Index = 0;
	(BindArgsUnbound(Entry, inArgs, Index), ...);

	return 1;
}

template <int NumArgs>
template <typename... Args>
inline bool HTNTable<NumArgs>::Check(const int inIndex, Args&... inArgs) const
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize == NumArgs);

	if (inIndex < 0 || inIndex >= mEntries.size())
	{
		return false;
	}

	const HTNEntry& Entry = mEntries[inIndex];
	int Index = 0;
	const bool Result = (CheckArgsBound(Entry, inArgs, Index) && ...);
	if (!Result)
	{
		return false;
	}

	Index = 0;
	(BindArgsUnbound(Entry, inArgs, Index), ...);

	return true;
}

template <int NumArgs>
inline int HTNTable<NumArgs>::GetNumEntries() const
{
	return static_cast<int>(mEntries.size());
}

template <int NumArgs>
template <typename T>
inline void HTNTable<NumArgs>::BindArgsUnbound(const HTNEntry& inEntry, T& inArg, int& ioIndex)
{
	if (!inArg.IsSet())
	{
		inArg = inEntry[ioIndex];
	}

	++ioIndex;
}

template <int NumArgs>
template <typename T>
inline bool HTNTable<NumArgs>::CheckArgsBound(const HTNEntry& inEntry, const T& inArg, int& ioIndex)
{
	bool Result = true;

	if (inArg.IsSet())
	{
		Result = (inEntry[ioIndex] == inArg);
	}

	++ioIndex;

	return Result;
}

static constexpr int MaxNumArgs = 8;

// World state for the HTN planner. 
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
	// Writes a fact with n arguments
	template <typename... Args>
	void MakeFact(const char* inKey, const Args&... inArgs);

	// It checks if there is an existing entry for the inKey + arguments.
	// If all the arguments are binded then the result is like a binary operation, that query results in true or false.
	// If not all the arguments are binded then the result might return more than 1.
	// Return the number of possible results in the database.
	template <typename... Args>
	int Query(const char* inKey, Args&... inArgs) const;

	// It checks if there is an existing entry for the inKey + arguments.
	// If all the arguments are binded then the result is like a binary operation, that query results in true or false.
	// If not all the arguments are binded then we need to bind it according to the row that correspond to inIndex, the result will always be 0 or 1.
	// Return either 0 or 1 because we are pointing to a specific row within a table.
	template <typename... Args>
	int QueryIndex(const char* inKey, const int inIndex, Args&... inArgs) const;

	template <typename... Args>
	bool CheckIndex(const char* inKey, const int inIndex, Args&... inArgs) const;

	// Returns the number of HTNTables registered for the fact inKey.
	int GetNumFactTables(const char* inKey) const;

	// Returns the number of HTNTables registered for the fact inKey and the inNumArgs. This function should only return 0 or 1.
	// If it does returns something different then this will cause problems later on during decomposition.
	int GetNumFactTablesByNumArgs(const char* inKey, const int inNumArgs) const;

private:
	// Helper method for fold expression
	template <typename T>
	static void CountArgsBound(const T& inArg, int& outNumArgsBound);

	typedef std::array<HTNTableBase*, MaxNumArgs> HTNFact;
	std::unordered_map<std::string, HTNFact> mFacts;
};

template <typename... Args>
inline void HTNWorldState::MakeFact(const char* inKey, const Args&... inArgs)
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize <= MaxNumArgs);

	HTNFact& Fact = mFacts[inKey];

	// Skip creating a table if there are no args
	if constexpr (ArgsSize > 0)
	{
		static constexpr int TableIndex = ArgsSize - 1;
		HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[TableIndex]);
		if (!Table)
		{
			Fact[TableIndex] = Table = new HTNTable<ArgsSize>();
		}

		Table->AddEntry(inArgs...);
	}
}

template <typename... Args>
inline int HTNWorldState::Query(const char* inKey, Args&... inArgs) const
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize <= MaxNumArgs);

	int NumArgsBound = 0;
	(CountArgsBound(inArgs, NumArgsBound), ...);
	if (ArgsSize == NumArgsBound)
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

	if constexpr (ArgsSize > 0)
	{
		const HTNFact& Fact = FactIt->second;
		static constexpr int TableIndex = ArgsSize - 1;
		const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[TableIndex]);
		if (!Table)
		{
			// Table not found
			return 0;
		}

		return Table->GetNumEntries();
	}

	return 1;
}

template <typename... Args>
inline int HTNWorldState::QueryIndex(const char* inKey, const int inIndex, Args&... inArgs) const
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize <= MaxNumArgs);

	int NumArgsBound = 0;
	(CountArgsBound(inArgs, NumArgsBound), ...);
	if (ArgsSize == NumArgsBound)
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

	if constexpr (ArgsSize > 0)
	{
		const HTNFact& Fact = FactIt->second;
		static constexpr int TableIndex = ArgsSize - 1;
		const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[TableIndex]);
		if (!Table)
		{
			// Table not found
			return 0;
		}

		return Table->Query(inIndex, inArgs...);
	}

	return 1;
}

template <typename... Args>
inline bool HTNWorldState::CheckIndex(const char* inKey, const int inIndex, Args&... inArgs) const
{
	static constexpr std::size_t ArgsSize = sizeof...(Args);
	static_assert(ArgsSize <= MaxNumArgs);

	const auto FactIt = mFacts.find(inKey);
	if (FactIt == mFacts.end())
	{
		// Fact not found
		return false;
	}

	if constexpr (ArgsSize > 0)
	{
		const HTNFact& Fact = FactIt->second;
		static constexpr int TableIndex = ArgsSize - 1;
		const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[TableIndex]);
		if (!Table)
		{
			// Table not found
			return false;
		}

		return Table->Check(inIndex, inArgs...);
	}

	return true;
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

template <typename T>
inline void HTNWorldState::CountArgsBound(const T& inArg, int& outNumArgsBound)
{
	if (inArg.IsSet())
	{
		++outNumArgsBound;
	}
}
