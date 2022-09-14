/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testing_features_artmende.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/08 17:24:29 by artmende          #+#    #+#             */
/*   Updated: 2022/09/14 11:31:18 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring> // for memset

#include <sys/socket.h> // for socket(), bind and listen
#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> // for htons and inet_addr
#include <sys/select.h> // for select()
#include <fcntl.h>

#include <unistd.h> // temp for close

#define PORT 19042
#define BACKLOG 255

int	main()
{
	// 1. Create a socket. // = socket(AF_INET, SOCK_STREAM, 0)
	// 2. Fill the struct sockaddr_in with the chosen port
	// 3. Bind and listen
	// 4. Enter in the main loop
	// 5. That's where we need to select. 
			// 2 options here :	1. there is something to read on the server socket --> call accept on this socket --> Add the new created socket to the SET
							//	2. there is something to read on another socket (that is the result of accept) --> go in read / write loop
	// 6. recv from that new socket we got 
	// 7. send to this new socket

	int	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in	my_sockaddr;
	memset(&my_sockaddr, 0, sizeof(my_sockaddr));
	my_sockaddr.sin_family = AF_INET;
	my_sockaddr.sin_port = htons(PORT);
	my_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(server_sock, (struct sockaddr *)&my_sockaddr, sizeof(my_sockaddr));
	listen(server_sock, BACKLOG);

	// define the 2 sets to be used with select and add the server_sock to it
	fd_set	socket_pool, socket_ready;
	FD_ZERO(&socket_pool);
	FD_SET(server_sock, &socket_pool);

	while (true)
	{
		std::cout << "Start of the main loop\n";
		socket_ready = socket_pool;
		select(FD_SETSIZE, &socket_ready, NULL, NULL, NULL);
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &socket_ready))
			{
				if (i == server_sock)
				{
					std::cout << "About to accept\n";
					// we need to accept this new connection
					int	new_socket = accept(server_sock, NULL, NULL); // probably NULL is not ok, and we have to store address somewhere
					//fcntl(i, F_SETFL, O_NONBLOCK);
					FD_SET(new_socket, &socket_pool);
				}
				else
				{
					char	buffer[5000];
					memset(buffer, 0, sizeof(buffer));
					recv(i, buffer, sizeof(buffer), 0);
					std::cout << "data received : \n" << buffer << std::endl;
					send(i, "HTTP/1.1 200 OK\r\n\r\nYOPPP", 24, 0);
					close(i); ///////
					FD_CLR(i, &socket_pool);
					// that's where we receive data, and then respond
				}
			}
		}
	}

	return 0;
}
