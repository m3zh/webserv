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

# include "Token.hpp"

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
		std::string     			trim(std::string s);				   // remove trailing spaces on the left and right of the line
		int             			match_any(char c, std::string set);    // check if the char argument matches any char in set argument
		void             			split(std::string line);			   // split lines on multiple separators (i.e. spaces)
        int             			valid_brackets(std::fstream &f);       // check if brackets in config are closed

		// Lexer TAGS
		std::vector<std::string>	types = {
			"Namespace",
			"Key",
			"Value", // to be deleted
			"Digit",
			"Path",
			"Method",
			"Separator"
		};
		std::vector<std::string>	namespace_types = {
			"server",
			"location"
		};
		std::vector<std::string>	key_types = { // some key types will be deleted afterwards
			"allowed_methods",
			"autoindex",
			"client_max_body_size",
			"client_body_buffer_size",
			"error_page",
			"include",
			"index",
			"limit_except",
			"listen",
			"redirect",
			"root",
			"server_name",
			"try_files",
			"upload",
			"workers"		
		};

		std::map<std::string, int> 	n_words_types = {
			{"allowed_methods", 4},						// "allowed_methods" can take up to 4 words GET, POST, PUT, DELETE
			{"error_page", 		2},						// "error_page" can take up to 2 words
			{"listen", 			1},						// etc.
			{"index", 			2}
		};
		std::vector<std::string>	method_types = {
			"on",
			"off",
			"GET",
			"POST",
			"PUT",
			"DELETE",
		};
		std::string		separator_types = "#{};";
		
		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

	public:

		Lexer();
		~Lexer();

        int 							read(char   *config);
		bool							tokenize();
		bool							tag(Token& token);
		bool					    	handleComments(Token& token);
		bool    						validate_by_position(Token& token);
		size_t							count_words_left(Token& token);
		
		// token methods
		bool    						setKeyParams(Token& token);
		bool    						setNamespaceParams(Token& token);
		bool    						setPathParams(Token& token);

		std::vector<std::string>		current_line;
		std::vector<Token>				tokens;
};
