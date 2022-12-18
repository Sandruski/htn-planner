#pragma once

class HTNWorldState;
struct HTNDecompositionContext
{
	const HTNWorldState*				mWorldState = nullptr;						///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
};
