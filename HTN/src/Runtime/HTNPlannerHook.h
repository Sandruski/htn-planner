#pragma once

#include "Interpreter/HTNInterpreter.h"

#include <memory>
#include <string>
#include <vector>

class HTNAtom;
class HTNTask;
class HTNWorldState;

// Runtime instance of an HTN Planner.
// 
class HTNPlannerHook
{
public:
	// Parse a domain file and extract the information needed to create the hierarchical structure.
	// The information needed would be: ConstantsGroups, Methods, Axioms.
	// Once we got all this information we need to know before hand what is the top_level method. 
	// The top level method determines the root/parent node of the entire hierarchy.
	bool parseDomain(const std::string& inDomainPath);

	// Returns an HTNAtom with the information about the constant requested. 
	// Returns an unbinded atom if the constant wasn't found.
	const HTNAtom* GetConstantByName(const char* inConstantGroup, const char* inConstantId)
	{
		// TODO SANDRA.
		(void*)inConstantGroup;
		(void*)inConstantId;
		return nullptr;
	}

	std::vector<std::shared_ptr<const HTNTask>> MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const;

private:
	HTNInterpreter mInterpreter;
};
