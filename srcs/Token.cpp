# include "../inc/Token.hpp"

Token::Token(std::string c, size_t p) : _content(c), _type("None"), _pos(p), _allowed_words(0) {}
Token::~Token() {}

void 			Token::setType(std::string t) 			{ 	_type = t;		 		}
void 			Token::setContent(std::string c) 		{ 	_content = c; 			}
void 			Token::setPos(size_t p) 				{	_pos = p; 				}
void 			Token::setAllowedWords(size_t p) 		{ 	_allowed_words = p; 	}

std::string 	Token::getType() 						{ 	return _type; 			}
std::string 	Token::getContent() 					{	return _content; 		}
size_t 			Token::getPos() 						{ 	return _pos; 			}
size_t 			Token::getAllowedWords() 				{ 	return _allowed_words; 	}
