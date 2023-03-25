#pragma once

struct HTNDecompositionContext;

class HTNCondition
{
public:
	virtual ~HTNCondition() = default;

	virtual bool Check(HTNDecompositionContext& ioContext) = 0;
};
