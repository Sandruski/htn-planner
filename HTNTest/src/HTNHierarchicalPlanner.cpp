#include "Log.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNTask.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"
#include "Runtime/HTNPlannerHook.h"

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
	HTNWorldState WorldState;
	const char* kShouldDecompose = "should_decompose";
	WorldState.MakeFact(kShouldDecompose);
	const std::vector<std::shared_ptr<const HTNTask>> Plan = Planner.MakePlan(DomainPath, WorldState);

	// Print plan
	for (const std::shared_ptr<const HTNTask>& Task : Plan)
	{
		LOG("{}", Task ? Task->ToString().c_str() : "Invalid Task");
	}

	// TODO salvarez HTNPlannerRunner // Action execution
}
