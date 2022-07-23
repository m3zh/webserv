#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config()      {};
Config::~Config()     {};


// void    Config::debug_me(Lexer &parser)
// {
//         for (auto it = std::begin (parser.tokens); it != std::end (parser.tokens); ++it) {
//         std::cout  << "type= "  << it->getType() << "; pos= ";
//         std::cout  << it->getPos() << "; content= ";
//         std::cout  << it->getContent();
//         if (it->getType() == "Key")
//             std::cout << "; aw= " << it->getAllowedWords();
//         std::cout << std::endl;
//         }
// }

int     Config::read(char   *config, char **envp)
{
    Lexer parser;

    if (parser.read(config, envp))
    {
        // debug_me(parser);
        setServers(_servers);
    }
    // debug_me(parser);
    return 0;
}

// Getters and Setters

// std::string     Config::getIP()         const     {   return _ip;         };
// int             Config::getPort()       const     {   return _port;       };
// std::string     Config::getProtocol()   const     {   return _protocol;   };

void            Config::setPort(Server s, std::string const str)
{
    (void)s;
    (void)str;
    // std::stringstream tmp;
    // std::string nums = "0123456789";
    // size_t start = s.find_first_of(nums);
    // size_t end = s.find_last_of(nums) + 1;
    // s->port = stoi(str.substr(start, end));
}

void            Config::setServers(std::vector<Server> s) { (void)s; };

// private functions


