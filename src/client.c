#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 4242

#include "crypt.h" // to encrypt and decrypt messages

/** @brief Source & Implementation of client for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

int main(void)
{
	// Creating socket file descriptor - communication domain IPv4, TCP communication, Protocol value for Internet Protocol (IP)
	int sock = socket(AF_INET, SOCK_STREAM, 0) ;
	if(sock == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue creating initial socket") ;
		return -1 ;
	}

	struct sockaddr_in serv_addr ;  
	serv_addr.sin_family = AF_INET ; // again, IPv4
	serv_addr.sin_port = htons(PORT) ; // forcefully attaching socket to the port 4242
       
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) == -1) 
	{
		fprintf(stderr, "Error: %s\n", "Invalid address / address not supported") ;
		return -2 ;
	}

	// connect() connects the socket of fd to the address specified by sockaddr struct
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue connecting to server socket") ;
		return -3 ;
	}
	
	while(1)
 	{
		char buffer[1024] = {'\0'} ; // declare&initialise buffer
		fprintf(stdout, "%s", "Input < ") ; fflush(stdout) ; // inline input prompt (&manual flush)
 		read(1, buffer, 1023) ; // read syscall to get user input from stdin
 		// encrypt input
 		if(send(sock, buffer, strlen(buffer), 0) == -1) // send inputted message over socket, break if server hangs / shuts down
 		{
 			fprintf(stderr, "Error: %s\n", "Server unexpectedly closed") ;
			break ;
    		}
    		//if(read(sock, buffer, 1024) == -1) break ; // if EOF
    		// else decrypt then print
    		//fprintf(stdout, "reply: %s\n", buffer) ; // print server reply
    	}

	// E(nd) O(f) P(rogram)
	close(sock) ;
	return 0 ;
}
