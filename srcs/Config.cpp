#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config()      {};
Config::~Config()     {};

int     Config::read(char   *config, char **envp)
{
    Lexer parser;

    if (parser.read(config, envp))
    {
        // debug_me(parser);
        std::vector<Token>::iterator    it = parser.tokens.begin();

        // si la config est valide, on parcourt les tokens
        while (it != parser.tokens.end() && it->getContent() == "server")
        {
            // si le token est un server, on l'instancie
            ServerInfo  server;

            it++;
            setServerParams(parser, server, it);
            setServerPageParams(parser, server, it);
            setServers(server);
        }
        if (valid_config(getServers()))
            return 1;
    }
    std::cout << "Error in config\n"; 
    // debug_me(parser);
    return 0;
}

// ************
// SETTER functions
// ************

void    Config::setServers(ServerInfo &server) { _servers.push_back(server); };

void    Config::setServerParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getType() == "Key")
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

void    Config::setServerPageParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getContent() == "location")
    {
        page    location;
        
        it++;
        location.location_path = it->getContent();
        it++;
        while (it != parser.tokens.end() && it->getType() == "Key")
        {
            Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
            it++;
            while (it != parser.tokens.end() && it->getType() != "Key" && it->getType() != "Namespace")
            {
                if (key_tmp.getContent() == "root")
                    location.root = it->getContent();
                else if (key_tmp.getContent() == "index")
                    location.index = it->getContent();
                else if (key_tmp.getContent() == "upload")
                    location.upload_path = it->getContent();
                else if (key_tmp.getContent() == "redirect")
                    location.redirect = it->getContent();
                 else if (key_tmp.getContent() == "autoindex")
                    location.autoindex = it->getContent();
                else if (key_tmp.getContent() == "allowed_methods")
                {
                    while (it != parser.tokens.end() && it->getType() == "Method")
                    {
                        location.methods.push_back(it->getContent());
                        it++;
                    }
                    break ;
                }
                it++;
            }
        }
        server.setPages(location);
    }
}

// ************
// GETTER functions
// ************

std::vector<ServerInfo>&     Config::getServers()        {   return _servers;    };

// ************
// VALIDATE functions
// ************

bool                    Config::valid_config(std::vector<ServerInfo>& s)
{   
    std::vector<ServerInfo>::iterator it = s.begin();

    while ( it != s.end() - 1)
    {
        int root = 0;
        if ((*it).getPort() < 0) return false;                                  // check if port has been set
        std::vector<page>::iterator pit = (*it).getPages().begin();                 
        while ( pit != (*it).getPages().end() )
        {    if ((*pit).location_path.compare("/") == 0)                         // check if there is ONE root location /
                root++;
            pit++;
        }
        std::vector<ServerInfo>::iterator it2 = it + 1;
        while ( it2 != s.end()) 
        {
            if ((*it).getServerName().compare((*it2).getServerName()) == 0
            && (*it).getPort() == (*it2).getPort())
                return false;
            it2++;
        }
        if (root != 1)
            return false;
        it++;
    }
    int root = 0;
    std::vector<page>::iterator pit = (*it).getPages().begin();                 
    while ( pit != (*it).getPages().end() )
    {           
        if ((*pit).location_path.compare("/") == 0)                         // check if there is ONE root location /
            root++;
        pit++;
    }
    if (root != 1)
        return false;
    return true;
};

// ************
// DEBUGGING
// ************

void    Config::debug_me(Lexer &parser)
{
    std::cout << "############### PARSER ###############\n";
    for (auto it = std::begin (parser.tokens); it != std::end (parser.tokens); ++it) {
    std::cout  << "type= "  << it->getType() << "; pos= ";
    std::cout  << it->getPos() << "; content= ";
    std::cout  << it->getContent();
    if (it->getType() == "Key")
        std::cout << "; aw= " << it->getAllowedWords();
    std::cout << std::endl;
    }

    std::cout << "############### SERVERS ###############\n";
    std::vector<ServerInfo>::iterator it = getServers().begin();

    while (it != getServers().end())
    {
        std::cout << "server_name: " << (*it).getServerName() << ",\n"
                << "  - port: " << (*it).getPort() << ",\n"
                << "  - client_max:  " << (*it).getClientMaxBodySize() << ",\n"
                    << "    - location " << (*it).getPages()[0].location_path << "\n"
                    << "        - autoindex: " << (*it).getPages()[0].autoindex << "\n";

        std::cout << "size pages : " << (*it).getPages().size() << "\n";
        std::vector<page>::iterator it2 = (*it).getPages().begin();
        while (it2 != (*it).getPages().end())
        {
            std::cout << getServers().size() << "\n";
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