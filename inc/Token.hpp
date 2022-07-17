# pragma once
# include <iostream>
# include <cstring>
# include <fstream>

// ****************************
// Token is the smallest unit the config can be divided into
// Config text is split into tokens based on the space separators " \n\r\t\f\v"
// Each token has:
// - _content = the actual string found in the config file
// - _type = the type of token ( is it a namespace? a port? a location? a path? a method ? ... )
// - _pos = the pos of the token ( a namespace can only be in position 0, a path can only be in position 1, etc. )
// - _allowed_words = allowed words for each type of token
// ****************************

class Token
{
	private:

		std::string		_content;
		std::string		_type;
		size_t			_pos;
		size_t			_allowed_words;

	public:

		Token(std::string c, size_t p);
		~Token();
		
		void 			setType(std::string t);
		std::string 	getType();
		void 			setContent(std::string c);
		std::string 	getContent();
		void 			setPos(size_t p);
		size_t 			getPos();
		void 			setAllowedWords(size_t p);
		size_t 			getAllowedWords();

};
