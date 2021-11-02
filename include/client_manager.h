#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
#pragma once

#include <pthread.h> // mutex
#include <stddef.h> // size_t
#include <string.h>

/** @brief Definitions of functionality related to management client details & their communications */

struct Client {
	/** @brief Client - Struct to store an individual client's details **/
	int socket ;
	
	const char* address ;

} ;

struct ClientStore { 
	/** @brief ClientStore - Struct to store details of multiple clients **/
	struct Client* clients ; // stores list of client sockets
	
	size_t size ; // stores current max size of array 

	pthread_mutex_t mutex ; // mutex so each execution branch can safely read / add to list
	                               // for example, when host connection is established, their IP is taken and stored in razan
	                               // when the client wishes to talk to someone in his own thread, a lookup is used to see if that person is using razan also
} ;

/** @brief client_store_init - allocates initialises array of clients and related details
  * @param struct ClientStore* - pointer to an instance of the ClientStore struct **/
void client_store_init(struct ClientStore*) ;

/** @brief find_client_gap - finds first instance of space in ClientStore struct  
  * @param struct ClientStore* - pointer to an instance of the ClientStore struct
  * @return struct Client* - pointer to first available Client struct-space **/
struct Client* find_client_gap(struct ClientStore*) ;

/** @brief add_client - adds client details into ClientStore struct  
  * @param const int - representing socket client is using to connect
  * @param const char* - c-string storing (IP) address of client 
  * @param struct ClientStore* - pointer to an instance of the ClientStore struct
  * @return struct Client* - pointer to the created Client struct **/
struct Client* add_client(const int, const char*, struct ClientStore*) ;

/** @brief get_client_address - determines the (IP) address behind a client's socket connection
  * @param const int - representing socket client is using to connect
  * @return const char* - c-string storing (IP) address of client **/
const char* get_client_address(const int) ;

/** @brief close_client - frees socket, marks the memory as reusuable for future client connections
  * @param struct Client* - pointer to Client struct instance **/
void close_client(struct Client*) ;

/** @brief client_connection - the multi-threadable method ran by a thread for each client connection, facilitating all communication and post-talk cleanup
  * @param void* - generic pointer to Client struct instance
  * @param void* - generic pointer to now used-up Client struct instance (not to be used necessarily, just for pthread's sake) **/
void* client_connection(void*) ;

/** @brief client_store_fini - deallocates initialises array of clients and related details and frees resources within
  * @param struct ClientStore* - pointer to an instance of the ClientStore struct **/
void client_store_fini(struct ClientStore*) ;

#endif // CLIENT_MANAGER_H
