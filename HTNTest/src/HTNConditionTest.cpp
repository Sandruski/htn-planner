#include "Domain/Interpreter/AST/HTNCondition.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNWorldState.h"
#include "Runtime/HTNDecompositionContext.h"

TEST(HTNAtom, DISABLED_HTNConditionTest)
{
	/*
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
	
	HTNDecompositionContext decomposition_context = HTNDecompositionContext(world_state);

	HTNAtom binded_to_jota("jota");
	HTNAtom unbinded_programmer;

	// Test condition node to ensure certain query with all the arguments binded decompose to true (Jota is a programmer)
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.SetKey(sProgrammer);
		EXPECT_TRUE(binded_to_jota.IsSet());
		condition_world_state_query.AddArgument(binded_to_jota);
		EXPECT_TRUE(condition_world_state_query.Check(decomposition_context));
	}

	// Test condition node to ensure certain query with all the arguments binded decompose to false (Jota is not a lead)
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.SetKey(sLeadProgrammer);
		EXPECT_TRUE(binded_to_jota.IsSet());
		condition_world_state_query.AddArgument(binded_to_jota);
		EXPECT_FALSE(condition_world_state_query.Check(decomposition_context));
	}

	// Test condition node to ensure certain query with all the arguments unbinded decompose to true
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.SetKey(sLeadProgrammer);
		EXPECT_FALSE(unbinded_programmer.IsSet());
		condition_world_state_query.AddArgument(unbinded_programmer);
		// 
		EXPECT_TRUE(condition_world_state_query.Check(decomposition_context));
		EXPECT_TRUE(unbinded_programmer.IsSet());
	}
	*/
}


TEST(HTNAtom, DISABLED_HTNMultiConditionTest)
{
	/*
	HTNWorldState world_state;

	const char* sProgrammer("programmer");
	world_state.MakeFact(sProgrammer, "jota");
	world_state.MakeFact(sProgrammer, "sandra");
	world_state.MakeFact(sProgrammer, "jose");
	world_state.MakeFact(sProgrammer, "mariano");
	world_state.MakeFact(sProgrammer, "michele");
	world_state.MakeFact(sProgrammer, "andres");

	HTNDecompositionContext multiresult_decomposition_context = HTNDecompositionContext(world_state);

	// Test condition multiple times, a condition node might have multiple valid results.
	// So every new call to Check should end up with the proper return + index increase, so next time we call it
	// we will test the next index.
	// A different alternative would be, the index is set to -1 at start and when we call Check we always increment the index first
	// before query the row in the database.
	// For now, we assume that the index at start is -1 and that we increase the index inside Check function before the proper check.
	// So, when we call Check once at the end of the Check function the value of the index should be 0.
	// Calling Check for second time will make index be 1 at the end of the Check function.
	// Calling Check for third time will make index be 2 at the end of the Check function.
	// etc... etc...
	{
		HTNConditionWorldStateQuery	condition_world_state_query;
		condition_world_state_query.SetKey(sProgrammer);

		HTNAtom unbinded_programmer_multiresult_0;
		condition_world_state_query.AddArgument(unbinded_programmer_multiresult_0);

		// Test first solution (jota)
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "jota");
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 0);


		// Test second solution (sandra)
		unbinded_programmer_multiresult_0.UnBind();													// unbind the previously binded HtnAtom because we are gonna reuse it.
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));			// second call to Check, if we are in a multi result context this should return true and a different result that the first time we call it.
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "sandra");	// second result of the database should return sandra.
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 1);									// the index after the second call should be 1.


		// Test second solution (jose)
		unbinded_programmer_multiresult_0.UnBind();													// unbind the previously binded HtnAtom because we are gonna reuse it.
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));			// third call to Check, if we are in a multi result context this should return true and a different result that the first and second time we call it.
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "jose");	// third result of the database should return jose.
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 2);									// the index after the second call should be 2.


		// unbind and test for mariano
		unbinded_programmer_multiresult_0.UnBind();
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "mariano");
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 3);

		// unbind and test for michele
		unbinded_programmer_multiresult_0.UnBind();
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "michele");
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 4);

		// unbind and test for andres
		unbinded_programmer_multiresult_0.UnBind();
		EXPECT_TRUE(condition_world_state_query.Check(multiresult_decomposition_context));
		EXPECT_TRUE(unbinded_programmer_multiresult_0.GetValue<std::string>() == "andres");
		EXPECT_EQ(multiresult_decomposition_context.GetIndex(condition_world_state_query), 5);

		// unbind and test last time, this should return false because we have inspected all the possible solutions!
		unbinded_programmer_multiresult_0.UnBind();
		EXPECT_FALSE(condition_world_state_query.Check(multiresult_decomposition_context));
	}
	*/
}