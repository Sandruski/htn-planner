#pragma once

class HTNAtom;

// Contains the array with a list of arguments that corresponds to certain HtnWorldState key.
class HTNTable
{
public:
	// TODO
private:
	int			mNumberOfArguments = 0;
};

// World state for the HTN planner. 
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
	// Writes a fact with 0 arguments
	void		MakeFact(const HTNAtom& inKey) {}

	// Writes a fact with 1 argument
	template <typename taArg0>
	void		MakeFact(const HTNAtom& inKey, const taArg0& inArg0) {}

	// Returns the number of HTNTables registered for the fact inKey.
	int			GetNumFactTables(const HTNAtom& inKey) const { return 1; }

	// Returns the number of HTNTables registered for the fact inKey and the inNumArguments. This function should only return 0 or 1.
	// If it does returns something different then this will cause problems later on during decomposition.
	int			GetNumFactTablesByArgNumber(const HTNAtom& inKey, int inNumArguments) const { return 0; }
private:
	// TODO
};
