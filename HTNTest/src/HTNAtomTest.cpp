#include "HTNAtom.h"

TEST(HTNAtom, HTNAtomInitialization) 
{
	HTNAtom HTNAtom;
	EXPECT_EQ(HTNAtom.IntValue, 0);
}
