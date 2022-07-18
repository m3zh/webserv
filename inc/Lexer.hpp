# pragma once

# include <algorithm>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>
# include <map>
# include <fcntl.h>
# include <stdexcept>
# include <unistd.h>
# include <limits.h>

# include "Token.hpp"

# define ALLOWED_TYPES 	4
# define ERROR_PAGES	2
# define LISTEN 		1
# define INDEX 			1

// ****************************
// Token is the smallest unit the config can be divided into
// the Config text is split into tokens based on the space separators " \n\r\t\f\v"
// Each token has:
// - _content = the actual string found in the config file
// - _type = the type of token ( is it a namespace? a port? a location? a path? a method? ... )
// - _pos = the pos of the token ( does the pathe follow a location token? )
// - _allowed_words = the pos of the token ( does the pathe follow a location token? )
// ****************************

class Token;

class Lexer
{
	private:

		// string manipulation utils
		std::string     			trim(std::string s);				   		// remove trailing spaces on the left and right of the line
		int             			match_anystring(std::string word, std::string set[]);    // check if the char argument matches any char in set argument
		int             			match_anychar(char c, std::string set);    // check if the char argument matches any char in set argument
		std::vector<std::string>	split(std::string line);			   		// split lines on multiple separators (i.e. spaces)
        int             			valid_brackets(std::fstream &f);       		// check if brackets in config are closed
        int             			valid_lineending(std::string line);      	// check if brackets in config are closed

		// Lexer TAGS - static members
		static std::string			types[];
		static std::string			namespace_types[];
		static std::string			key_types[];
		static std::string			method_types[];
		static std::string			separator_types;
		std::string					curr_workdir;
		
		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

	public:

		Lexer();
		~Lexer();

        int 							read(char   *config, char** envp);
		bool							tokenize(std::vector<std::string> current_line);
		bool							tag(Token& token);
		bool					    	handleComments(Token& token);
		bool    						validate_by_position(std::vector<Token> tokens, size_t num_of_tokens);
		
		// token methods
		bool    						setKeyParams(Token& token);
		bool    						setNamespaceParams(Token& token);
		bool    						setPathParams(Token& token);
		void    						setCurrWorkdir(char **envp);

		std::string  					getCurrWorkdir();

		std::vector<std::string>		current_line;
		std::vector<Token>				tokens;
};
