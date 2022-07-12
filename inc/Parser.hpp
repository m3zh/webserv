/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmasse <vmasse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 18:49:57 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/12 15:49:02 by vmasse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <stdlib.h> 
# include <fstream>
# include <sstream>
# include <cstring>
# include <vector>

class Parser
{
    private:
        std::string     _ip;
        int             _port;
        std::string     _protocol;

        std::string     trim(std::string s);
        int             valid_brackets(std::fstream &f);

        Parser(Parser const &p);
        Parser& operator=(Parser const &p);
        
    public:
        Parser();
        ~Parser();

        int read(char   *config);
        std::string     getIP() const;
        int             getPort() const;
        std::string     getProtocol() const;
        void            setPort(std::string const str);
};