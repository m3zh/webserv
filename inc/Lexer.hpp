/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/15 09:13:31 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/12 16:33:11 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
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

# define ALLOWED_TYPES 	4
# define ERROR_PAGES	2
# define LISTEN 		1
# define INDEX 			1

// ****************************
// Token is the smallest unit the config can be divided into
// Lexer splits the file.conf split into tokens based on the space separators " \n\r\t\f\v"
// Each token has:
// - _content = the actual string found in the config file
// - _type = the type of token ( is it a namespace? a port? a location? a path? a method ? ... )
// - _pos = the pos of the token ( a namespace can only be in position 0, a path can only be in position 1, etc. )
// - _allowed_words = allowed words for each type of token
// and check if they are valid tokens according to their content and position in the config
// ****************************

class Token
{
	private:

		std::string		_content;
		std::string		_type;
		size_t			_pos;
		size_t			_allowed_words;

	public:

		Token();
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

class Lexer
{
	private:

		// string manipulation utils
		std::string     			trim(std::string s);				   									// remove trailing spaces on the left and right of the line
		int             			match_anystring(std::string word, std::string set[], size_t len);    	// check if the char argument matches any string in set argument
		int             			match_anychar(char c, std::string set);    								// check if the char argument matches any char in set argument
		bool             			pair_wpath(std::string word);    										// check if the word argument pairs with path
		bool             			pair_wdigits(std::string word);    										// check if the word argument pairs with digits
		bool             			pair_wvalues(std::string word);    										// check if the word argument pairs with values
		bool             			pair_wmethods(std::string word);    									// check if the word argument pairs with methods
		std::vector<std::string>	split(std::string line);			   									// split lines on multiple separators (i.e. spaces)
        int             			valid_brackets(std::fstream &f);       									// check if brackets in config are closed
        int             			valid_lineending(std::string line);      								// check if line ending is valid ( if it ends with ; { or } )

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
		
		bool    						setKeyParams(Token& token);
		bool    						setNamespaceParams(Token& token);
		bool    						setPathParams(Token& token);

		void    						setCurrWorkdir(char **envp);
		std::string  					getCurrWorkdir();

		std::vector<std::string>		current_line;
		std::vector<Token>				tokens;
};
