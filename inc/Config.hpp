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
    std::string                 autoindex;
    int                         error;
};

class Server 
{
    public:
        Server() {};
        ~Server() {};

        std::string             getServerName() { return _server_name; };
        void                    setServerName(std::string s) { _server_name = s; };
        int                     getPort() { return _port; };
        void                    setPort(int p) { _port = p; };
        int                     getClientMaxBodySize() { return _client_max_body_size; };
        void                    setClientMaxBodySize(int c) { _client_max_body_size = c; };
        std::vector<page>       getPages() { return _pages; };
        void                    setPages(page p) { _pages.push_back(p); };

    private:
        std::string                 _server_name;
        int                         _port;
        int                         _client_max_body_size;
        std::vector<page>           _pages;
        page                        _error_page;
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

        void            setServers(Server &s);
        int             read(char   *config, char **envp);
        void            debug_final();
        void            setServerParams(Lexer &parser, Server &server, std::vector<Token>::iterator &it);
        void            setServerPageParams(Lexer &parser, Server &server, std::vector<Token>::iterator &it);

};
