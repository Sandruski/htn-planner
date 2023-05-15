#include "Log.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNTask.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"
#include "Runtime/HTNPlannerHook.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <execution>
#include <iterator>
#include <vector>
#include <string>

// Test constants groups parsing
TEST(HTNHierarchicalPlanner, HTNDomainConstantsParsing)
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
TEST(HTNHierarchicalPlanner, HTNDomainMethodsParsing)
{
}

// Test planning
TEST(HTNHierarchicalPlanner, HTNPlanning)
{
	HTNPlannerHook Planner; // Decision making
	const std::string DomainPath = "../Domains/simple_domain.domain";
	if (!Planner.parseDomain(DomainPath))
	{
		return;
	}

	const std::string EntryPointName = "entry_point";

	HTNWorldState WorldState;
	const char* kShouldDecompose = "should_decompose";
	WorldState.MakeFact(kShouldDecompose, 2);
	
	// TODO Sandra: MakePlan should receive as parameter the name of the entry point function. This way we will prepare the planner for the future to support delayed decomposition of other methods.
	// The name of the entry point function should be part of the HtnPlanningUnit instance. There might be more than 1 planning units running at the same time in sequence:
	// ex: main_body_planning_unit and upper_body_planning_unit. The entry point function MUST be different
	// ex: main_body_planning_unit	-> entry point function: "behave"
	// ex: upper_body_planning_unit -> entry point function: "behave_upper_body"
	const std::vector<std::shared_ptr<const HTNTask>> Plan = Planner.MakePlan(EntryPointName, WorldState);
	for (const std::shared_ptr<const HTNTask>& Task : Plan)
	{
		LOG("{}", Task ? Task->ToString().c_str() : "Invalid Task");
	}

	// TODO salvarez HTNPlannerRunner // Action execution
}


// Planning unit structure that holds the planner hook and the database
struct PlanningUnit
{
public:
	// Execute planning unit top level method
	void				ExecuteTopLevelMethod(size_t inPlanningUnitIndex)
	{
		WorldState.MakeFact("iteration_number", (int)inPlanningUnitIndex);
		const std::string DomainPath = "../Domains/multithreading_domain.domain";
		const std::vector<std::shared_ptr<const HTNTask>> Plan = Planner.MakePlan(DomainPath, WorldState);		
		
		// TODO SANDRA: We need to gather the primitive task resulted from the decomposition.
		// The primitive task should be (!log inPlanningUnitIndex), and we need to add
		// the corresponding EXPECT_EQ(PrimitiveTaskArgument->asInt(), inPlanningUnitIndex) to pass the test.

		/*
		// Print thread id + plan
		std::cout << "Thread: " << std::this_thread::get_id() << std::endl;
		for (const std::shared_ptr<const HTNTask>& Task : Plan)
		{
			LOG("{}", Task ? Task->ToString().c_str() : "Invalid Task");
		}*/
	}
private:
	HTNPlannerHook Planner;		// Decision making
	HTNWorldState WorldState;	// World state database
};

// called per thread to execute the planner top level method
static void sParallelWork(PlanningUnit& inPlanningUnit, size_t inIndex)
{
	inPlanningUnit.ExecuteTopLevelMethod(inIndex);
}


TEST(HTNHierarchicalPlanner, MultiThreadingHTNPlanning)
{
	std::vector<PlanningUnit> PlanningUnits;
	PlanningUnits.resize(1000);				/// 10 planning units executed in parallel.

	// parallel for
	std::for_each(std::execution::par, PlanningUnits.begin(), PlanningUnits.end(),
		[&PlanningUnits](PlanningUnit& inPlanningUnit)
		{
			size_t idx = &inPlanningUnit - &PlanningUnits[0];
			sParallelWork(inPlanningUnit, idx);
		});
	// TODO salvarez HTNPlannerRunner // Action execution
}

