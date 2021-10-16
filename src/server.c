#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // (p)threads

#define PORT 4242

#include "client_manager.h" // functionality to manage clients
#include "crypt.h" // encryption related functionality

/** @brief Source & Implementation of multithreaded server for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

int main(void)
{
	/* Initialisation part */
	// set up socket, attach to location, set up as initial communication line

	// Creating socket file descriptor - communication domain IPv4, TCP communication, Protocol value for Internet Protocol (IP)
	int server_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if(server_fd == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue creating initial socket") ;
		return -1 ;
	}
	
	/*
	// Forcefully attaching socket to the port 8080
	int opt = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		// edit so it starts cycling through available ports and prints out determined ports in a do-while loop
		fprintf(stderr, "Error: %s\n", "Issue connecting to port") ;
		return -2 ;
	}
	*/
	
	// Fill in struct and binds the socket to the address and port number specified in sockaddr struct
	struct sockaddr_in address ;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET ; // again, IPv4
	address.sin_addr.s_addr = INADDR_ANY ; // localhost
	address.sin_port = htons(PORT) ; // forcefully attaching socket to the port 4242

	// 'associate' the socket file descriptor to a name (IP address and port)
	if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue establishing initial address point") ;
		return -2 ;
	}

	// listen function marks  the  socket referred to by sockfd as a passive socket (ie as a socket that will be used to 
	// select & accept incoming connection requests
	if(listen(server_fd, 3) == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue establishing listener to current port") ;
		return -3 ;
	}

	/* Client hosting part */
	// uses multithreading pthreads, because I cba to make sure I use c11
	
	ClientStore client_store ; // now that server is basically set up, create functionality to store client details
	client_store_init(&client_store) ;
	
	// now create dedicated server-client socket, connecting to user who reached passive socket first
	#define MAX_THREADS 2
	pthread_t thread_ids[MAX_THREADS] ;
	size_t i = 0 ;
	for( ; i < MAX_THREADS ; ++i) // people that connect @ once == max threads
	{
		int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen) ;
		if(new_socket == -1)
		{
			fprintf(stderr, "Error: %s\n", "Issue creating client-server dedicated socket!") ;
			break ;
		}

		add_client(new_socket, get_client_address(new_socket), &client_store) ;
		Client* client = client_store.clients + (client_store.client_count-1) ;
		pthread_create(thread_ids+i, NULL, &client_connection, (void*)client) ;
	}
	
	for(size_t j = 0 ; j <= i ; ++j)
	{
		pthread_join(thread_ids[j], NULL) ; // do not allow server to die till last client disconnects
	}

	// E(nd) O(f) P(rogram)
	close(server_fd) ;
	return 0 ;
}

