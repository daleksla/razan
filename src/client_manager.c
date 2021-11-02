#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // (p)threads
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "key_sharing.h"
#include "client_manager.h" // functionality to manage client information
#include "aes.h"

/** @brief Implementation of functionality related to storing client details & their communications **/

void client_store_init(struct ClientStore* cs)
{
	cs->size = 3 ;
	cs->clients = malloc(sizeof(struct Client) * cs->size) ; // list of max 256 char pointers
	assert(cs->clients) ;
	for(size_t i = 0 ; i < cs->size ; ++i)
	{
		(cs->clients+i)->socket = -1 ; // allows us to identify whi
		(cs->clients+i)->address = NULL ; // allows us to identify whi
	}
}

struct Client* add_client(const int sk, const char* ad, struct ClientStore* cs)
{
	pthread_mutex_lock(&cs->mutex) ;
	struct Client* tmp = find_client_gap(cs) ;
	if(!tmp)
	{
		cs->size *= 2 ;
		cs->clients = realloc(cs->clients, cs->size) ;
		assert(cs->clients) ;
		tmp = cs->clients + (cs->size / 2) ;
	}
	tmp->socket = sk ;
	tmp->address = ad ;
	pthread_mutex_unlock(&cs->mutex) ;
	return tmp ;
}

struct Client* find_client_gap(struct ClientStore* cs)
{
	pthread_mutex_lock(&cs->mutex) ;
	for(size_t i = 0 ; i < cs->size ; ++i)
	{
		struct Client* cl = cs->clients+i ;
		if(cl->socket == -1)
		{
			return cl ;
		}
	}
	pthread_mutex_unlock(&cs->mutex) ;
	return NULL ;
}

void close_client(struct Client* cl)
{
	close(cl->socket) ;
	cl->socket = -1 ;
	cl->address = NULL ;
}

const char* get_client_address(const int socket) 
{
	struct sockaddr_in addr ; socklen_t addr_size = sizeof(struct sockaddr_in) ;
	getpeername(socket, (struct sockaddr*)&addr, &addr_size) ;
	return inet_ntoa(addr.sin_addr) ;
}

void* client_connection(void* v_client)
{
	struct Client* client = (struct Client*)v_client ;
	fprintf(stdout, "LOG: Dedicated connection for client on %s, via socket %d, is online\n", client->address, client->socket) ;

	const struct two_keys public_keys = generate_public_keys() ;
	send(client->socket, &public_keys, sizeof(struct two_keys), 0) ; // send pointer of public keys
	
	const struct two_keys secret_keys = generate_secret_keys(&public_keys) ;
	send(client->socket, &(secret_keys.key_b), sizeof(unsigned long int), 0) ; // send mashed key to other side		
	unsigned long int her_mashed_key ;
	read(client->socket, &her_mashed_key, sizeof(unsigned long int)) ; // read in other clients mashed key
		
	const unsigned long int KEY = generate_symmetric_key(her_mashed_key, secret_keys.key_a, public_keys.key_a) ; // actual key

	struct AES_ctx ctx ;
	AES_init_ctx(&ctx, (const uint8_t*)&KEY) ;
	
	// validate input
	
	// begin messaging
	while(1)
 	{
		char buffer[1024] = {'\0'} ; // declare&initialise buffer
    	if(!read(client->socket, buffer, 1023)) break ;
		AES_ECB_decrypt(&ctx, (uint8_t*)buffer) ;
    	fprintf(stdout, "LOG: message from address %s, via socket %d: %s\n", "localhost", client->socket, buffer) ;
		AES_ECB_encrypt(&ctx, (uint8_t*)buffer) ;
    	if(send(client->socket, buffer, strlen(buffer), 0) != strlen(buffer)) break ;
    }
    
	fprintf(stdout, "LOG: Freeing client on %s, socket %d\n", client->address, client->socket) ;
    close_client(client) ;
    return v_client ;
}

void client_store_fini(struct ClientStore* cs)
{
	for(size_t i = 0 ; i < cs->size ; ++i)
	{
		struct Client* cl = cs->clients+i ;
		if(cl->socket == -1)
		{
			close_client(cl) ;
		}
	}
	free(cs->clients) ; // list of max 256 char pointers
	cs->clients = NULL ;
	cs->size = 0 ;
}
