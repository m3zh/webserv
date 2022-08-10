/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/03 09:29:52 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 15:25:56 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <vector>

struct page 
{
        std::string                 location_path;
        std::string                 root;
        std::string                 upload_store;
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
        
        std::string                 getServerRoot();             
        void                        setServerRoot(std::string s);

        std::string                 getServerIndex();             
        void                        setServerIndex(std::string s);

        int                         getPort();      
        void                        setPort(int p);

        int                         getClientMaxBodySize();      
        void                        setClientMaxBodySize(int c);  

        std::vector<page>&          getPages();             
        void                        setPages(page p);            

    private:
        std::string                 _server_name;
        std::string                 _server_root;
        std::string                 _server_index;
        int                         _port;
        int                         _client_max_body_size;
        std::vector<page>           _pages;
        page                        _error_page;
};