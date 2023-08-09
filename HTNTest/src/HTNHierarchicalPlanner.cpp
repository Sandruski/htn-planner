#include "Log.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNPlanningUnit.h"
#include "Runtime/HTNWorldState.h"

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

// Test planning
TEST(HTNHierarchicalPlanner, HTNPlanning)
{
    const std::string DomainPath = "../Domains/example_domain3.domain";
    const std::string EntryPointName = "entry_point";

    HTNPlanningUnit PlanningUnit;
    EXPECT_TRUE(PlanningUnit.ParseDomain(DomainPath));

    HTNWorldState& WorldState = PlanningUnit.GetWorldState();
    WorldState.MakeFact("item", "wallet");
    WorldState.MakeFact("item", "apple");
    WorldState.MakeFact("item", "fruit");
    WorldState.MakeFact("item", "tea");
    WorldState.MakeFact("item", "wallet", "full");
    WorldState.MakeFact("loggable", "wallet");
    WorldState.MakeFact("loggable", "apple");
    WorldState.MakeFact("edible", "tea");

    const std::vector<HTNTaskInstance>& Plan = PlanningUnit.ExecuteTopLevelMethod(EntryPointName);
    for (const HTNTaskInstance& Task : Plan)
    {
        std::string Message = Task.GetName();
        const std::vector<HTNAtom>& Arguments = Task.GetArguments();
        for (const HTNAtom& Argument : Arguments)
        {
            Message += " ";
            Message += Argument.ToString().c_str();
        }

        LOG("{}", Message.c_str());
    }

    // TODO salvarez HTNPlannerRunner // Action execution
}

// called per thread to execute the planner top level method
static void sParallelWork(HTNPlanningUnit& inPlanningUnit, const std::string& EntryPointName, size_t inIndex)
{
    HTNWorldState& WorldState = inPlanningUnit.GetWorldState();
    WorldState.MakeFact("iteration_number", (int)inIndex);

    const std::vector<HTNTaskInstance>& Plan = inPlanningUnit.ExecuteTopLevelMethod(EntryPointName);
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

TEST(HTNHierarchicalPlanner, DISABLED_MultiThreadingHTNPlanning)
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
