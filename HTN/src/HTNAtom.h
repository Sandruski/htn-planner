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

	explicit HTNAtom(int inValue)
	{
		Type = AtomType::EInt;
		Data.IntValue = inValue;
	}

	explicit HTNAtom(float inValue)
	{
		Type = AtomType::EFloat;
		Data.FloatValue = inValue;
	}

	explicit HTNAtom(const char* inValue)
	{
		Type = AtomType::EString;
		Data.StringValue = inValue;
	}

	bool IsSet() const
	{
		return (AtomType::EInvalid != Type);
	}

	union HtnData
	{
		int			IntValue;
		float		FloatValue;
		const char* StringValue;
	};

	AtomType Type = EInvalid;
	HtnData Data;
};
