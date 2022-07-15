#  include "../inc/Config.hpp"

Config::Config ():      _port(0) {};

Config::~Config()  {};

int     Config::read(char   *config)
{
    (void)config;
    return 1;
}

// Getters and Setters

std::string     Config::getIP()         const     {   return _ip;         };
int             Config::getPort()       const     {   return _port;       };
std::string     Config::getProtocol()   const     {   return _protocol;   };

void            Config::setPort(std::string const s)
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

int     Config::valid_brackets(std::fstream &f) // check if { } are well closed  
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

// string manipulation functions

std::string     Config::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    return s.substr(start, end);
}

