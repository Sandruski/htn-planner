#pragma once

#include <string>

class HTNAtom
{
public:
	enum AtomType
	{
		EInvalid = 0,
		EInt,
		EFloat,
		EString,
		EObject,
		EList,
	};

	HTNAtom() : Type(AtomType::EInvalid) {}

	HTNAtom(const char* inValue) 
	{
		Type = AtomType::EString;
		Data.StringValue = inValue;
	}

	HTNAtom(int inValue)
	{
		Type = AtomType::EInt;
		Data.IntValue = inValue;
	}

	union HtnData
	{
		int			IntValue;
		float		FloatValue;
		const char* StringValue;
	};

	AtomType		Type = EInt;
	HtnData			Data;
};
