#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNArgumentBinding)
{
	HTNWorldState world_state;

	const char* sFood("food");

	world_state.MakeFact(sFood, "fish");
	world_state.MakeFact(sFood, "beef");
	world_state.MakeFact(sFood, "meat");
	world_state.MakeFact(sFood, "fruit");

	HTNAtom atom_fish("fish");
	EXPECT_EQ(world_state.Query(sFood, atom_fish), 1);

	HTNAtom atom_unbinded;
	EXPECT_EQ(world_state.Query(sFood, atom_unbinded), 4);

	// Query Row specific 0
	{
		HTNAtom atom_from_unbinded_to_fish;
		EXPECT_FALSE(atom_from_unbinded_to_fish.IsSet());
		EXPECT_EQ(world_state.QueryIndex(sFood, 0, atom_from_unbinded_to_fish), 1);
		EXPECT_TRUE(atom_from_unbinded_to_fish.IsSet());
		EXPECT_TRUE(atom_from_unbinded_to_fish.IsType<const char*>());
		EXPECT_EQ(strcmp(atom_from_unbinded_to_fish.GetValue<const char*>(), "fish"), 0);
	}

	// Query Row specific 1
	{
		HTNAtom atom_from_unbinded_to_beef;
		EXPECT_FALSE(atom_from_unbinded_to_beef.IsSet());
		EXPECT_EQ(world_state.QueryIndex(sFood, 1, atom_from_unbinded_to_beef), 1);
		EXPECT_TRUE(atom_from_unbinded_to_beef.IsSet());
		EXPECT_TRUE(atom_from_unbinded_to_beef.IsType<const char*>());
		EXPECT_EQ(strcmp(atom_from_unbinded_to_beef.GetValue<const char*>(), "beef"), 0);
	}

	// Query Row specific 2
	{
		HTNAtom atom_from_unbinded_to_meat;
		EXPECT_FALSE(atom_from_unbinded_to_meat.IsSet());
		EXPECT_EQ(world_state.QueryIndex(sFood, 2, atom_from_unbinded_to_meat), 1);
		EXPECT_TRUE(atom_from_unbinded_to_meat.IsSet());
		EXPECT_TRUE(atom_from_unbinded_to_meat.IsType<const char*>());
		EXPECT_EQ(strcmp(atom_from_unbinded_to_meat.GetValue<const char*>(), "meat"), 0);
	}

	// Query Row specific 3
	{
		HTNAtom atom_from_unbinded_to_fruit;
		EXPECT_FALSE(atom_from_unbinded_to_fruit.IsSet());
		EXPECT_EQ(world_state.QueryIndex(sFood, 3, atom_from_unbinded_to_fruit), 1);
		EXPECT_TRUE(atom_from_unbinded_to_fruit.IsSet());
		EXPECT_TRUE(atom_from_unbinded_to_fruit.IsType<const char*>());
		EXPECT_EQ(strcmp(atom_from_unbinded_to_fruit.GetValue<const char*>(), "fruit"), 0);
	}

	// Query Row specific 4 (doesn't exist)
	{
		HTNAtom atom_from_unbinded_to_nothing;
		EXPECT_FALSE(atom_from_unbinded_to_nothing.IsSet());
		EXPECT_EQ(world_state.QueryIndex(sFood, 4, atom_from_unbinded_to_nothing), 0);
		EXPECT_FALSE(atom_from_unbinded_to_nothing.IsSet());
	}
}
