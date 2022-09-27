/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/12 20:39:03 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/26 19:39:09 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"
# include "../inc/Cgi.hpp"

int main(int argc, char **args, char **envp)
{
    Config  config;

    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    if (config.read(args[1], envp))
    {
        try {
            Webserv webserv(config.getServers());

            webserv.run_server();
            throw WebException<std::string>(RED, "WebServ error: shutting down...", "555");     // templated class, works as well with
                                                                                                // WebException<std::string>(RED, "something ain't right", "555");
        }
        catch (WebException<std::string>& e){    std::cout << e.what() << std::endl;   return EXIT_FAILURE;   };      
    }
    return EXIT_SUCCESS;
}
