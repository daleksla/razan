#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <cstring>
#include <cmath>
#include <algorithm>

#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <endian.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "encryption.hpp"
#include "logger.h"
#include "types.hpp"

#include "client_management.hpp" /* functionality to manage client information */

/**
 * @brief Implementation of functionality related to management client details & their communications
 * @author Salih MSA
 */


razan::Client::Client() noexcept : socket(-1), name(LOGIN_NAME_MAX, '\0'), dst(nullptr)
{}


razan::Client::Client(const int sock, const razan::msg_t& i_name, const EncryptionDetails& i_enc_details, Client const* i_dst) noexcept : socket(sock), name(i_name), enc_details(i_enc_details), dst(const_cast<razan::Client*>(i_dst))
{}


razan::Client::Client(razan::Client&& tmp_client) noexcept : socket(tmp_client.socket), name(tmp_client.name), dst(tmp_client.dst)
{
	tmp_client.socket = -1 ;
	std::fill(tmp_client.name.begin(), tmp_client.name.end(), '\0') ;
	if(tmp_client.dst)
	{
		tmp_client.dst->dst = this ; // i.e. if the person this rvalue client is connected to exists, we set their connection to be us
		tmp_client.dst = nullptr ;
	}
}


razan::Client& razan::Client::operator=(razan::Client&& tmp_client) noexcept
{
	this->socket = tmp_client.socket ;
	tmp_client.socket = -1 ;

	this->name = tmp_client.name ;
	std::fill(tmp_client.name.begin(), tmp_client.name.end(), '\0') ;

	if(tmp_client.dst)
	{
		tmp_client.dst->dst = this ; // i.e. if the person this rvalue client is connected to exists, we set their connection to be us
		tmp_client.dst = nullptr ;
	}

	return *this ;
}


razan::Client::~Client() noexcept
{
	if(this->socket)
	{
		close(this->socket) ;
		this->socket = -1 ;
	}

	std::fill(this->name.begin(), this->name.end(), '\0') ;

	/* no cleanup for enc_details needed - static buffers */

	if(this->dst && this->dst != this)
	{ /* if this client was actually connected to someone and it wasn't themselves (relay functionality) */
		this->dst->dst = nullptr ; /* turn the connection to you off */
		this->dst = nullptr ; /* turn your dangling reference to them off now */
	}
}


razan::ClientStore::ClientStore(const size_t limit) noexcept : _clients(limit)
{}


razan::Client* razan::ClientStore::add(const int sk) noexcept
{
	Client* client = nullptr ; /* reserve memory - we'll store the new client in whatever slot fills this */

	this->_mutex.lock() ;
	for(auto it = this->_clients.begin() ; it != this->_clients.end() ; it = std::next(it))
	{
		if(it->socket == -1)
		{ /* if socket of this client entry is unset */
			printl(DEBUG, "Valid client spot found (in function %s, file %s)", __func__, __FILE__) ;
			client = &(*it) ; /* ...then we use it as it's the first available spot' */
			break ;
		}
	}

	if(!client)
	{ /* if the iterations finished and we have don't have a valid ptr */
		printl(WARN, "No client space found") ;
		return nullptr ;
	}

	/* setup encryption details (actual cryptography, wanted to keep it seperate) and store within our struct */
	const auto [p, g] = razan::generate_public_keys() ; /* server create public keys */

	if(send(sk, &p, sizeof(p), 0) != sizeof(razan::key_t) || send(sk, &g, sizeof(g), 0) != sizeof(razan::key_t))
	{ /* client and server (us) now have access to same public keys */
		printl(WARN, "Unable to send public keys to client") ;
		return nullptr ;
	}

	const auto [priv, mashed] = generate_secret_keys(p, g) ; /* server generates own secret keys - a private and a mathematically amalgamated version */
	if(send(sk, &mashed, sizeof(mashed), 0) != sizeof(razan::key_t))
	{ /* send a mashed version of secret key to other side */
		printl(WARN, "Part i of Diffie-Hellman Key exchange failed - unable to send server's mangled key") ;
		return nullptr ;
	}

	razan::key_t her_mashed_key ; /* for clients mashed key */
	if(read(sk, &her_mashed_key, sizeof(her_mashed_key)) != sizeof(razan::key_t))
	{ /* read in other clients mashed key */
		/* TODO sleep for a second - if they haven't sent data / part of data's been sent */
		printl(WARN, "Part ii of Diffie-Hellman Key exchange failed - unable to receive client's mangled key") ;
		return nullptr ;
	}

	client->enc_details.key = generate_symmetric_key(her_mashed_key, priv, p) ; /* actual AES key is a mix of your private, their mashed and the public key */
	client->enc_details.iv = 9 ;

	printl(DEBUG, "Encryption setup for client (with socket %d) complete! (in function %s, file %s)", sk, __func__, __FILE__) ;

	/* get communication details - name of client and the person they want to talk to */
	razan::msg_t cipher_txt(AES_BLOCK_SIZE, '\0') ;

	const auto name_size = std::ceil(LOGIN_NAME_MAX / AES_BLOCK_SIZE) * AES_BLOCK_SIZE ;
	if(read(sk, const_cast<char8_t*>(cipher_txt.c_str()), name_size) == -1)
	{ /* read (ciphertext'd) name */
		printl(WARN, "Incoming input of client's not received") ;
		return nullptr ;
	}

	const auto plain_txt = decrypt(client->enc_details, cipher_txt) ; /* decrypt text */
	for(std::size_t i = 0 ; i < LOGIN_NAME_MAX ; ++i)
	{
		client->name[i] = plain_txt[i] ;
	}

	client->socket = sk ; /* finally, we'll put the socket ID in it */
	this->_mutex.unlock() ; /* unlock _mutex - the socket is filled in so it's all good */

	return client ;
}


