/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmasse <vmasse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 18:49:57 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/13 17:03:51 by vmasse           ###   ########.fr       */
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

class Config
{
    private:
        std::string     _ip; // pourquoi l'ip ici ?
        int             _port;
        std::string     _protocol;

        std::string     trim(std::string s);
        int             valid_brackets(std::fstream &f);

        Config(Config const &p);
        Config& operator=(Config const &p);

    public:
        Config();
        ~Config();

        std::string     getIP() const;
        int             getPort() const;
        std::string     getProtocol() const;
        void            setPort(std::string const str);

        int read(char   *config);

};
