# pragma once
# include "Config.hpp"

class Token
{
	private:

		std::string	_content;
		std::string	_type;
		size_t		_pos;
		size_t		_allowed_words;

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