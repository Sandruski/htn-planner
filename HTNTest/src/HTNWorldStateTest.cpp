#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNWorldStateTest) 
{
	HTNWorldState world_state;

	HTNAtom sThreat("threat");
	HTNAtom sDay("day");
	HTNAtom sFavoriteColor("favorite_color");

	world_state.MakeFact(sThreat);
	world_state.MakeFact(sDay, "Monday");

	EXPECT_EQ(world_state.GetNumFactTables(sDay), 1);

	EXPECT_EQ(world_state.GetNumFactTables(sFavoriteColor), 0);
}
