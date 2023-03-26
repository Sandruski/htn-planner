#include "HTNPlannerHook.h"

#include "Interpreter/Expressions/HTNExpressionBase.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNLexer.h"
#include "Interpreter/HTNParser.h"
#include "Interpreter/HTNPrinter.h"
#include "Log.h"

#include <fstream>
#include <string>
#include <vector>

void HTNPlannerHook::parseDomain(const char* inDomainPath)
{
	std::ifstream File = std::ifstream(inDomainPath);
	if (!File.is_open())
	{
		LOG("File {} could not be opened", inDomainPath);
		return;
	}

	std::string Line;
	while (getline(File, Line))
	{
		// Lex
		HTNLexer Lexer = HTNLexer(Line);
		std::vector<HTNToken> Tokens;
		const bool LexerResult = Lexer.Lex(Tokens);
		if (!LexerResult)
		{
			LOG("Lex failed")
			break;
		}

		// Parse
		HTNParser Parser = HTNParser(Tokens);
		std::unique_ptr<const HTNExpressionBase> Root;
		const bool ParseResult = Parser.Parse(Root);
		if (!ParseResult)
		{
			LOG("Parse failed")
			break;
		}

		// Interpret
		const HTNInterpreter Interpreter = HTNInterpreter(std::move(Root));
		std::any Value;
		const bool InterpretResult = Interpreter.Interpret(Value);
		if (!InterpretResult)
		{
			LOG("Interpret failed")
			break;
		}

		LOG("Value is {}", std::any_cast<double>(Value));

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
	}

	File.close();
}
