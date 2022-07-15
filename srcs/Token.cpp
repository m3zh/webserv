# include "../inc/Token.hpp"

Token::Token(std::string c) : _content(c), _type("None"), _pos(0) {}
Token::~Token() {}

void 			Token::setType(std::string t) { _type = t; }
std::string 	Token::getType() { return _type; }
void 			Token::setContent(std::string c) { _content = c; }
std::string 	Token::getContent() { return _content; }
void 			Token::setPos(size_t p) { _pos = p; }
size_t 			Token::getPos() { return _pos; }

bool Token::validate()
{

	return true;
}
