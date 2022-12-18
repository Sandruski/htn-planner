#pragma once

class HTNAtom
{
public:
	enum AtomType
	{
		EInvalid = 0,
		EInt,
		EFloat,
		EString,
		EObject, // TODO
		EList, // TODO
	};

	HTNAtom() = default;

	HTNAtom(int inValue)
	{
		Type = AtomType::EInt;
		Data.IntValue = inValue;
	}

	HTNAtom(float inValue)
	{
		Type = AtomType::EFloat;
		Data.FloatValue = inValue;
	}

	HTNAtom(const char* inValue)
	{
		Type = AtomType::EString;
		Data.StringValue = inValue;
	}

	bool IsSet() const
	{
		return (AtomType::EInvalid != Type);
	}
	
	bool IsListType() const 
	{
		return (AtomType::EList == Type);
	}

	// TODO: I think you need to implement operator==
	
	// Adds a new element to this list type, and return the next element in the list.
	HTNAtom* AddElementList(HTNAtom* inElement)
	{
		// TODO:
		(void*)inElement;
		return nullptr;
	}

	int GetListNumItems() const
	{
		if (AtomType::EList != Type)
			return -1;

		// TODO...
		return 0;
	}

	// Return the list element
	HTNAtom* GetListElement(int inElementIdx) const
	{
		(void*)&inElementIdx;
		return nullptr;
	}

	union HtnData
	{
		int			IntValue;
		float		FloatValue;
		const char* StringValue;
		HTNAtom*	mListValue;				///< Used for EList types. I am not sure about this being part of the HtnData, I suspect this might have to be moved out so we can have tree structures of HtnAtoms
	};

	AtomType Type = EInvalid;
	HtnData Data;
};
