#include "Log.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"
#include "Runtime/HTNPlannerHook.h"
#include "Runtime/HTNDecompositionContext.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <execution>
#include <iterator>
#include <vector>
#include <sstream>
#include <string>

// Test constants groups parsing
TEST(HTNHierarchicalPlanner, DISABLED_HTNDomainConstantsParsing)
{	
	// TODO salvarez
	/*
	const char* domain_path = "../Domains/simple_domain.domain";
	
	HTNPlannerHook htn_planner;
	htn_planner.parseDomain(domain_path);

	const char* constant_group = "Horse_Constants";
	const char* constant_idle_wait_time_min = "idle_wait_time_min";
	const char* constant_idle_wait_time_max = "idle_wait_time_max";
	const HTNAtom* constant_0 = htn_planner.GetConstantByName(constant_group, constant_idle_wait_time_min);
	const HTNAtom* constant_1 = htn_planner.GetConstantByName(constant_group, constant_idle_wait_time_max);

	EXPECT_TRUE(constant_0->IsSet());
	EXPECT_TRUE(constant_1->IsSet());
	EXPECT_EQ(constant_0->GetValue<float>(), 5.0f);
	EXPECT_EQ(constant_1->GetValue<float>(), 10.0f);
	*/
}

// Test methods parsing
TEST(HTNHierarchicalPlanner, DISABLED_HTNDomainMethodsParsing)
{
}

// TODO salvarez Make the actual HTNPlannerHook return this
class HTNTaskInstance
{
public:
	explicit HTNTaskInstance(const std::string& inName, const std::vector<HTNAtom>& inArguments)
		: mName(inName), mArguments(inArguments)
	{
	}

	const std::string& GetName() const
	{
		return mName;
	}

	const std::vector<HTNAtom>& GetArguments() const
	{
		return mArguments;
	}

private:
	std::string mName;
	std::vector<HTNAtom> mArguments;
};

// Planning unit structure that holds the planner hook and the database
struct HTNPlanningUnit
{
public:
	bool ParseDomain(const std::string& inDomainPath)
	{
		return mPlanner.parseDomain(inDomainPath);
	}

	// Execute planning unit top level method
	std::vector<HTNTaskInstance> ExecuteTopLevelMethod(const std::string& inEntryPointName, size_t inPlanningUnitIndex)
	{
		mLastPlan.clear();

		mWorldState.MakeFact("iteration_number", (int)inPlanningUnitIndex);

		HTNDecompositionContext DecompositionContext = HTNDecompositionContext(mWorldState);
		const std::vector<std::shared_ptr<const HTNTask>> Plan = mPlanner.MakePlan(inEntryPointName, DecompositionContext);
		for (const std::shared_ptr<const HTNTask>& Task : Plan)
		{
			const std::string& Name = Task->GetName();

			std::vector<HTNAtom> InstanceArguments;
			const std::vector<std::shared_ptr<const HTNValue>>& Arguments = Task->GetArguments();
			for (const std::shared_ptr<const HTNValue>& Argument : Arguments)
			{
				const HTNAtom& ArgumentValue = Argument->GetValue();
				const std::string& VariableName = ArgumentValue.GetValue<std::string>();
				const HTNAtom* Variable = DecompositionContext.FindVariable(VariableName);
				if (!Variable)
				{
					continue;
				}

				InstanceArguments.emplace_back(*Variable);
			}

			mLastPlan.emplace_back(Name, InstanceArguments);
		}

		return mLastPlan;
	}

	const std::vector<HTNTaskInstance>& GetLastPlan() const
	{
		return mLastPlan;
	}

private:
	HTNPlannerHook mPlanner;		// Decision making
	HTNWorldState mWorldState;		// World state database
	std::vector<HTNTaskInstance> mLastPlan;
};

// Test planning
TEST(HTNHierarchicalPlanner, DISABLED_HTNPlanning)
{
	const std::string DomainPath = "../Domains/simple_domain.domain";
	const std::string EntryPointName = "entry_point";

	HTNPlanningUnit PlanningUnit;
	EXPECT_TRUE(PlanningUnit.ParseDomain(DomainPath));
	PlanningUnit.ExecuteTopLevelMethod(EntryPointName, 0);

	// TODO salvarez HTNPlannerRunner // Action execution
}

// called per thread to execute the planner top level method
static void sParallelWork(HTNPlanningUnit& inPlanningUnit, const std::string& EntryPointName, size_t inIndex)
{
	const std::vector<HTNTaskInstance>& Plan = inPlanningUnit.ExecuteTopLevelMethod(EntryPointName, inIndex);
	EXPECT_TRUE(Plan.size() == 1);

	for (const HTNTaskInstance& Task : Plan)
	{
		const std::string& Name = Task.GetName();
		EXPECT_TRUE(Name == "log");

		const std::vector<HTNAtom>& Arguments = Task.GetArguments();
		EXPECT_TRUE(Arguments.size() == 1);

		const HTNAtom& Argument = Arguments[0];
		EXPECT_TRUE(Argument.IsSet());
		EXPECT_TRUE(Argument.IsType<int>());

		const int& IterationNumber = Argument.GetValue<int>();
		EXPECT_TRUE(static_cast<int>(inIndex) == IterationNumber);

		std::ostringstream Buffer;
		Buffer << std::this_thread::get_id();
		LOG("Thread: {} Name: {} Argument: {}", Buffer.str(), Name.c_str(), IterationNumber);
	}
}

TEST(HTNHierarchicalPlanner, MultiThreadingHTNPlanning)
{
	const std::string DomainPath = "../Domains/multithreading_domain.domain";
	const std::string EntryPointName = "entry_point";

	std::vector<HTNPlanningUnit> PlanningUnits;
	PlanningUnits.resize(1000);				/// 10 planning units executed in parallel.

	// Parse domain
	// TODO salvarez Parse the domain only once because it is the same for all the planning units 
	for (HTNPlanningUnit& PlanningUnit : PlanningUnits)
	{
		EXPECT_TRUE(PlanningUnit.ParseDomain(DomainPath));
	}

	// Make plan
	// parallel for
	std::for_each(std::execution::par, PlanningUnits.begin(), PlanningUnits.end(),
		[&PlanningUnits, &EntryPointName](HTNPlanningUnit& inPlanningUnit)
		{
			size_t idx = &inPlanningUnit - &PlanningUnits[0];
			sParallelWork(inPlanningUnit, EntryPointName, idx);
		});

	// TODO salvarez HTNPlannerRunner // Action execution
}
