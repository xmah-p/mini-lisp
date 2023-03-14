#include "./parser.h"

#include "./error.h"

ValuePtr Parser::parse() {
    auto token = std::move(this->tokens.front());

    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<NumericValue>(value);
    }

    if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<BooleanValue>(value);
    }

    if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<StringValue>(value);
    }

    if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        tokens.pop_front();
        return std::make_shared<SymbolValue>(value);
    }

    if (token->getType() == TokenType::LEFT_PAREN) {
        tokens.pop_front();
        auto value = parseTails();
        return value;
    }

    throw SyntaxError("Unimplemented");
}

ValuePtr Parser::parseTails() {
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = parse();
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = parse();
        if (tokens.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Unexpected EOF; expect an element after .");
        }
        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}