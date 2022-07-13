# pragma once
# include "Parser.hpp"

class AToken;

class Lexer
{
	private:
	
		Lexer();
		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

		std::string		_rawFile;

	protected:

		std::vector<std::string>	 raw;
		std::vector<AToken>			*tokens;

	public:

		Lexer(std::string fileName);
		~Lexer();

		std::vector<AToken>			*tokenize();

};
