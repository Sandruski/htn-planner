#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNArgumentBinding)
{
	HTNWorldState world_state;

	const char* sFood("food");
	//const char* sFish("favorite_color");

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
		EXPECT_EQ(world_state.QueryRow(0, sFood, atom_from_unbinded_to_fish), 1);
		EXPECT_TRUE(atom_from_unbinded_to_fish.IsSet());
		EXPECT_EQ(atom_from_unbinded_to_fish.Type, HTNAtom::AtomType::EString);
		EXPECT_EQ(strcmp(atom_from_unbinded_to_fish.Data.StringValue, "fish"), 0);
	}


	// Query Row specific 1
	{
		HTNAtom atom_from_unbinded_to_beef;
		EXPECT_FALSE(atom_from_unbinded_to_beef.IsSet());
		EXPECT_EQ(world_state.QueryRow(1, sFood, atom_from_unbinded_to_beef), 1);
		EXPECT_TRUE(atom_from_unbinded_to_beef.IsSet());
		EXPECT_EQ(atom_from_unbinded_to_beef.Type, HTNAtom::AtomType::EString);
		EXPECT_EQ(strcmp(atom_from_unbinded_to_beef.Data.StringValue, "beef"), 0);
	}

	// Query Row specific 2
	{
		HTNAtom atom_from_unbinded_to_meat;
		EXPECT_FALSE(atom_from_unbinded_to_meat.IsSet());
		EXPECT_EQ(world_state.QueryRow(2, sFood, atom_from_unbinded_to_meat), 1);
		EXPECT_TRUE(atom_from_unbinded_to_meat.IsSet());
		EXPECT_EQ(atom_from_unbinded_to_meat.Type, HTNAtom::AtomType::EString);
		EXPECT_EQ(strcmp(atom_from_unbinded_to_meat.Data.StringValue, "meat"), 0);
	}

	// Query Row specific 3
	{
		HTNAtom atom_from_unbinded_to_fruit;
		EXPECT_FALSE(atom_from_unbinded_to_fruit.IsSet());
		EXPECT_EQ(world_state.QueryRow(3, sFood, atom_from_unbinded_to_fruit), 1);
		EXPECT_TRUE(atom_from_unbinded_to_fruit.IsSet());
		EXPECT_EQ(atom_from_unbinded_to_fruit.Type, HTNAtom::AtomType::EString);
		EXPECT_EQ(strcmp(atom_from_unbinded_to_fruit.Data.StringValue, "fruit"), 0);
	}

	// Query Row specific 4 (doesn't exist)
	{
		HTNAtom atom_from_unbinded_to_nothing;
		EXPECT_FALSE(atom_from_unbinded_to_nothing.IsSet());
		EXPECT_EQ(world_state.QueryRow(4, sFood, atom_from_unbinded_to_nothing), 0);
		EXPECT_FALSE(atom_from_unbinded_to_nothing.IsSet());
		EXPECT_EQ(atom_from_unbinded_to_nothing.Type, HTNAtom::AtomType::EInvalid);
	}

}
