#include "HTNPlannerHook.h"

#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNLexer.h"
#include "Interpreter/HTNParser.h"
#include "Interpreter/HTNPrinter.h"
#include "Log.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	bool ReadFile(const std::string& inPath, std::string& outText)
	{
		// Open file
		std::ifstream File;
		File.open(inPath);
		if (!File.is_open())
		{
			LOG("File {} could not be opened", inPath);
			return false;
		}

		// Read file
		std::ostringstream Buffer;
		Buffer << File.rdbuf();
		outText = Buffer.str();

		// Close file
		File.close();

		return true;
	}

	const std::string kStartCompoundTaskName = "entry_point";
}

bool HTNPlannerHook::parseDomain(const char* inDomainPath)
{
	std::string Text;
	const bool ReadFileResult = ReadFile(inDomainPath, Text);
	if (!ReadFileResult)
	{
		LOG("File {} could not be read", inDomainPath);
		return false;
	}

	/*
	std::any Value;
	const bool InterpretTextResult = InterpretText(Text, Value);
	if (!InterpretTextResult)
	{
		LOG(R"(Text "{}" could not be interpreted)", Text);
		return false;
	}

	LOG("Value is {}", std::any_cast<double>(Value));
	*/

	return true;
}

std::vector<std::shared_ptr<const HTNTask>> HTNPlannerHook::MakePlan(const std::string& inDomainPath, const HTNWorldState& inWorldState) const
{
	std::string Text;
	const bool ReadFileResult = ReadFile(inDomainPath, Text);
	if (!ReadFileResult)
	{
		LOG("File {} could not be read", inDomainPath);
		return {};
	}

	// Lex
	HTNLexer Lexer = HTNLexer(Text);
	std::vector<HTNToken> Tokens;
	const bool LexerResult = Lexer.Lex(Tokens);
	if (!LexerResult)
	{
		LOG("Lex failed");
		return {};
	}

	// Parse
	HTNParser Parser = HTNParser(Tokens);
	std::shared_ptr<const HTNDomain> Domain = Parser.Parse();
	if (!Domain)
	{
		LOG("Parse failed");
		return {};
	}

	// Interpret
	HTNInterpreter Interpreter = HTNInterpreter(Domain, kStartCompoundTaskName);
	const std::vector<std::shared_ptr<const HTNTask>> Plan = Interpreter.Interpret(inWorldState);
	if (Plan.empty())
	{
		LOG("Interpret failed");
		return {};
	}

	// Print
	/*
	const HTNPrinter Printer = HTNPrinter(std::move(Domain));
	std::string Text;
	const bool PrintResult = Printer.Print(Text);
	if (!PrintResult)
	{
		LOG("Printer failed")
		return false;
	}

	LOG("Text is {}", Text);
	*/

	return Plan;
}
