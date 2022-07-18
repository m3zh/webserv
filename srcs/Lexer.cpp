# include "../inc/Lexer.hpp"

Lexer::Lexer()      {} 
Lexer::~Lexer()     {}

std::string	    Lexer::types[]  =       {
                                            "Namespace",
                                            "Key",
                                            "Value", // to be deleted
                                            "Digit",
                                            "Path",
                                            "Method",
                                            "Separator"
                                        };
std::string	    Lexer::namespace_types[] = {
                                            "server",
                                            "location"
                                        };
std::string	    Lexer::key_types[] = { // some key types will be deleted afterwards
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
std::string	    Lexer::method_types[] = {
                                            "on",
                                            "off",
                                            "GET",
                                            "POST",
                                            "PUT",
                                            "DELETE",
                                        };
std::string		Lexer::separator_types = "#{};";

int     Lexer::read(char   *config, char **envp)
{
    setCurrWorkdir(envp);
    std::fstream file(config, std::fstream::in);
    if  (file.good() && valid_brackets(file))
    {
		std::string line;
        file.seekg(0);              // start at beginning of file
        while (getline( file, line ))
        {
            while (line.length() == 0 || trim(line).find("#") == 0)
                getline(file, line); // skip blank lines and comment lines
            line = trim(line);
            if (!tokenize(split(line))) {   file.close();  return 0;  }
        } 
        file.close();     
    }
    else
    {
        std::cout << "Error opening file" << std::endl;
        return 0;
    }
    return 1;
}

int     Lexer::valid_brackets(std::fstream &f) // check if { } are well closed, change brackets with tokens
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

size_t  Lexer::count_words_left(Token& token)
{
    size_t words_left = 0;
    bool    start_counting = false;
    std::vector<std::string>::iterator it = current_line.begin();

    while (it != current_line.end())
    {
        if (*it == token.getContent() && !start_counting) // et aussi que it n'est pas un séparateur...
            start_counting = true;
        else
            words_left++;
        it++;
    }
    return words_left;
}

bool    Lexer::validate_by_position(std::vector<Token> tokens, size_t num_of_tokens)       // check if tokens are in the right sequence (eg, port should follow listen, not viceversa)
{
    (void)tokens;
    (void)num_of_tokens;
    // std::vector<Token>::iterator it = tokens.rbegin() - num_of_tokens;

    // while ( it != tokens(end) )
    // {
    //     if (*it.getType() == "Namespace")
    //     {

    //     }
    //     else
    //     {
            
    //     }
    //     it++;

    // }
    return true;
}

bool    Lexer::setPathParams(Token& token)
{
    int fd;
    // std::cout << "PATH\n";
    token.setType("Path");

    fd = open((getCurrWorkdir() + token.getContent()).c_str(), O_RDONLY);         // absolute path to rework
    if (fd < 0)
    {
        std::cout << "Invalid path in config" << std::endl;
        return false;
    }
    close(fd);
    return true;    
}

bool    Lexer::setNamespaceParams(Token& token)
{
    // std::cout << "NAMESPACE\n";
    token.setType("Namespace");
    if (token.getContent() == "location")
        token.setAllowedWords(2);       // + 1 pour le {
    else
        token.setAllowedWords(1);
    return  true;
}

bool   Lexer::setKeyParams(Token& token)
{
    // std::cout << "KEY\n";
    token.setType("Key");
    token.setAllowedWords(1);           // par défaut chaque clef en a au moins un OU PAS ! ex: on peut choisir de laisser "listen"

    // std::map<std::string, int>::iterator    it = n_words_types.begin();
    // while (it != n_words_types.end())
    // {
    //     if (it->first == token.getContent())
    //         token.setAllowedWords(it->second);
    //     it++;
    // }
    return true;
}

void    						Lexer::setCurrWorkdir(char **envp)
{
    size_t i = -1;

    while ( envp[++i] )
    {
        if (strncmp(envp[i], "PWD=", 4) == 0)
        {
            std::string tmp(envp[i]);
            curr_workdir = tmp.substr(5, tmp.length() - 5);
        }    
    }
}

std::string    						Lexer::getCurrWorkdir()     {       return curr_workdir;    }

bool    Lexer::handleComments(Token& token)
{
    // std::cout << "COMMENTS\n";
    std::string before_comment = token.getContent().substr(0, token.getContent().find("#"));

    token.setContent(before_comment);
    tag(token);
    tokens.push_back(token);
    return false;
}

bool    Lexer::tag(Token& token)
{
    std::string token_content = token.getContent();

    if (token_content.find("#") != std::string::npos)
        return (handleComments(token));
    else if (match_any(token_content, namespace_types))
        return  setNamespaceParams(token);
    else if (token_content.find("/") == 0)                              // if it starts with a / it's a path.
        return  setPathParams(token);
    else if (match_any(token_content, method_types))
        { token.setType("Method"); return true; }
    else if (match_any(token_content, key_types))
        return  setKeyParams(token);
    else if (token_content.find_first_of("{}#")) // ???
        {   token.setType("Value"); return true; }
    else if (token_content.size() == 1    )                              // if the token content matches a separator
                //&& match_any(token_content[0], separator_types))       
        {   token.setType("Separator"); return true; }
    return false;
}

bool    Lexer::tokenize(std::vector<std::string> current_line)
{
    size_t  i;

    for ( i = 0; i < current_line.size(); i++)
    {
        Token token(current_line[i], i);            // create token with content and pos

        if (!tag(token))                            // si on a pas tag le token, c'est qu'on a un comment donc on skippe la ligne
            return false;
        tokens.push_back(token);
    }
    if (!validate_by_position(tokens, i))
        return false;
    return true;
}

// string manipulation functions

std::string     Lexer::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    if  ( start == end )
        return s;
    return s.substr(start, start - end);
}


int             Lexer::match_any(std::string word, std::string set[])
{
    for ( size_t i = 0; i < set->length(); i++ )
        if (word.compare(set[i]) == 0)
            return 1;
    return 0;
}

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
