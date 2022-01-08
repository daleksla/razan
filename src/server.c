#include <unistd.h>
#include <pthread.h> // mutex
#include <string.h>
#include <pthread.h> // (p)threads
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "server.h" // functionality to manage client information
#include "encryption.h"

/** @brief Implementation of functionality related to storing client details & their communications **/

void client_store_init(struct ClientStore* cs)
{
    cs->size = 20 ;
    cs->clients = malloc(sizeof(struct Client) * cs->size) ; // list of max 256 char pointers

    assert(cs->clients) ;

    for(size_t i = 0 ; i < cs->size ; ++i)
    {
        (cs->clients+i)->socket = -1 ; // allows us to identify whi
        (cs->clients+i)->address = NULL ; // allows us to identify whi
    }

    pthread_mutex_init(&cs->mutex, NULL) ;
}

struct Client* add_client(const int sk, const char* ad, struct ClientStore* cs)
{
    fprintf(stdout, "log4.1\n") ;
    pthread_mutex_lock(&cs->mutex) ;
    fprintf(stdout, "log4.2\n") ;
    struct Client* tmp = find_client_gap(cs) ;
    fprintf(stdout, "log4.3\n") ;
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
    for(size_t i = 0 ; i < cs->size ; ++i)
    {
        struct Client* cl = cs->clients+i ;
        if(cl->socket == -1)
        {
            return cl ;
        }
    }
    return NULL ;
}

void close_client(struct Client* cl)
{
    close(cl->socket) ;
    cl->socket = -1 ;
    cl->address = NULL ;
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

    pthread_mutex_destroy(&cs->mutex) ;
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
    flockfile(stdout) ;
    fprintf(stdout, "LOG: Dedicated connection for client on %s, via socket %d, is online\n", client->address, client->socket) ;
    funlockfile(stdout) ;

    struct EncryptionDetails enc_details = encryption_setup(client->socket, SERVER) ;
    struct TextData text_data ; text_data_init(&text_data) ;

    const int old_flags = fcntl(client->socket, F_GETFL) ;
    fcntl(client->socket, F_SETFL, old_flags | O_NONBLOCK) ; // allows us to read from socket with a timeout

    // begin messaging
    while(1)
    {
        /* read message */
        unsigned long long byte_count = 0 ;
        read(client->socket, &byte_count, sizeof(unsigned long long)) ;
        if(byte_count) // if we managed to retrieve the data
        {
            if(byte_count > text_data.max_len)
            {
                text_data_grow(&text_data) ;
            }
            read(client->socket, text_data.cipher_text, byte_count) ;
            text_data.cipher_text_len = byte_count ;
            decrypt(&enc_details, &text_data) ;
            flockfile(stdout) ;
            fprintf(stdout, "LOG: message from address %s, via socket %d: %s\n", "localhost", client->socket, text_data.plain_text) ;
            funlockfile(stdout) ;
        /* send message */
            encrypt(&enc_details, &text_data) ;
            int ret = send(client->socket, &text_data.cipher_text_len, sizeof(unsigned long long), 0) ;
            if(ret == -1) break ;
            ret = send(client->socket, &text_data.cipher_text, strlen((const char*)text_data.cipher_text), 0) ;
            if(ret == -1) break ;
            
        }
    }

    flockfile(stdout) ;
    fprintf(stdout, "LOG: Freeing client on %s, socket %d\n", client->address, client->socket) ;
    funlockfile(stdout) ;
    close_client(client) ;
    return v_client ;
}
