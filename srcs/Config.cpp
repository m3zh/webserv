#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config()      {};
Config::~Config()     {};


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


void    Config::debug_final()
{
    std::vector<Server>::iterator it = _servers.begin();

    while (it != _servers.end())
    {
        std::cout << "server_name: " << it->getServerName() << ",\n"
                  << "  - port: " << it->getPort() << ",\n"
                  << "  - client_max:  " << it->getClientMaxBodySize() << ",\n";

        std::vector<page>::iterator it2 = it->getPages().begin();
        while (it2 != it->getPages().end())
        {
            std::cout << "  - location " << it2->location_path << ",\n"
                        << "        - " << it2->autoindex << ",\n";
                // if (it2->methods[0])
                // {

                //     std::cout   << "        - " << it2->methods[0] << ",\n";
                // }
            it2++;
        }
        it++;
    }
}

void    Config::setServerParams(Lexer &parser, Server &server, std::vector<Token>::iterator &it)
{
    while (it->getType() == "Key")
    {
        Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
        it++;
        while (it != parser.tokens.end() && it->getType() != "Key" && it->getType() != "Namespace")
        {
            if (key_tmp.getContent() == "listen")
            {
                server.setPort(stoi(it->getContent()));
            }
            else if (key_tmp.getContent() == "server_name")
                server.setServerName(it->getContent());
            else if (key_tmp.getContent() == "client_max_body_size")
                server.setClientMaxBodySize(stoi(it->getContent()));
            it++;
        }
    }
}

void    Config::setServerPageParams(Lexer &parser, Server &server, std::vector<Token>::iterator &it)
{
    while (it->getContent() == "location")
    {
        page    location;
        
        it++;
        location.location_path = it->getContent();
        it++;
        while (it->getType() == "Key")
        {
            Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
            it++;
            while (it != parser.tokens.end() && it->getType() != "Key" && it->getType() != "Namespace")
            {
                if (key_tmp.getContent() == "root")
                    {location.root = it->getContent(); it++; }
                else if (key_tmp.getContent() == "index")
                    {location.index = it->getContent(); it++; }
                else if (key_tmp.getContent() == "upload")
                    {location.upload_path = it->getContent(); it++; }
                else if (key_tmp.getContent() == "redirect")
                    {location.redirect = it->getContent(); it++; }
                else if (key_tmp.getContent() == "allowed_methods")
                {
                    while (it->getType() == "Method")
                    {
                        location.methods.push_back(it->getContent());
                        it++;
                    }
                }
                else
                    it++;
            }
        }
        server.getPages().push_back(location);
    }
}

int     Config::read(char   *config, char **envp)
{
    Lexer parser;

    if (parser.read(config, envp))
    {
        debug_me(parser);
        std::cout << std::endl;
        std::vector<Token>::iterator    it = parser.tokens.begin();

        // si la config est valide, on parcourt les tokens
        while (it != parser.tokens.end() && it->getContent() == "server")
        {
            // si le token est un server, on l'instancie
            Server  server;

            it++;
            setServerParams(parser, server, it);
            setServerPageParams(parser, server, it);
            setServers(server);
        }
    }
    debug_final();
    return 0;
}

void            Config::setServers(Server &server) { _servers.push_back(server); };

// private functions


