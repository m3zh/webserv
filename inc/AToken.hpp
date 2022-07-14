# pragma once
# include "Parser.hpp"

class AToken
{
	private:

		std::string	_content;
		std::string	_type;

	public:
		AToken(std::string c);
		~AToken();
		
		bool			validate();
		void 			setType(std::string t);
		std::string 	getType();
		void 			setContent(std::string c);
		std::string 	getContent();

};
