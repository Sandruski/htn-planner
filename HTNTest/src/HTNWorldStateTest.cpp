#include "HTNAtom.h"
#include "HTNWorldState.h"

TEST(HTNAtom, HTNWorldStateSimpleFacts)
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

// Test adding facts
TEST(HTNAtom, HTNWorldStateComplexFacts)
{
	HTNWorldState world_state;

	const char* sMenu("menu");
	EXPECT_EQ(world_state.GetNumFactTables(sMenu), -1);


	// First dish
	HTNAtom* paella = new HTNAtom("paella");
	HTNAtom* migas = new HTNAtom("migas");
	HTNAtom* salmorejo = new HTNAtom("salmorejo");
	HTNAtom first_dish;
	first_dish.AddElementList(paella)
				->AddElementList(migas)
				->AddElementList(salmorejo);

	// Second dish
	HTNAtom* carne = new HTNAtom("carne");
	HTNAtom* pescado = new HTNAtom("pescado");
	HTNAtom* tortilla_de_patatas = new HTNAtom("TortillaDePatatas");			// Careful! We need to convert all the string to snake_case format! so TortillaDePatatas needs to be "tortilla_de_patatas".
	HTNAtom second_dish;
	second_dish.AddElementList(carne)
				->AddElementList(pescado)
				->AddElementList(tortilla_de_patatas);

	HTNAtom* yogurt = new HTNAtom("yogurt");
	HTNAtom* flan = new HTNAtom("flan");
	HTNAtom* crema_catalana = new HTNAtom("CremaCatalana");					// Careful! We need to convert all the string to snake_case format! so CremaCatalana needs to be "crema_catalana".
	HTNAtom dessert;
	dessert.AddElementList(yogurt)
		->AddElementList(flan)
		->AddElementList(crema_catalana);

	world_state.MakeFact(sMenu, first_dish, second_dish, dessert);								// (menu (paella migas salmorejo) (carne pescado tortilla_de_patatas) (yogurt flan crema_catalana))
	EXPECT_EQ(world_state.GetNumFactTables(sMenu), 1);

	EXPECT_EQ(world_state.GetNumFactTablesByNumArgs(sMenu, 3), 1);

	HTNAtom first_dish_unbinded;
	EXPECT_EQ(world_state.Query(sMenu, first_dish_unbinded), 1);

	EXPECT_TRUE(first_dish_unbinded.IsSet());

	EXPECT_TRUE(first_dish_unbinded.IsType<HTNAtomList>());

	EXPECT_EQ(first_dish_unbinded.GetListNumItems(), 3);
}
