/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 18:49:57 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/13 19:54:53 by mlazzare         ###   ########.fr       */
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
