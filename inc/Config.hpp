/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/15 09:13:06 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/06 08:48:32 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
// the Config class stores all information parsed by Lexer
// in a readable way for the Webserv class
// ****************************


class Config
{
    private:
        std::vector<ServerInfo>    _servers;
        
        void                    debug_me(Lexer &parser);

        Config(Config const &p);
        Config& operator=(Config const &p);
        
    public:
        Config();
        ~Config();

        
        int             is_valid(char   *config, char **envp);
        
        void            setServers(ServerInfo &s);
        void            setServerParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it);
        void            setServerPageParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it);

        std::vector<ServerInfo>&     getServers();

        bool            valid_config(std::vector<ServerInfo>    &s);
};
