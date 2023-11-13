#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Helpers/HTNFileHelpers.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"

#include <filesystem>
#include <tuple>

namespace
{
std::string MakeFilePath(const std::string& inDirectoryName, const std::string& inFileName, const std::string& inFileExtension);

const std::string TestDirectoryName = "Test";
} // namespace

class HTNDecompositionTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>>
{
protected:
    const std::string& GetWorldStateFileName() const;
    const std::string& GetDomainFileName() const;
    const std::string& GetEntryPointID() const;
};

namespace
{
std::string MakeFilePath(const std::string& inDirectoryName, const std::string& inFileName, const std::string& inFileExtension)
{
    return std::format("{}/{}/{}{}", inDirectoryName, TestDirectoryName, inFileName, inFileExtension);
}
} // namespace

const std::string& HTNDecompositionTest::GetWorldStateFileName() const
{
    return std::get<0>(GetParam());
}

const std::string& HTNDecompositionTest::GetDomainFileName() const
{
    return std::get<1>(GetParam());
}

const std::string& HTNDecompositionTest::GetEntryPointID() const
{
    return std::get<2>(GetParam());
}

TEST_P(HTNDecompositionTest, IsDecompositionSuccessful)
{
    HTNDatabaseHook   DatabaseHook;
    const std::string WorldStateFileName = GetWorldStateFileName();
    const std::string WorldStateFileRelativePath =
        MakeFilePath(HTNFileHelpers::WorldStatesDirectoryName, WorldStateFileName, HTNFileHelpers::WorldStateFileExtension);
    const std::string WorldStateFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(WorldStateFileRelativePath).string();
    const bool        ParseWorldStateFileResult  = DatabaseHook.ParseWorldStateFile(WorldStateFileAbsolutePath);
    ASSERT_TRUE(ParseWorldStateFileResult);
    HTNPlannerHook    PlannerHook;
    const std::string DomainFileName = GetDomainFileName();
    const std::string DomainFileRelativePath =
        MakeFilePath(HTNFileHelpers::DomainsDirectoryName, DomainFileName, HTNFileHelpers::DomainFileExtension);
    const std::string DomainFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(DomainFileRelativePath).string();
    const bool        ParseDomainFileResult  = PlannerHook.ParseDomainFile(DomainFileAbsolutePath);
    ASSERT_TRUE(ParseDomainFileResult);
    HTNPlanningUnit   PlanningUnit                = HTNPlanningUnit("", DatabaseHook, PlannerHook);
    const std::string EntryPointID                = GetEntryPointID();
    const bool        ExecuteTopLevelMethodResult = PlanningUnit.ExecuteTopLevelMethod(EntryPointID);
    EXPECT_TRUE(ExecuteTopLevelMethodResult);
}

INSTANTIATE_TEST_CASE_P(Human, HTNDecompositionTest,
                        testing::Values(std::make_tuple("human", "human", HTNDecompositionHelpers::kDefaultMainTopLevelMethodID),
                                        std::make_tuple("human_axioms", "human_axioms", HTNDecompositionHelpers::kDefaultMainTopLevelMethodID)));
