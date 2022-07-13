# pragma once
# include "Parser.hpp"

class AToken
{
	private:

		std::string	content;
		std::string	type;

	public:
		AToken();
		virtual ~AToken();
		
		bool	validate();

};
