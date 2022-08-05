/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ablondel <ablondel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/03 09:29:52 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 13:59:21 by ablondel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <vector>

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
        int                         is_cgi;
};

class ServerInfo 
{
    public:
        ServerInfo();
        ~ServerInfo();

        std::string                 getServerName();             
        void                        setServerName(std::string s);
        int                         getPort();      
        void                        setPort(int p);             
        int                         getClientMaxBodySize();      
        void                        setClientMaxBodySize(int c);  
        std::vector<page>&          getPages();             
        void                        setPages(page p);            

    private:
        std::string                 _server_name;
        int                         _port;
        int                         _client_max_body_size;
        std::vector<page>           _pages;
        page                        _error_page;
};
