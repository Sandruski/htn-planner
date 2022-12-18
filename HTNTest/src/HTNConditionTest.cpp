#include "HTNAtom.h"
#include "HTNWorldState.h"
#include "HTNDecompositionContext.h"
#include "HTNConditionWorldStateQuery.h"

TEST(HTNAtom, HTNConditionTest)
{
	HTNWorldState world_state;

	const char* sProgrammer("programmer");
	world_state.MakeFact(sProgrammer, "jota");
	world_state.MakeFact(sProgrammer, "sandra");
	world_state.MakeFact(sProgrammer, "jose");
	world_state.MakeFact(sProgrammer, "mariano");
	world_state.MakeFact(sProgrammer, "michele");
	world_state.MakeFact(sProgrammer, "andres");
	world_state.MakeFact(sProgrammer, "hector");
	
	const char* sLeadProgrammer("lead_programmer");
	world_state.MakeFact(sLeadProgrammer, "andres");
	
	HTNDecompositionContext decomposition_context;
	decomposition_context.mWorldState = &world_state;

	HTNAtom binded_to_jota("jota");
	HTNAtom unbinded_programmer;

	// Test condition node to ensure certain query with all the arguments binded decompose to true (Jota is a programmer)
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.setKey(sProgrammer);
		EXPECT_TRUE(binded_to_jota.IsSet());
		condition_world_state_query.setArgument(0, &binded_to_jota);
		EXPECT_TRUE(condition_world_state_query.check(decomposition_context));
	}

	// Test condition node to ensure certain query with all the arguments binded decompose to false (Jota is not a lead)
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.setKey(sLeadProgrammer);
		EXPECT_TRUE(binded_to_jota.IsSet());
		condition_world_state_query.setArgument(0, &binded_to_jota);
		EXPECT_FALSE(condition_world_state_query.check(decomposition_context));
	}

	// Test condition node to ensure certain query with all the arguments binded decompose to false (Jota is not a lead)
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.setKey(sLeadProgrammer);
		EXPECT_FALSE(unbinded_programmer.IsSet());
		condition_world_state_query.setArgument(0, &unbinded_programmer);
		// 
		EXPECT_TRUE(condition_world_state_query.check(decomposition_context));
		EXPECT_TRUE(unbinded_programmer.IsSet());
	}
}
