# pragma once

# include <iostream>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>
# include <map>

# include "Lexer.hpp"

class Lexer;

// ****************************
// the Config class stocks all information parsed by Lexer
// in a readable way for the Webserv class
// ****************************

class Config
{
    private:
        std::string     _ip;        // pourquoi l'ip ici ?
        int             _port;
        std::string     _protocol;

        void            debug_me(Lexer &parser);

        Config(Config const &p);
        Config& operator=(Config const &p);
        
    public:
        Config();
        ~Config();

        std::string     getIP() const;
        int             getPort() const;
        std::string     getProtocol() const;
        void            setPort(std::string const str);
        int             read(char   *config);
};
