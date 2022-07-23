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

struct page 
{
    std::string                 location_path;
    std::string                 root;
    std::string                 index;
    std::string                 upload_path;
    std::string                 redirect;
    std::vector<std::string>    methods;
    int                         autoindex;
    int                         error;
};

class Server 
{
    public:
        Server() {};
        ~Server() {};

        std::string                 server_name;
        int                         port;
        int                         client_max_body_size;
        std::vector<page>           pages;
        page                        error_page;
};

class Config
{
    private:
        std::vector<Server>   _servers;

        void            debug_me(Lexer &parser);
        Config(Config const &p);
        Config& operator=(Config const &p);
        
    public:
        Config();
        ~Config();

        void            setServers(std::vector<Server> s);
        void            setPort(Server s, std::string const str);
        int             read(char   *config, char **envp);
};
