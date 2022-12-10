#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNWorldStateTest) 
{
	HTNWorldState world_state;

	const char* sThreat("threat");
	const char* sDay("day");
	const char* sFavoriteColor("favorite_color");

	world_state.MakeFact(sThreat);
	world_state.MakeFact(sDay, "Monday");
	world_state.MakeFact(sDay, "Monday", 10);

	EXPECT_EQ(world_state.GetNumFactTables(sThreat), 0);

	EXPECT_EQ(world_state.GetNumFactTables(sDay), 2);
	EXPECT_EQ(world_state.GetNumFactTablesByArgNumber(sDay, 1), 1);
	EXPECT_EQ(world_state.GetNumFactTablesByArgNumber(sDay, 2), 1);

	EXPECT_EQ(world_state.GetNumFactTables(sFavoriteColor), 0);
}
