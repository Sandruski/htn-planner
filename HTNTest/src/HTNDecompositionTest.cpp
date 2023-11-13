#include "Helpers/HTNFileHelpers.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"

#include <filesystem>
#include <string>

TEST(HTNDecompositionTest, IsExampleDecompositionSuccessful)
{
    HTNDatabaseHook   DatabaseHook;
    const std::string WorldStateFileRelativePath = "WorldStates/human.worldstate";
    const std::string WorldStateFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(WorldStateFileRelativePath);
    const bool        ParseWorldStateFileResult  = DatabaseHook.ParseWorldStateFile(WorldStateFileAbsolutePath);
    ASSERT_TRUE(ParseWorldStateFileResult);
    HTNPlannerHook    PlannerHook;
    const std::string DomainFileRelativePath = "Domains/human.domain";
    const std::string DomainFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(DomainFileRelativePath);
    const bool        ParseDomainFileResult  = PlannerHook.ParseDomainFile(DomainFileAbsolutePath);
    ASSERT_TRUE(ParseDomainFileResult);
    HTNPlanningUnit   PlanningUnit                = HTNPlanningUnit("", DatabaseHook, PlannerHook);
    const std::string EntryPointID                = "behave";
    const bool        ExecuteTopLevelMethodResult = PlanningUnit.ExecuteTopLevelMethod(EntryPointID);
    EXPECT_TRUE(ExecuteTopLevelMethodResult);
}
