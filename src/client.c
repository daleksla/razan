#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>

#define PORT 4242

#include "key_sharing.h" // to encrypt and decrypt messages
#include "aes.h"
#include "client_chat.h"

/** @brief Source & Implementation of client for razan terminal messaging **/

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
 
 	// determine symmetric key & set up encryption (tiny-AES-c)
   	read(sock, buffer, sizeof(struct two_keys)) ;
 	const struct two_keys public_keys = {
 	    ((struct two_keys*)(buffer))->key_a,
 	    ((struct two_keys*)(buffer))->key_b
 	} ;
	
  	const struct two_keys secret_keys = generate_secret_keys(&public_keys) ;
	send(sock, &(secret_keys.key_b), sizeof(unsigned long int), 0) ; // send mashed key to other side
	
	unsigned long int her_mashed_key ;
	read(sock, &her_mashed_key, sizeof(unsigned long int)) ; // read in other clients mashed key

	const unsigned long int KEY = generate_symmetric_key(her_mashed_key, secret_keys.key_a, public_keys.key_a) ; // actual key
	
	struct AES_ctx ctx ;
	AES_init_ctx(&ctx, (const uint8_t*)&KEY) ;

    fprintf(stdout, "LOG: Launching chat window\n") ;
    client_chat(sock, &ctx) ;
    fprintf(stdout, "LOG: Chat closed. Ending connecting and terminating\n") ;
    
	// E(nd) O(f) P(rogram)
	close(sock) ;
	return 0 ;
}
