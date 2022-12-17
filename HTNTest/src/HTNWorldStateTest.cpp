#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNWorldStateTest) 
{
	HTNWorldState world_state;

	const char* sFavoriteColor("favorite_color");
	EXPECT_EQ(world_state.GetNumFactTables(sFavoriteColor), -1);

	const char* sThreat("threat");
	world_state.MakeFact(sThreat);
	EXPECT_EQ(world_state.GetNumFactTables(sThreat), 0);

	const char* sDay("day");
	world_state.MakeFact(sDay, "Monday");
	EXPECT_EQ(world_state.GetNumFactTables(sDay), 1);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sDay, 1), 1);

	const char* sWeather("weather");
	world_state.MakeFact(sWeather, "Sunny");
	world_state.MakeFact(sWeather, "Cloudy", 10);
	EXPECT_EQ(world_state.GetNumFactTables(sWeather), 2);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sWeather, 1), 1);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sWeather, 2), 1);
}