razan::Client* razan::ClientStore::find(const razan::msg_t& name) noexcept
{
	Client* tmp = nullptr ;

	this->_mutex.lock_shared() ;
	for(auto it = this->_clients.begin() ; it != this->_clients.end() ; it = std::next(it))
	{
		if(it->name == name)
		{
			tmp = &(*it) ;
			break ;
		}
	}
	this->_mutex.unlock_shared() ;

	return tmp ;
}


razan::ClientStore::~ClientStore() noexcept
{}


void razan::client_connection(const razan::Client& client) noexcept
{
	/* actual setup here. this includes:
	 	* hiffie-delman encryption setup
	 	* getting the persons name
	 	* finding out who they're talking to in the form of a name. 'echo' will identify themselves, 'none' will mean nothing
	 * After all this, for logging purposes, the clients IP address and name is printed to say the connection is fully established, and the name of the person they're talking to. Note, if the person said none, then the thread should sleep for *constant* intervals until someone else connects
	 */
	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	////     DELETE ME WHEN YOU GET CLIENTS ACTUALLY SEARCHING FOR EACHOTHER        ////
	////                   THIS IS DONE NOW IN `add_client`                         ////
	////////////////////////////////////////////////////////////////////////////////////
	const_cast<razan::Client&>(client).dst = const_cast<razan::Client*>(&client) ; /* set client's endpoint... to itself */
	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	//const int old_flags = fcntl(client->socket, F_GETFL) ;
	//fcntl(client->socket, F_SETFL, old_flags | O_NONBLOCK) ; /* allows us to read from socket with a timeout atop of any previous set flags - UPDATE i don't know why exactly I did this, but i don't see the benefit and there's a downside so I won't bother with this - the client application continues to make use of this for obvious reasons (if no messages, you can still use screen etc.) */

	printl(INFO, "Dedicated client connection to %s (socket #%d) setup, awaiting communication", client.name, client.socket) ;

	/* begin messaging
	 * we use ioctl to see how much data is available to read - we extend the buffer as necessary
	 * we then read in the data from our client and decrypt it
	 * we then encrypt and forward to the dst of client
	 */
	int byte_count ;
	razan::msg_t cipher_txt(AES_BLOCK_SIZE, '\0') ; // we declare this outside to limit needless allocs
	while(ioctl(client.socket, FIONREAD, &byte_count) != -1)
	{ /* if connection / socket still works */
		std::fill(cipher_txt.begin(), cipher_txt.end(), '\0') ; // we blank it

		if(byte_count == 0) /* if EOF */
			continue ;

		printl(DEBUG, "Buffer size of text_data: %lu. Size of incoming data: %d (in function %s, file %s)", cipher_txt.size(), byte_count, __func__, __FILE__) ;

		read(client.socket, cipher_txt.data(), cipher_txt.size()) ;
		const auto plain_txt = razan::decrypt(client.enc_details, cipher_txt) ;
		printl(INFO, "Message received from %s: %s", client.name, plain_txt.c_str()) ;

		if(!client.dst->socket)
		{ /* Endpoint user has closed their connection thus we dont have a socket to send to */
			printl(INFO, "Endpoint user has disconnected, terminating connection") ;
			break ;
		}
		cipher_txt = razan::encrypt(client.dst->enc_details, plain_txt) ; /* encrypt data if endpoint is still w us */
		if(send(client.dst->socket, cipher_txt.c_str(), cipher_txt.size(), 0) == -1)
		{/* send data - if failed for whatever reason then connection broke */
			printl(INFO, "Unexpected error forwarding message to user %s (socket %d)", client.dst->name, client.dst->socket) ;
			break ;
		}
	}

	printl(INFO, "Closing connection for client %s (socket %d)", client.name, client.socket) ;
}
