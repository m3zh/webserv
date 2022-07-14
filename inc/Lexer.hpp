# pragma once

# include <iostream>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>
# include <map>

# include "AToken.hpp"
# include "TokenType.hpp"
# include "Parser.hpp"

class AToken;

class Lexer
{
	private:

		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

		std::string     			trim(std::string s);
        int             			valid_brackets(std::fstream &f);

		std::vector<std::string>	current_line;

	protected:

		std::vector<AToken>			tokens;

	public:

		Lexer();
		~Lexer();

        int 						read(char   *config);
		void						tokenize(std::string line); // crée l'array de token; si c'est invalide, on return NULL et notre main quitte 
		bool						valid_line(std::string line);
		void 						split(std::string line);
};
