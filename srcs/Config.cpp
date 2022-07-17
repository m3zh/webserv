#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config ():      _port(0) {};
Config::~Config()  {};

int     Config::read(char   *config)
{
    Lexer parser;

    if (parser.read(config))
    {
        // set stuff
        return 1;
    }
    return 0;
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



