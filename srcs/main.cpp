/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/12 20:39:03 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 17:36:59 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"
# include "../inc/Cgi.hpp"
# include "../inc/Config.hpp"
# include "../inc/ServerInfo.hpp"

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
        Webserv webserv(config.getServers());
        Cgi     CGI(envp);

        std::fstream file2test;
        file2test.open("/home/user42/webserv/pages/website1/upload.html", std::fstream::in | std::fstream::out | std::fstream::app);
        std::string html_content;
        std::string line;
        while (getline(file2test, line))
            html_content.append(line);
        file2test.close();
        std::cout << "Webserv class starts here\n";
        if (webserv.isCGI_request(html_content))
        {    std::cout << "Ready4CGI\n";    }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
