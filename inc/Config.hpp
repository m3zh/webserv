# pragma once

# include <iostream>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>
# include <map>

# include "Lexer.hpp"
# include "ServerInfo.hpp"

class Lexer;
class ServerInfo;

// ****************************
// the Config class stocks all information parsed by Lexer
// in a readable way for the Webserv class
// ****************************


class Config
{
    public:
        std::vector<ServerInfo>    _servers;
    private:

        void                    debug_me(Lexer &parser);

        Config(Config const &p);
        Config& operator=(Config const &p);
        
    public:
        Config();
        ~Config();

        
        int             read(char   *config, char **envp);
        
        void            setServers(ServerInfo &s);
        void            setServerParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it);
        void            setServerPageParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it);

        std::vector<ServerInfo>&     getServers();

        bool            valid_config(std::vector<ServerInfo>    &s);
};
