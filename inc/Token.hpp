# pragma once
# include "Parser.hpp"

class Token
{
	private:

		std::string	_content;
		std::string	_type;
		size_t		_pos;

	public:
		Token(std::string c);
		~Token();
		
		bool			validate();
		void 			setType(std::string t);
		std::string 	getType();
		void 			setContent(std::string c);
		std::string 	getContent();
		void 			setPos(size_t p);
		size_t 			getPos();

};
