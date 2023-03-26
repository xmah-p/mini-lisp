#include "./parser.h"
#include <memory>

#include "./error.h"

ValuePtr Parser::parse() {
    if (this->tokens.empty()) throw SyntaxError("Unexpected end of file");

    auto token = std::move(this->tokens.front());
    tokens.pop_front();

    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    }

    if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    }

    if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    }

    if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(value);
    }

    if (token->getType() == TokenType::LEFT_PAREN) {
        auto value = parseTails();
        return value;
    }

    if (token->getType() == TokenType::QUOTE) {
        auto quote = std::make_shared<SymbolValue>("quote");
        auto value = parse();
        return Value::makeList({quote, value});
    }

    if (token->getType() == TokenType::QUASIQUOTE) {
        auto quasiquote = std::make_shared<SymbolValue>("quasiquote");
        auto value = parse();
        return Value::makeList({quasiquote, value});
    }

    if (token->getType() == TokenType::UNQUOTE) {
        auto unquote = std::make_shared<SymbolValue>("unquote");
        auto value = parse();
        return Value::makeList({unquote, value});
    }

    throw SyntaxError("Unsupported syntax");
}

ValuePtr Parser::parseTails() {
    if (tokens.size() == 0)
        throw SyntaxError("Unexpected end of file");

    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = parse();
    if (tokens.size() == 0) throw SyntaxError("Unexpected end of file");
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        if (tokens.size() == 0) throw SyntaxError("Unexpected end of file");
        auto cdr = parse();
        if (tokens.size() == 0) throw SyntaxError("Unexpected end of file");
        if (tokens.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected exactly one element after .");
        }
        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}