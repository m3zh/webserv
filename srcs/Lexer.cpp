# include "../inc/Lexer.hpp"

Lexer::	Lexer() {}
Lexer::~Lexer() {}

int     Lexer::read(char   *config)
{
    std::fstream file(config, std::fstream::in);
    if  (file.good() && valid_brackets(file))
    {
		std::string line;
        file.seekg(0);
        while (getline( file, line ))
        {
            while (line.length() == 0)
                getline(file, line); // skip blank lines
            line = trim(line);
            if (!valid_line(line))
				return 0;
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

int     Lexer::valid_brackets(std::fstream &f) // check if { } are well closed  
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

bool    Lexer::validate_by_position(Token& token)
{
    size_t words_left = count_words_left(token);
    
    if ((token.getType() == "Namespace" || token.getType() == "Key")
    &&  (token.getPos() != 0            || words_left > token.getAllowedWords() || (!token.getAllowedWords() && words_left > 0))) // && is separator ?
    // si allowed words > 1; alors il faut min 1
        return false;
    if (token.getType() == "Value" && token.getPos() == 0)
        return false;
    return true;
}

void    Lexer::setNamespaceParams(Token& token)
{
    token.setType("Namespace");
    if (token.getContent() == "location")
        token.setAllowedWords(2); // + 1 pour le {
    else
        token.setAllowedWords(1);
}

void    Lexer::setKeyParams(Token& token)
{
    token.setType("Key");
    token.setAllowedWords(1); // par défaut chaque clef en a au moins un OU PAS ! ex: on peut choisir de laisser "listen"

    std::map<std::string, int>::iterator    it = n_words_types.begin();
    while (it != n_words_types.end())
    {
        if (it->first == token.getContent())
            token.setAllowedWords(it->second);
        it++;
    }
}

bool    Lexer::handleComments(Token& token)
{
    std::string before_comment = token.getContent().substr(0, token.getContent().find("#"));

    std::cout << before_comment << "\n";
    token.setContent(before_comment);
    tag(token);
    return false;
}

bool    Lexer::tag(Token& token)
{
    if (!token.getContent().find("#"))
        return (handleComments(token));
    else if (std::find(namespace_types.begin(), namespace_types.end(), token.getContent()) != namespace_types.end())
        setNamespaceParams(token);
    else if (!token.getContent().find("/")) // a préciser ...
        token.setType("Path");
    else if (std::find(method_types.begin(), method_types.end(), token.getContent()) != method_types.end())
        token.setType("Method");
    else if (std::find(key_types.begin(), key_types.end(), token.getContent()) != key_types.end())
        setKeyParams(token);
    else if (token.getContent().find_first_of("{}#"))
        token.setType("Value"); // temporaire ...
    else
    {
        for (size_t i=0; i < separator_types.size(); i++)
            if (separator_types.find(token.getContent()))
                token.setType("Separator"); // à compléter ..
    }
    return true;
}

bool    Lexer::tokenize()
{
    for (size_t i=0; i < current_line.size(); i++)
    {
        // if (current_line[i] == "#") // si notre mot est un #, alors on a pas besoin de tag les mots suivants
        //     break; => a revoir !! pcq un mot peut contenir un # ...

        Token elem(current_line[i], i);

        if (!tag(elem)) // si on a pas tag l'élément, c'est qu'on a un comment don on skippe la ligne
            break;
        if (!validate_by_position(elem))
            return false;

        tokens.push_back(elem);
    }
    return true;
}

bool	Lexer::valid_line(std::string line)
{
    current_line.clear();
	split(line);
    if (!tokenize())
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


int             Lexer::match_any(char c, std::string set)
{
    std::string::iterator it = set.begin();
    while ( it != set.end() )
    {   
        if (*it == c) return 1;
        it++;     
    }
    return 0;
}

void     Lexer::split(std::string line)
{
    std::size_t prev = 0, pos;
    while ((pos = line.find_first_of(" \n\r\t\f\v", prev)) != std::string::npos)
    {
        if (pos > prev)
            current_line.push_back(line.substr(prev, pos-prev));
        prev = pos+1;
    }
    if (prev < line.length())
        current_line.push_back(line.substr(prev, std::string::npos));
}
