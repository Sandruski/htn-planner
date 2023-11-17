#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"

TEST(HTNAtom, DISABLED_HTNWorldStateSimpleFacts)
{
	HTNWorldState world_state;

	const char* sFavoriteColor("favorite_color");
	EXPECT_EQ(world_state.GetNumFactTables(sFavoriteColor), -1);

	const char* sThreat("threat");
	world_state.AddFact(sThreat);
	EXPECT_EQ(world_state.GetNumFactTables(sThreat), 0);

	const char* sDay("day");
	world_state.AddFact(sDay, "Monday");
	EXPECT_EQ(world_state.GetNumFactTables(sDay), 1);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sDay, 1), 1);

	const char* sWeather("weather");
	world_state.AddFact(sWeather, "Sunny");
	world_state.AddFact(sWeather, "Cloudy", 10);
	EXPECT_EQ(world_state.GetNumFactTables(sWeather), 2);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sWeather, 1), 1);
	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sWeather, 2), 1);
}

// Test adding facts
TEST(HTNAtom, DISABLED_HTNWorldStateComplexFacts)
{
	HTNWorldState world_state;

	const char* sMenu("menu");
	EXPECT_EQ(world_state.GetNumFactTables(sMenu), -1);

	// First dish
	HTNAtom first_dish;
	first_dish.AddListElement(HTNAtom("paella"));
	first_dish.AddListElement(HTNAtom("migas"));
	first_dish.AddListElement(HTNAtom("salmorejo"));

	// Second dish	
	HTNAtom second_dish;
	second_dish.AddListElement(HTNAtom("carne"));
	second_dish.AddListElement(HTNAtom("pescado"));
	second_dish.AddListElement(HTNAtom("TortillaDePatatas")); // Careful! We need to convert all the string to snake_case format! so TortillaDePatatas needs to be "tortilla_de_patatas".
				
	HTNAtom dessert;
	dessert.AddListElement(HTNAtom("yogurt"));
	dessert.AddListElement(HTNAtom("flan"));
	dessert.AddListElement(HTNAtom("CremaCatalana")); // Careful! We need to convert all the string to snake_case format! so CremaCatalana needs to be "crema_catalana".

	world_state.AddFact(sMenu, first_dish, second_dish, dessert); // (menu (paella migas salmorejo) (carne pescado tortilla_de_patatas) (yogurt flan crema_catalana))

	EXPECT_EQ(world_state.GetNumFactTables(sMenu), 1);

	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sMenu, 3), 1);

	HTNAtom first_dish_unbinded;
	HTNAtom second_dish_unbinded;
	HTNAtom third_dish_unbinded;
	EXPECT_EQ(world_state.QueryIndex(sMenu, 0, first_dish_unbinded, second_dish_unbinded, third_dish_unbinded), 1);

	EXPECT_TRUE(first_dish_unbinded.IsSet());
	EXPECT_TRUE(first_dish_unbinded.IsType<HTNAtomList>());
	EXPECT_EQ(first_dish_unbinded.GetListNumItems(), 3);

	const HTNAtom* first_dish_paella = first_dish_unbinded.FindListElement(0);
	EXPECT_TRUE(first_dish_paella != nullptr);
	EXPECT_TRUE(first_dish_paella->IsSet());
	EXPECT_TRUE(first_dish_paella->IsType<std::string>());
	EXPECT_TRUE(first_dish_paella->GetValue<std::string>() == "paella");

	const HTNAtom* first_dish_migas = first_dish_unbinded.FindListElement(1);
	EXPECT_TRUE(first_dish_migas != nullptr);
	EXPECT_TRUE(first_dish_migas->IsSet());
	EXPECT_TRUE(first_dish_migas->IsType<std::string>());
	EXPECT_TRUE(first_dish_migas->GetValue<std::string>() == "migas");

	const HTNAtom* first_dish_salmorejo = first_dish_unbinded.FindListElement(2);
	EXPECT_TRUE(first_dish_salmorejo != nullptr);
	EXPECT_TRUE(first_dish_salmorejo->IsSet());
	EXPECT_TRUE(first_dish_salmorejo->IsType<std::string>());
	EXPECT_TRUE(first_dish_salmorejo->GetValue<std::string>() == "salmorejo");
}