#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <pthread.h> // mutex
#include <stddef.h> // size_t
#include <string.h>

/** @brief Definitions of functionality related to management client details & their communications
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

typedef struct {

	int socket ;
	
	const char* address ;

} Client ;

typedef struct { 

	Client* clients ; // stores list of client sockets
	
	size_t client_count ; // stores current count of clients
	
	size_t size_ ; // stores current max size of array 

	pthread_mutex_t mutex ; // mutex so each execution branch can safely read / add to list
	                               // for example, when host connection is established, their IP is taken and stored in razan
	                               // when the client wishes to talk to someone in his own thread, a lookup is used to see if that person is using razan also
} ClientStore ;

void client_store_init(ClientStore*) ;

void add_client(const int, const char*, ClientStore*) ;

const char* get_client_address(const int) ;

void client_store_fini(ClientStore*) ;

void* client_connection(void*) ;

#endif // CLIENT_MANAGER_H
