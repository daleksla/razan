#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 4242

#include "key_sharing.h" // to encrypt and decrypt messages
#include "aes.h"

/** @brief Source & Implementation of client for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

int main(void)
{
	fprintf(stdout, "LOG: %s\n", "Launching razan client") ;
	/* Initialisation part */
	// set up socket, attach to location, attempt connection
	
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

	/* Communication part */
	fprintf(stdout, "LOG: %s\n", "razan client connected to server") ;
 	char buffer[1024] = {'\0'} ; // declare&initialise buffer
 
 	// determine symmetric key
   	read(sock, buffer, sizeof(two_keys)) ;
 	const two_keys public_keys = {((two_keys*)(buffer))->key_a, ((two_keys*)(buffer))->key_b} ;
	
  	const two_keys secret_keys = generate_secret_keys(&public_keys) ;
	send(sock, &(secret_keys.key_b), sizeof(long long int), 0) ; // send mashed key to other side
	
	long long int her_mashed_key ;
	read(sock, &her_mashed_key, sizeof(long long int)) ; // read in other clients mashed key

	const long long int key = generate_symmetric_key(her_mashed_key, secret_keys.key_a, public_keys.key_a) ; // actual key
	
	// use tiny-AES to set up encryption
	struct AES_ctx ctx ;
	AES_init_ctx(&ctx, &key) ;
	
	while(1)
 	{
 		memset(buffer, '\0', 1024) ; // declare&initialise buffer
		fprintf(stdout, "%s", "Input < ") ; fflush(stdout) ; // inline input prompt (&manual flush)
 		read(1, buffer, 1023) ; // read syscall to get user input from stdin
 		AES_ECB_encrypt(&ctx, buffer) ; // encrypt input
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
