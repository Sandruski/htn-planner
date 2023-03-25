#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"
#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNConditionWorldStateQuery.h"
#include "Runtime/HTNPlannerHook.h"

// Test constants groups parsing
TEST(HTNHierarchicalPlanner, HTNDomainConstantsParsing)
{
	//const char* domain_path = "../Domains/horse_domain_test2.domain";
	const char* domain_path = "../Domains/calculator.domain";
	
	HTNPlannerHook htn_planner;
	htn_planner.parseDomain(domain_path);

	// TODO salvarez
	/*
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