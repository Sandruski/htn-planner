#include "HTNPlannerHook.h"

#include "Log.h"

bool HTNPlannerHook::parseDomain(const std::string& inDomainPath)
{
	return mInterpreter.Init(inDomainPath);
}

std::vector<std::shared_ptr<const HTNTask>> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const
{
	const std::vector<std::shared_ptr<const HTNTask>> Plan = mInterpreter.Interpret(inEntryPointName, inWorldState);
	if (Plan.empty())
	{
		LOG_ERROR("Interpret failed");
		return {};
	}

	return Plan;
}
