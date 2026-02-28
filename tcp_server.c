#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/select.h>
#include <time.h>

int main(void)
{
	struct addrinfo hints; // make our "hints" for server. It creates block hints in addrinfo structure.
	memset(&hints, 0, sizeof(hints)); // initzialize hints with zeros unless we don't want to fill it with garbage values for examples(543543) and etc
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	// addrinfo hints 
	// {
	//	int ai_flags;
	//	int ai_family;
	//	int ai_socktype;
	//	int ai_protocol;
	//	char *ai_canonname;
	//	struct sockaddr * ai_addr
	//	struct addrinfo * ai_next
	// }
	
	struct addrinfo *peer_address; // create pointer to addrinfo structure
	getaddrinfo(0, "8080", &hints, &peer_address); // 8080 is port of the server you can chose other
	// use getaddrnifo() to fill structure (addrinfo) with new values
	printf("Creating socket...\n");
	int socket_s = socket((*peer_address).ai_family, (*peer_address).ai_socktype,0); // socket() function read more in documentation sys/socket.h
	printf("Completed!\n");
	printf("Binding...\n");
	int bind_s = bind(socket_s, (*peer_address).ai_addr, (*peer_address).ai_addrlen); // bind ip address to server 
	freeaddrinfo(peer_address); // we don't need pointer to our addrinfo anymore and we should to free space
	printf("Completed!\n");
	printf("Listening...\n");
	int server_l = listen(socket_s, 10); // 10 is maximum of users queque
	printf("Completed!\n");
	
	int client_socket = -1; // initzialize client_socket as global variable and set -1 because we don't need garbage values in this variable

	while(1)
	{
		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(socket_s, &read_fds);
		if(client_socket!=-1) FD_SET(client_socket, &read_fds);
		int max_fd = (client_socket > socket_s) ? client_socket : socket_s;
		if(select(max_fd+1, &read_fds, NULL, NULL, NULL)<0) break;
		if(FD_ISSET(socket_s, &read_fds))
		{
			struct sockaddr_storage client_addr; // initzialize block client_addr in sockaddr_storage for client data
			socklen_t addr_len = sizeof(client_addr);
			client_socket = accept(socket_s, (struct sockaddr*)&client_addr, &addr_len);
			char buffer[100];
			getnameinfo((struct sockaddr*)&client_addr, addr_len, buffer, sizeof(buffer), 0,0,NI_NUMERICHOST);
			printf("Connection from: %s\n", buffer);
		} else if(client_socket != -1 && FD_ISSET(client_socket, &read_fds))
			{
				char recv_buffer[1024]; 
				int bytes_received = recv(client_socket, recv_buffer, 1024, 0);
				if(bytes_received<1)
					{
						FD_CLR(socket_s, &read_fds);
						close(socket_s);
						continue;
					} else 
		      				{
							char request[1024];
							const char *response = "HTTP/1.1 200 OK\r\nContent-Lenght: 5\r\n\r\nHello";
							int server_send = send(client_socket, response, 1024, 0);
							printf("%d\n", bytes_received);
							close(client_socket);
							client_socket = -1;
			
						}

			}
	}


 
}



