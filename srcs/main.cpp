/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ablondel <ablondel@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/12 20:39:03 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/20 12:14:31 by ablondel         ###   ########.fr       */
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
        // Cgi     CGI;

        // std::fstream file2test;
        // std::string pwd = getenv("PWD");
        // file2test.open(pwd + "/pages/website1/hello.html", std::fstream::in | std::fstream::out | std::fstream::app);
        // std::string html_content;
        // std::string line;
        // file2test.seekg(0); // only for mac
        // while (getline(file2test, line))
        //     html_content.append(line);
        // file2test.close();
        // std::cout << "Webserv class starts here\n";
        // if (CGI.isCGI_request(html_content))
        // {    std::cout << "Ready4CGI\n";  CGI.exec_CGI(CGI.get_CGIrequest());       };

        try {
            Webserv webserv(config.getServers());
            webserv.run_server();
            throw WebException<std::string>(RED, "something ain't right", "555");              // templated class, works as well with
                                                                                  // WebException<std::string>(RED, "something ain't right", "555");
        }
        catch (WebException<std::string>& e){    std::cout << e.what() << std::endl;      };      
        // return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
