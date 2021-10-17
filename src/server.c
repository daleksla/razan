#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // (p)threads
#include <signal.h>

#define PORT 4242

#include "client_manager.h" // functionality to manage clients
#include "crypt.h" // encryption related functionality

/** @brief Source & Implementation of multithreaded server for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

/** @brief main entry function for server
  * @return int showing execution status (non-zero = error, zero = success) **/
int main(void) ;

int main(void)
{
	fprintf(stdout, "LOG: %s\n", "Launching razan server") ;
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
	fprintf(stdout, "LOG: %s\n", "razan server online and awaiting clients") ;
	
	ClientStore client_store ; // now that server is basically set up, create functionality to store client details
	client_store_init(&client_store) ;
	
	// now create dedicated server-client socket, connecting to user who reached passive socket first
	#define MAX_THREADS 2
	pthread_t thread_ids[MAX_THREADS] ;
	size_t i = 0 ;
	while(1) // people that connect @ once == max threads
	{
		int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen) ;
		if(new_socket == -1)
		{
			fprintf(stderr, "Error: %s\n", "Issue creating client-server dedicated socket!") ;
			break ;
		}

		Client* client = add_client(new_socket, get_client_address(new_socket), &client_store) ;
		pthread_create(thread_ids+i, NULL, &client_connection, (void*)client) ;
	}
	
	//for(size_t j = 0 ; j <= i ; ++j) // find a way to keep track of which threads exit
	//{
	//	pthread_join(thread_ids[i], NULL) ; // do not allow server to die till last client disconnects
	//}

	// E(nd) O(f) P(rogram) (need to find a way to force program to reach this point upon SIGINT w/o globals
	// not all *nix operating systems have a nice resource cleanuo like linux
	fprintf(stdout, "LOG: %s\n", "Disconnecting clients from server") ;
	client_store_fini(&client_store) ; // close all client sockets, free memory
	fprintf(stdout, "LOG: %s\n", "Closing server, goodbye") ;
	close(server_fd) ;
	return 0 ;
}

