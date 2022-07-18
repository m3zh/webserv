#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config ():      _port(0) {};
Config::~Config()  {};


void    Config::debug_me(Lexer &parser)
{
        for (auto it = std::begin (parser.tokens); it != std::end (parser.tokens); ++it) {
        std::cout  << "type= "  << it->getType() << "; pos= ";
        std::cout  << it->getPos() << "; content= ";
        std::cout  << it->getContent();
        if (it->getType() == "Key")
            std::cout << "; aw= " << it->getAllowedWords();
        std::cout << std::endl;
        }
}
int     Config::read(char   *config)
{
    Lexer parser;

    if (parser.read(config))
    {
        debug_me(parser);
        std::cout << "HERE 1\n";

        return 1;
    }
    std::cout << "HERE 0\n";
    debug_me(parser);
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


