# pragma once

# include <algorithm>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>
# include <map>

# include "Token.hpp"

class Token;

class Lexer
{
	private:

		std::string     			trim(std::string s);				   // remove trailing spaces on the left and right of the line
		int             			match_any(char c, std::string set);    // check if the char argument matches any char in set argument
		void             			split(std::string line);			   // split lines on multiple separators (i.e. spaces)
        int             			valid_brackets(std::fstream &f);       // check if brackets in config are closed

		std::vector<std::string>	types = {
			"Namespace",
			"Key",
			"Value",
			"Path",
			"Method",
			"Separator"
		};
		std::vector<std::string>	namespace_types = {
			"server",
			"location"
		};
		std::vector<std::string>	key_types = {
			"listen",
			"ssl",
			"ssl_certificate",
			"ssl_certificate_key",
			"ssl_protocols",
			"root",
			"index",
			"autoindex",
			"try_files",
			"fastcgi_split_path_info",
			"fastcgi_pass",
			"fastcgi_param",
			"fastcgi_index",
			"include",
			"error_page",
			"cgi",
			"cgi_bin",
			"limit_except",
			"client_max_body_size",
			"client_body_buffer_size",
			"upload",
			"workers",
			"auth",
			"server_name",
			"allowed_methods"
		};


		std::map<std::string, int> 	n_words_types = {
			{"allowed_methods", 5},
			{"error_page", 2},
			{"listen", 2},
			{"index", 2},
			{"ssl_protocols", 2},
		};
		std::vector<std::string>	method_types = {
			"on",
			"off",
			"GET",
			"HEAD",
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

        int 						read(char   *config);
		bool						tokenize();
		bool						tag(Token& token);
		
		// token methods ?
		void    					setKeyParams(Token& token);
		void    					setNamespaceParams(Token& token);
		bool					    handleComments(Token& token);
		void    					setPathParams(Token& token);
		
		bool    					validate_by_position(Token& token);
		size_t						count_words_left(Token& token);
		
		std::vector<std::string>			current_line;
		std::vector<Token>					tokens;
};
