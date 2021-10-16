#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // (p)threads
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "client_manager.h" // functionality to manage client information

/** @brief Implementation of functionality related to storing client details & their communications
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

void client_store_init(ClientStore* cs)
{
	cs->size_ = 3 ;
	cs->clients = malloc(sizeof(Client) * cs->size_) ; // list of max 256 char pointers
	assert(cs->clients) ;
	cs->client_count = 0 ;
}

void add_client(const int sk, const char* ad, ClientStore* cs)
{
	//pthread_mutex_lock(&cs->mutex) ;
	if(cs->client_count == cs->size_)
	{
		cs->size_ *= 2 ;
		cs->clients = realloc(cs->clients, cs->size_) ;
		assert(cs->clients) ;
	}
	Client* tmp = cs->clients + cs->client_count ;
	tmp->socket = sk ;
	tmp->address = ad ;
	++cs->client_count ;
	//pthread_mutex_unlock(&cs->mutex) ;
}

const char* get_client_address(const int socket) 
{
	struct sockaddr_in addr ; socklen_t addr_size = sizeof(struct sockaddr_in) ;
	getpeername(socket, (struct sockaddr*)&addr, &addr_size) ;
	return inet_ntoa(addr.sin_addr) ;
}

void* client_connection(void* v_client)
{
	Client* client = (Client*)v_client ;
	fprintf(stdout, "Dedicated connection for client on %s, via socket %d, is online\n", client->address, client->socket) ;
	// add client to list (useless for now, since we're just doing sever-client)
	// call diffie-hellman exchange to set up symmetric keys
	// begin messaging
	while(1)
 	{
		char buffer[1024] = {'\0'} ; // declare&initialise buffer
    		if(!read(client->socket, buffer, 1023)) break ;
    		fprintf(stdout, "incoming: %s\n", buffer) ;
    	}
    	return v_client ;
}
