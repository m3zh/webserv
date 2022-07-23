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

void    Config::debug_final()
{
    std::vector<Server>::iterator it = _servers.begin();

    while (it != _servers.end())
    {
        std::cout << "server_name : " << it->getServerName() << " ,\n"
                  << "  - port : " << it->getPort() << " ,\n"
                  << "  - client_max : " << it->getClientMaxBodySize() << " ,\n";

        // std::vector<page>::iterator it2 = it->pages.begin();
        // while (it2 != it->pages.end())
        // {
        //     it2++;
        // }
        it++;
    }
}

int     Config::read(char   *config, char **envp)
{
    Lexer parser;

    if (parser.read(config, envp))
    {
        // debug_me(parser);
        std::vector<Server>             servers;
        std::vector<Token>::iterator    it = parser.tokens.begin();

        // si la config est valide, on parcourt les tokens
        while (it != parser.tokens.end())
        {
            // si le token est un server, on l'instancie
            if (it->getType() == "Namespace"
                && it->getContent() == "server"
                && ++it != parser.tokens.end())
            {
                Server  server;

                // puis on itère tant qu'on a pas un autre token server pour ajouter les variables du server courant
                while (it != parser.tokens.end()
                        && it->getType() != "server")
                {
                    // on checke si c'est une clef, puis combien de mots cette clef autorise; 
                    if (it->getType() == "Key"
                        && (it + 1) != parser.tokens.end())
                    {
                        Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
                        it++;
                        //  on ajoute la valeur ou les valeurs a la config et on itère sur le nb de mots autorisés
                        for (size_t i = 0; i < it->getAllowedWords(); i++, it++)
                        {
                            if (key_tmp.getContent() == "listen")
                                server.setPort(stoi(it->getContent()));
                            if (key_tmp.getContent() == "server_name")
                                server.setServerName(it->getContent());
                            if (key_tmp.getContent() == "client_max_body_size")
                                server.setClientMaxBodySize(stoi(it->getContent()));
                        }
                    }
                    it++;
                }
                _servers.push_back(server);
            }
            it++;
        }
        setServers(servers);
    }
    debug_final();
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

void            Config::setServers(std::vector<Server> s) { _servers = s; }

// private functions


