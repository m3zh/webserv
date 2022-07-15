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

		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

		std::string     			trim(std::string s);
		int             			match_any(char c, std::string set);
		void             			split(std::string line);
        int             			valid_brackets(std::fstream &f);

		std::string					_rawFile;


	protected:

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
			"allow_methods"
		};


		std::map<std::string, int> 	n_words_types = {
			{"allowed_methods", 5},
			{"error_page", 2},

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
		


	public:

		Lexer();
		~Lexer();

        int 						read(char   *config);
		bool						tokenize();
		void						tag(Token& token);
		bool						valid_line(std::string line);
		
		// token methods ?
		void    					setTokenParams(Token& token);
		bool    					validate_by_position(Token& token);
		
		std::vector<std::string>			current_line;
		std::vector<Token>					tokens;
};
