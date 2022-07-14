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
    std::cout << "1) Brackets OK\n";
    return 1;
}



std::vector<AToken>	Lexer::tokenize()
{
    std::vector<AToken> curr_line_tokens;

    for (size_t i=0; i < current_line.size(); i++)
    {
        AToken elem(current_line[i]);

        if (current_line[i] == "server" || current_line[i] == "location")
            elem.setType("Namespace");

        curr_line_tokens.push_back(elem);
    }
    return curr_line_tokens;
}

bool	Lexer::valid_line(std::string line)
{
    std::vector<AToken> line_tokens;

    current_line.clear();
	split(line);
    line_tokens = tokenize();
    tokens.push_back(line_tokens);
	return true;
}

// string manipulation functions

std::string     Lexer::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    return s.substr(start, end);
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
