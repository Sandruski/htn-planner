// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Core/HTNTokenHelpers.h"

#include "Core/HTNTokenType.h"

namespace HTNTokenHelpers
{
std::string GetTokenTypeString(const HTNTokenType inTokenType)
{
    static const std::unordered_map<HTNTokenType, std::string> TokenTypesToStrings = {{HTNTokenType::COLON, "colon"},
                                                                                      {HTNTokenType::LEFT_PARENTHESIS, "left parenthesis"},
                                                                                      {HTNTokenType::RIGHT_PARENTHESIS, "right parenthesis"},
                                                                                      {HTNTokenType::EXCLAMATION_MARK, "exclamation mark"},
                                                                                      {HTNTokenType::QUESTION_MARK, "question mark"},
                                                                                      {HTNTokenType::HASH, "hash"},
                                                                                      {HTNTokenType::AT, "at"},
                                                                                      {HTNTokenType::HTN_DOMAIN, "domain"},
                                                                                      {HTNTokenType::HTN_TOP_LEVEL_DOMAIN, "top level domain"},
                                                                                      {HTNTokenType::HTN_METHOD, "method"},
                                                                                      {HTNTokenType::HTN_TOP_LEVEL_METHOD, "top level method"},
                                                                                      {HTNTokenType::HTN_AXIOM, "axiom"},
                                                                                      {HTNTokenType::HTN_CONSTANTS, "constants"},
                                                                                      {HTNTokenType::AND, "and"},
                                                                                      {HTNTokenType::OR, "or"},
                                                                                      {HTNTokenType::ALT, "alt"},
                                                                                      {HTNTokenType::NOT, "not"},
                                                                                      {HTNTokenType::IDENTIFIER, "identifier"},
                                                                                      {HTNTokenType::NUMBER, "number"},
                                                                                      {HTNTokenType::STRING, "string"},
                                                                                      {HTNTokenType::END_OF_FILE, "end of file"}};
    const auto                                                 It                  = TokenTypesToStrings.find(inTokenType);
    return (It != TokenTypesToStrings.end() ? It->second : "");
}
} // namespace HTNTokenHelpers
