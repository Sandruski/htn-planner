#include "HTNAtom.h"

TEST(HTNAtom, HTNAtomInitialization) 
{
	HTNAtom HTNAtom(0);
	EXPECT_EQ(HTNAtom.Data.IntValue, 0);
}
