#  include "../inc/Parser.hpp"

Parser::Parser ():  _ip(0),
                    _port(0),
                    _protocol(0),
                    _config(0) {};

Parser::~Parser()  {};

int     Parser::read(char   *config)
{
    std::fstream file(config, std::fstream::in);
    if  (file.good())
    {
        setConfigFile(file);
        std::string  line;
        while(std::getline( file, line ) )
        {
            while (line.length() == 0)
                getline(file, line); // skip blank lines
            line = trim(line);
            if ( line.compare(0, 6, "listen") == 0 )
                setPort(line);
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

// Getters and Setters

std::string     Parser::getIP() const                       {   return _ip;                 };
int             Parser::getPort() const                     {   return _port;               };
std::string     Parser::getProtocol() const                 {   return _protocol;           };
std::fstream    Parser::getConfigFile() const               {   return _config;             };

void            Parser::setPort(std::string const s)
{
    
    if (_port != 0)
        return ;
    std::stringstream tmp;
    std::string nums = "0123456789";
    size_t start = s.find_first_of(nums);
    size_t end = s.find_last_of(nums) + 1;
    _port = stoi(s.substr(start, end));
};

// private functions

int     Parser::valid_brackets(std::fstream f) // check if { } are well closed  
{
    std::vector<char>   brackets; 
    std::ostringstream  sstr;

    sstr << f.rdbuf();
    if (std::count(s.begin(), s.end(), '{') == std::count(s.begin(), s.end(), '}')
    {
        size_t len = sstr.length();
        for ( int i = 0; i < len; i++)
        {
            if ( sstr[i] == '{' || sstr[i] == '}' )
                brackets.push(sstr[i]);
        }
        while (brackets.size())
        {
            std::vector<char>::iterator it = brackets.begin();
            while (it != brackets.end())
            {
                if ( *it == '{' && *(it + 1) == '}')
                {
                    brackets.erase(it,it + 1);
                    std::cout << brackets;
                    it = brackets.begin();
                }
            }
        }
        if (brackets.size())
        {    std::cout << "PARSER KO\n"; return 0;}
        std::cout << "PARSER OK\n";
        return 1;
    }
    return 0;
}

// string manipulation functions

std::string     Parser::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    return s.substr(start, end);
}