/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testing_features_artmende.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/08 17:24:29 by artmende          #+#    #+#             */
/*   Updated: 2022/09/09 11:52:47 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring> // for memset

#include <sys/socket.h> // for socket(), bind and listen
#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> // for htons and inet_addr

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

	while (true)
	{
		
	}

	return 0;
}
