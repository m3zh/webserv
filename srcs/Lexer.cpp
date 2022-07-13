# include "../inc/Lexer.hpp"

Lexer::	Lexer() {}
Lexer::~Lexer() {}

int     Lexer::read(char   *config)
{
    std::fstream file(config, std::fstream::in);
    if  (file.good() && valid_brackets(file))
    {
		std::string line;
        while (getline( file, line ))
        {
			std::cout << "ici\n";
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

void	Lexer::tokenize()
{

}

bool	Lexer::valid_line(std::string line)
{
	std::cout << line << std::endl;
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
