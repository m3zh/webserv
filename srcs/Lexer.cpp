# include "../inc/Lexer.hpp"

Lexer::Lexer()      {} 
Lexer::~Lexer()     {}

std::string	    Lexer::types[]           = {
                                            "Digit",
                                            "Key",
                                            "Method",
                                            "Namespace",
                                            "Path",
                                            "Value"                       // to be deleted
                                        };
std::string	    Lexer::namespace_types[] = {
                                            "location",
                                            "server"
                                        };
std::string	    Lexer::key_types[]       = {                               // some key types will be deleted afterwards
                                            "allowed_methods",
                                            "autoindex",
                                            "client_max_body_size",
                                            "client_body_buffer_size",
                                            "error_page",
                                            "index",
                                            "limit_except",
                                            "listen",
                                            "redirect",
                                            "root",
                                            "server_name",
                                            "try_files",
                                            "upload_store",
                                            "workers"		
                                        };
std::string	    Lexer::method_types[]    = {                               // some key types will be deleted afterwards
                                            "GET",
                                            "DELETE",
                                            "POST",
                                            "PUT",
                                            "off",
                                            "on"	
                                        };
std::string		Lexer::separator_types   =  "#{};\0";

// ************
// READ, TAG, TOKENIZE functions
// ************

int     Lexer::read(char   *config, char **envp)
{
    setCurrWorkdir(envp);
    std::fstream file(config, std::fstream::in);
    if  (file.good() && valid_brackets(file))
    {
		std::string line;
        file.seekg(0);                                                      // start at beginning of file
        while (getline( file, line ))
        {
            if (line.length() == 0 || trim(line).find("#") == 0)
                continue;                                        // skip blank lines and comment lines
            line = trim(line);
            if  (!valid_lineending(line)
                || !tokenize(split(line.substr(0, line.length() - 1))))     // substr remove last character of each line, ie { ; }
                    {   std::cout << "Error in config\n"; file.close();  return 0;  }
        } 
        file.close();     
    }
    else
    {
        std::cout << "Error opening file\n";
        return 0;
    }
    return 1;
}

bool            Lexer::tag(Token& token)
{
    std::string token_content = token.getContent();

    if (token_content.find("#") != std::string::npos)
        return (handleComments(token));
    else if (match_anystring(token_content, namespace_types, 2))
        return  setNamespaceParams(token);
    else if (token_content.find("/") == 0 || token_content.compare(0, 2, "./") == 0)                                          // if it starts with a / it's a path.
        return  setPathParams(token);
    else if (match_anystring(token_content, method_types, 6))
        { token.setType("Method"); return true;             }
    else if (match_anystring(token_content, key_types, 13))
        return  setKeyParams(token);
    else if (token_content.find_first_not_of("0123456789") == std::string::npos)    // if it's only digits
        {   token.setType("Digit"); return true;            }
    else if (token_content.find_first_not_of("0123456789abcdefghijklmnopqrstuwxyz.") == std::string::npos)          // if it's a string of type mywebsite.com     
        {   token.setType("Value"); return true;            }
    else if (token_content.compare(0, 4, "http") == 0)          // if it's a string of type mywebsite.com     
        {   token.setType("Website"); return true;            }
    return false;
}

bool            Lexer::tokenize(std::vector<std::string> current_line)
{
    size_t  i;

    if (current_line.size() == 0)                                               // if line is empty, we do not tokenize
        return true;
    for ( i = 0; i < current_line.size(); i++)
    {
        Token token(current_line[i], i);                                        // create token with content and pos

        if (!tag(token))            return false;                               // si on a pas tag le token, c'est qu'on a un comment donc on skippe la ligne   
        tokens.push_back(token);
    }
    return validate_by_position(tokens, i);
}

// ************
// SET and GET functions
// ************

bool    Lexer::setPathParams(Token& token)
{
    int     fd;
    Token   last;
    token.setType("Path");
    
    last = tokens[tokens.size() - 1];
    if (last.getContent().compare("location") == 0
        || last.getContent().compare("upload_store") == 0)                                  // location and upload must start with '/' ONLY
        if (token.getContent().compare(0, 1, ".") == 0) return false;
    if (token.getContent().compare(0, 1, ".") == 0)
        token.setContent(token.getContent().substr(1, token.getContent().size() - 1));
    if (last.getContent().compare("root") == 0)                                             // root must be followed by an EXISTING path
    {
        fd = open((getCurrWorkdir() + token.getContent()).c_str(), O_RDONLY);               // check if absolute path exists
        if (fd < 0)
        {
            std::cout << "Invalid path in config" << std::endl;
            return false;
        }
        close(fd);
    }
    return true;    
}

bool    Lexer::setNamespaceParams(Token& token)
{
    token.setType("Namespace");
    token.setAllowedWords(1);
    if (token.getContent() == "server")
        token.setAllowedWords(0);        
    return  true;
}

bool            Lexer::setKeyParams(Token& token)
{
    token.setType("Key");
    token.setAllowedWords(1);
    if (token.getContent().compare("allowed_methods") == 0)
        token.setAllowedWords(4);
    return true;
}

