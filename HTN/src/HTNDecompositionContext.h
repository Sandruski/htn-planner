#pragma once

class HTNWorldState;

struct HTNDecompositionContext
{
public:
	HTNDecompositionContext(const HTNWorldState& inWorldState) : mWorldState(&inWorldState)
	{
	}

	const HTNWorldState* GetWorldState() const
	{
		return mWorldState;
	}

	int GetIndex() const 
	{
		return mIndex;
	}

	void IncreaseIndex()
	{
		++mIndex;
	}

	void ResetIndex()
	{
		mIndex = 0;
	}

private:
	const HTNWorldState* mWorldState = nullptr;						///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	
	// NOTE JOSE: Explain what this represents
	int mIndex = 0;													
};
