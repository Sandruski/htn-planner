// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDomainHelpers.h"
#include "Core/HTNFileHelpers.h"
#include "HTNCoreMinimal.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"

#include "optick.h"
#include "gtest/gtest.h"
#include "gtest/gtest-param-test.h"

#include <execution>
#include <filesystem>
#include <format>
#include <string>
#include <tuple>
#include <vector>

class HTNDecompositionTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>>
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    void SetUp() override;

    const std::string& GetWorldStateFileName() const;
    const std::string& GetDomainFileName() const;
    const std::string& GetEntryPointID() const;

    HTNDatabaseHook mDatabaseHook;
    HTNPlannerHook  mPlannerHook;
};

void HTNDecompositionTest::SetUpTestCase()
{
    OPTICK_START_CAPTURE();
}

void HTNDecompositionTest::TearDownTestCase()
{
    OPTICK_STOP_CAPTURE();
    const std::string CaptureFileRelativePath = std::format("{}/{}", HTNFileHelpers::kCapturesDirectoryName, HTNFileHelpers::kCaptureFileName);
    const std::string CaptureFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(CaptureFileRelativePath).string();
    OPTICK_SAVE_CAPTURE(CaptureFileAbsolutePath.c_str());
}

void HTNDecompositionTest::SetUp()
{
    const std::string WorldStatesTestDirectoryName =
        std::format("{}/{}", HTNFileHelpers::kWorldStatesDirectoryName, HTNFileHelpers::kTestDirectoryName);
    const std::string WorldStateFileName = GetWorldStateFileName();
    const std::string WorldStateFileRelativePath =
        HTNFileHelpers::MakeFilePath(WorldStatesTestDirectoryName, WorldStateFileName, HTNFileHelpers::kWorldStateFileExtension);
    const std::string WorldStateFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(WorldStateFileRelativePath).string();
    const bool        ParseWorldStateFileResult  = mDatabaseHook.ParseWorldStateFile(WorldStateFileAbsolutePath);
    ASSERT_TRUE(ParseWorldStateFileResult);

    const std::string DomainsTestDirectoryName = std::format("{}/{}", HTNFileHelpers::kDomainsDirectoryName, HTNFileHelpers::kTestDirectoryName);
    const std::string DomainFileName           = GetDomainFileName();
    const std::string DomainFileRelativePath =
        HTNFileHelpers::MakeFilePath(DomainsTestDirectoryName, DomainFileName, HTNFileHelpers::kDomainFileExtension);
    const std::string DomainFileAbsolutePath = HTNFileHelpers::MakeAbsolutePath(DomainFileRelativePath).string();
    const bool        ParseDomainFileResult  = mPlannerHook.ParseDomainFile(DomainFileAbsolutePath);
    ASSERT_TRUE(ParseDomainFileResult);
}

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
    HTNPlanningUnit   PlanningUnit                = HTNPlanningUnit("", mDatabaseHook, mPlannerHook);
    const std::string EntryPointID                = GetEntryPointID();
    const bool        ExecuteTopLevelMethodResult = PlanningUnit.ExecuteTopLevelMethod(EntryPointID);
    EXPECT_TRUE(ExecuteTopLevelMethodResult);
}

TEST_P(HTNDecompositionTest, AreDecompositionsSuccessful)
{
    std::vector<HTNPlanningUnit> PlanningUnits;
    static constexpr size        PlanningUnitsSize = 100;
    PlanningUnits.resize(PlanningUnitsSize, HTNPlanningUnit("", mDatabaseHook, mPlannerHook));
    const std::string EntryPointID = GetEntryPointID();
    std::for_each(std::execution::par, PlanningUnits.begin(), PlanningUnits.end(), [&](HTNPlanningUnit& inPlanningUnit) {
        const bool ExecuteTopLevelMethodResult = inPlanningUnit.ExecuteTopLevelMethod(EntryPointID);
        EXPECT_TRUE(ExecuteTopLevelMethodResult);
    });
}

INSTANTIATE_TEST_CASE_P(Human, HTNDecompositionTest,
                        testing::Values(std::make_tuple("human", "human", HTNDomainHelpers::kDefaultMainTopLevelMethodID)));
