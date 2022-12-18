#pragma once

struct HTNDecompositionContext;
class HTNCondition
{
public:
	virtual bool check(HTNDecompositionContext& ioContext) = 0;
};
