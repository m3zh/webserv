# include "../inc/AToken.hpp"

AToken::AToken(std::string c) : _content(c), _type("None") {}
AToken::~AToken() {}

void 			AToken::setType(std::string t) { _type = t; }
std::string 	AToken::getType() { return _type; }
void 			AToken::setContent(std::string c) { _content = c; }
std::string 	AToken::getContent() { return _content; }

bool AToken::validate()
{

	return true;
}
