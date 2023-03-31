#include "HTNPlannerHook.h"

#include "Interpreter/Expressions/HTNExpressionBase.h"
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
		std::stringstream Buffer;
		Buffer << File.rdbuf();
		outText = Buffer.str();

		// Close file
		File.close();

		return true;
	}

	bool InterpretText(const std::string& inText, std::any& outValue)
	{
		// Lex
		HTNLexer Lexer = HTNLexer(inText);
		std::vector<HTNToken> Tokens;
		const bool LexerResult = Lexer.Lex(Tokens);
		if (!LexerResult)
		{
			LOG("Lex failed");
			return false;
		}

		// Parse
		HTNParser Parser = HTNParser(Tokens);
		std::unique_ptr<const HTNExpressionBase> Root;
		const bool ParseResult = Parser.Parse(Root);
		if (!ParseResult)
		{
			LOG("Parse failed");
			return false;
		}

		// Interpret
		const HTNInterpreter Interpreter = HTNInterpreter(std::move(Root));
		const bool InterpretResult = Interpreter.Interpret(outValue);
		if (!InterpretResult)
		{
			LOG("Interpret failed");
			return false;
		}

		// Print
		/*
		const HTNPrinter Printer = HTNPrinter(std::move(Root));
		std::string Text;
		const bool PrintResult = Printer.Print(Text);
		if (!PrintResult)
		{
			LOG("Printer failed")
			break;
		}

		LOG("Text is {}", Text);
		*/

		return true;
	}
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

	std::any Value;
	const bool InterpretTextResult = InterpretText(Text, Value);
	if (!InterpretTextResult)
	{
		LOG(R"(Text "{}" could not be interpreted)", Text);
		return false;
	}

	LOG("Value is {}", std::any_cast<double>(Value));

	return true;
}
