#include "Interpreter/HTNParser.h"

#include "HTNAtom.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Log.h"

std::shared_ptr<const HTNDomain> HTNParser::Parse()
{
	return ParseDomain();
}

std::shared_ptr<const HTNDomain> HTNParser::ParseDomain()
{
	// LEFT_PARENTHESIS COLON DOMAIN <identifier> <method>* RIGHT_PARENTHESIS

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::COLON))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::HTN_DOMAIN))
	{
		return nullptr;
	}

	std::unique_ptr<const HTNValue> Name = ParseIdentifier();
	if (!Name)
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<const HTNMethod>> Methods;
	while (std::shared_ptr<const HTNMethod> Method = ParseMethod())
	{
		Methods.emplace_back(Method);
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	return std::make_shared<HTNDomain>(std::move(Name), Methods);
}

std::shared_ptr<const HTNMethod> HTNParser::ParseMethod()
{
	// LEFT_PARENTHESIS COLON METHOD LEFT_PARENTHESIS <identifier> RIGHT_PARENTHESIS <branch>* RIGHT_PARENTHESIS

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::COLON))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::HTN_METHOD))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	std::unique_ptr<const HTNValue> Name = ParseIdentifier();
	if (!Name)
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<const HTNBranch>> Branches;
	while (std::shared_ptr<const HTNBranch> Branch = ParseBranch())
	{
		Branches.emplace_back(Branch);
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	return std::make_shared<HTNMethod>(std::move(Name), Branches);
}

std::shared_ptr<const HTNBranch> HTNParser::ParseBranch()
{
	// LEFT_PARENTHESIS <identifier> LEFT_PARENTHESIS <condition>* RIGHT_PARENTHESIS LEFT_PARENTHESIS <task>* RIGHT_PARENTHESIS RIGHT_PARENTHESIS

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	std::unique_ptr<const HTNValue> Name = ParseIdentifier();
	if (!Name)
	{
		return nullptr;
	}

	const std::shared_ptr<const HTNConditionBase> Condition = ParseCondition();

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<const HTNTask>> Tasks;
	while (std::shared_ptr<const HTNTask> Task = ParseTask())
	{
		Tasks.emplace_back(Task);
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	return std::make_shared<HTNBranch>(std::move(Name), Condition, Tasks);
}

std::shared_ptr<const HTNConditionBase> HTNParser::ParseCondition()
{
	// LEFT_PARENTHESIS ((AND | OR | NOT) <condition>) | <identifier>) RIGHT_PARENTHESIS

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	std::shared_ptr<const HTNConditionBase> Condition;

	if (ParseToken(HTNTokenType::AND))
	{
		std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
		while (std::shared_ptr<const HTNConditionBase> SubCondition = ParseCondition())
		{
			SubConditions.emplace_back(SubCondition);
		}

		Condition = std::make_shared<HTNConditionAnd>(SubConditions);
	}
	else if (ParseToken(HTNTokenType::OR))
	{
		std::vector<std::shared_ptr<const HTNConditionBase>> SubConditions;
		while (std::shared_ptr<const HTNConditionBase> SubCondition = ParseCondition())
		{
			SubConditions.emplace_back(SubCondition);
		}

		Condition = std::make_shared<HTNConditionOr>(SubConditions);
	}
	else if (ParseToken(HTNTokenType::NOT))
	{
		const std::shared_ptr<const HTNConditionBase> SubCondition = ParseCondition();
		Condition = std::make_shared<HTNConditionNot>(SubCondition);
	}
	else
	{
		// TODO salvarez Delete the HTNValue and do news of the HTNAtom that are created when reading the HTN domain

		if (std::unique_ptr<const HTNValue> Key = ParseIdentifier())
		{
			std::shared_ptr<HTNConditionWorldStateQuery> ConditionWorldStateQuery = std::make_shared<HTNConditionWorldStateQuery>();
			const char* Value = Key->GetValue().GetValue<const char*>(); // TODO salvarez Use std::string here instead of const char* because it's dangerous
			ConditionWorldStateQuery->SetKey(Value);
			Condition = ConditionWorldStateQuery;

			std::vector<HTNAtom*> SubConditions;
			while (std::shared_ptr<const HTNConditionBase> SubCondition = ParseCondition())
			{
				SubConditions.emplace_back(SubCondition);
			}
		}
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	return Condition;
}

std::shared_ptr<const HTNTask> HTNParser::ParseTask()
{
	// LEFT_PARENTHESIS EXCLAMATION_MARK* <identifier> (<number> | <string>)* RIGHT_PARENTHESIS

	if (!ParseToken(HTNTokenType::LEFT_PARENTHESIS))
	{
		return nullptr;
	}

	const EHTNTaskType Type = ParseToken(HTNTokenType::EXCLAMATION_MARK) ? EHTNTaskType::PRIMITIVE : EHTNTaskType::COMPOUND;

	std::unique_ptr<const HTNValue> Name = ParseIdentifier();
	if (!Name)
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<const HTNValue>> Arguments;
	while (std::shared_ptr<const HTNValue> Argument = ParseArgument())
	{
		Arguments.emplace_back(Argument);
	}

	if (!ParseToken(HTNTokenType::RIGHT_PARENTHESIS))
	{
		return nullptr;
	}

	return std::make_shared<HTNTask>(Type, std::move(Name), Arguments);
}

std::unique_ptr<const HTNValue> HTNParser::ParseArgument()
{
	// QUESTION_MARK <identifier>

	if (ParseToken(HTNTokenType::QUESTION_MARK))
	{
		return ParseIdentifier();
	}
	else if (std::unique_ptr<const HTNValue> Number = ParseNumber())
	{
		return Number;
	}
	else if (std::unique_ptr<const HTNValue> String = ParseString())
	{
		return String;
	}

	return nullptr;
}

std::unique_ptr<const HTNValue> HTNParser::ParseIdentifier()
{
	// IDENTIFIER

	const HTNToken* Identifier = ParseToken(HTNTokenType::IDENTIFIER);
	if (!Identifier)
	{
		return nullptr;
	}

	const std::string Value = std::any_cast<std::string>(Identifier->GetValue());
	return std::make_unique<HTNValue>(HTNAtom(Value.c_str()));
}

std::unique_ptr<const HTNValue> HTNParser::ParseNumber()
{
	// NUMBER
	 
	const HTNToken* Number = ParseToken(HTNTokenType::NUMBER);
	if (!Number)
	{
		return nullptr;
	}

	const float Value = std::any_cast<float>(Number->GetValue());
	return std::make_unique<HTNValue>(HTNAtom(Value));
}

std::unique_ptr<const HTNValue> HTNParser::ParseString()
{
	// STRING

	const HTNToken* String = ParseToken(HTNTokenType::STRING);
	if (!String)
	{
		return nullptr;
	}

	const std::string Value = std::any_cast<std::string>(String->GetValue());
	return std::make_unique<HTNValue>(HTNAtom(Value.c_str()));
}

const HTNToken* HTNParser::ParseToken(const HTNTokenType inTokenType)
{
	const HTNToken* Token = GetToken();
	if (!Token)
	{
		return nullptr;
	}

	const HTNTokenType TokenType = Token->GetType();
	if (inTokenType != TokenType)
	{
		return nullptr;
	}

	AdvancePosition();

	return Token;
}
