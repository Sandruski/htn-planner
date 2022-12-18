#pragma once

#include "HTNCondition.h"

struct HTNDecompositionContext;
class HTNConditionWorldStateQuery : public HTNCondition
{
public:
	// Check if the condition is true.
	virtual bool check(HTNDecompositionContext& ioContext) override
	{
		(void*)&ioContext;
		// TODO JOSE:
		return false;
	}

	void				setKey(const char* inKey) { mKey = inKey; }
	void				setArgument(int inIdx, HTNAtom* inAtom0)
	{
		(void*)&inIdx;
		(void*)inAtom0;
		// TODO JOSE:
	}
private:
	const char*			mKey	= nullptr;
	// TODO JOSE: Add arguments?
};