void    	    Lexer::setCurrWorkdir(char **envp)
{
    size_t i = -1;

    while ( envp[++i] )
    {
        if (strncmp(envp[i], "PWD=", 4) == 0)
        {
            std::string tmp(envp[i]);
            curr_workdir = tmp.substr(4, tmp.length() - 4);
        }    
    }
}

std::string    	Lexer::getCurrWorkdir()     {       return curr_workdir;    }

// ************
// VALIDATE functions
// ************

int     Lexer::valid_brackets(std::fstream &f)                          // check if { } are well closed, change brackets with tokens
{
    std::vector<char>   brackets; 
    std::ostringstream  sstr;
    std::string         s;

    sstr << f.rdbuf();
    s = sstr.str();
    size_t len = s.size();
    for ( size_t i = 0; i < len; i++)
    {
        if ( s[i] == '{' || s[i] == '}' )
            brackets.push_back(s[i]);
    }
    std::vector<char>::iterator it = brackets.begin();
    while (it != brackets.end())
    {
        if ( *it == '{' && (it + 1) != brackets.end() && *(it + 1) == '}')
        {
            brackets.erase(it, it + 2);
            it = brackets.begin();
        }
        else
            ++it;
    }
    if (brackets.size())
    {
        std::cout << "Error\nNo Matching brackets in config file\n";
        return 0;
    }
    return 1;
}

int     Lexer::valid_lineending(std::string line)                {             return match_anychar(line.back(), "{;}");               };

bool    Lexer::validate_by_position(std::vector<Token> tokens, size_t num_of_tokens)            // check if tokens are in the right sequence (eg, port should follow listen, not viceversa)
{
    std::vector<Token>::iterator it = tokens.end();
    
    
    it = it - num_of_tokens;

    if ((*it).getContent() != "allowed_methods")
        if ((*it).getAllowedWords() > num_of_tokens)
            return false;
    while ( it != tokens.end() - 1 )
    {
        if (pair_wpath((*it).getContent()) && (*(it + 1)).getType() == "Path")                         // if pairs with path ok
            return true;
        if (pair_wdigits((*it).getContent()) && (*(it + 1)).getType() == "Digit")                      // if pairs with digit ok
            return true;
        if (pair_wvalues((*it).getContent()) && (*(it + 1)).getType() == "Value")                      // if pairs with value ok ( ie, generci string, website name )
            return true;
        if (pair_wmethods((*it).getContent()) && (*(it + 1)).getType() == "Method")                    // if pairs with methods ok
        {
            while ( ++it != tokens.end() )
                if ((*it).getType() != "Method")
                    return false;
            return true;
        }
        if ((*it).getContent().compare("redirect") == 0
            && (*(it + 1)).getType() == "Website")                      
            return true;                   
        it++;

    }
    return true;
}

bool    Lexer::handleComments(Token& token)
{
    std::string before_comment = token.getContent().substr(0, token.getContent().find("#"));

    token.setContent(before_comment);
    tag(token);
    tokens.push_back(token);
    return false;
}

// ************
// PAIR functions
// ************

bool            Lexer::pair_wpath(std::string word)    		// check if the word argument pairs with digits
{
    if ( word.compare("location") == 0 || word.compare("root") == 0
        || word.compare("upload_store") == 0 )
        return true;
    return false;
}

bool            Lexer::pair_wdigits(std::string word)    		// check if the word argument pairs with digits
{
    if ( word.compare("listen") == 0 || word.compare("client_max_body_size") == 0
        || word.compare("workers") == 0 || word.compare("client_body_buffer_size") == 0
            || word.compare("limit_except") == 0 )
        return true;
    return false;
}

bool            Lexer::pair_wvalues(std::string word)     		// check if the word argument pairs with values
{
    return (!pair_wdigits(word) && !pair_wmethods(word));
}

bool            Lexer::pair_wmethods(std::string word)    		// check if the word argument pairs with values
{
    if (word.compare("allowed_methods") == 0
        || word.compare("autoindex") == 0)
        return true;
    return false;
}

// ************
// STRING manipulations functions
// ************

std::string     Lexer::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    if  ( start == end )
        return s;
    return s.substr(start, end - start + 1);
}

int             Lexer::match_anystring(std::string word, std::string set[], size_t len)
{
    for ( size_t i = 0; i < len; i++ )
        if (word == set[i])
            return 1;
    return 0;
}

int             Lexer::match_anychar(char   c, std::string set)         {       return      set.find(c) != std::string::npos;       }

std::vector<std::string>     Lexer::split(std::string line)
{
    std::vector<std::string> current_line;

    std::size_t prev = 0, pos;
    while ((pos = line.find_first_of(" \n\r\t\f\v", prev)) != std::string::npos)
    {
        if (pos > prev)
            current_line.push_back(line.substr(prev, pos-prev));
        prev = pos+1;
    }
    if (prev < line.length())
        current_line.push_back(line.substr(prev, std::string::npos));
    return current_line;
}