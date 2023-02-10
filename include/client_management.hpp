#ifndef CLIENT_MANAGEMENT_HPP
#define CLIENT_MANAGEMENT_HPP
#pragma once

#include <cstddef>
#include <limits.h>
#include <vector>
#include <array>
#include <shared_mutex>

#include "encryption.hpp"
#include "types.hpp"

/**
 * @brief Definitions of functionality related to management client details & their communications
 * @author Salih MSA
 */


#define TIMEOUT_DURATION 1


namespace razan {

	/**
	 * @brief Client (struct) - struct to store an individual client's details
	 */
	struct Client {
		int socket ; /* socket created. -1 if not in use - this is a MUST
		              * for server use: stores server-client socket of whatever client running on the dedicated thread
		              * for client use: stores client-server socket */

		razan::msg_t name ; /* name of user. NULL-bytes if not in use */

		EncryptionDetails enc_details ; /* encryption details of connection
		                                 * for server use: stores details to encrypt between server and client, where the client is the one running on the dedicated thread which sends messages to the server
		                                 * for client use: stores details to encrypt between client and server
		                                 */

		Client* dst ; /* client they're talking to - NULL if doesnt exist
		               * for server use: stores client that the client on the dedicated thread is trying to communicate with
		               * for client use: nullptr
		               */

		/**
		 * @brief Client (Empty Constructor) - initialises socket id, values of array, dst pointer
		 */
		Client() noexcept ;

		/**
		 * @brief Client (Initialising Constructor) - sets values provided
		 * @param const int sock - socket of current client
		 * @param const razan::msg_t& i_name - your name
		 * @param const EncryptionDetails& i_enc_details - encryption details between client-to-client
		 * @param Client const* i_dst - destination client
		 */
		Client(const int sock, const razan::msg_t& i_name, const EncryptionDetails& i_enc_details, Client const* i_dst) noexcept ;

		/**
		 * @brief Client (Move Constructor) - safely copies over client details whilst nullifying source's
		 * @param Client&& tmp_client - ref to existing rval client
		 */
		Client(Client&& tmp_client) noexcept ;

		/**
		 * @brief Client (Assignment Operator) - safely copies over client details whilst nullifying source's
		 * @param Client&& tmp_client - ref to existing rval client
		 * @return Client& - new client
		 */
		Client& operator=(Client&& tmp_client) noexcept ;

		/**
		 * @brief Client (Destructor) - if socket id in use, close it
		 */
		~Client() noexcept ;

		/* Below are the defaulted and deleted methods
		 * I deleted the copy functionality as... we don't need it and I'd rather a static error if its accidentally used
		 */
		Client(const Client&) = delete ; // delete copy constructor
		Client& operator=(const Client&) = delete ; // delete copy assignment
	} ;


	/**
	 * @brief ClientStore (class) - class to store details of multiple clients
	 */
	class ClientStore {
		private:
			std::vector<Client> _clients ; /* stores list of client sockets */

			std::shared_mutex _mutex ; /* SHARED mutex so each individual thread can safely modify list or allow multiple readers
			                         * This should be used when doing something along the lines of traversing users
			                         */

		public:
			/**
			 * @brief ClientStore (Constructor) - allocates initialises array of clients and related details
			 * @param const std::size_t size - number of client structs to be created (pre-determined by user) (for each possible clients)
			 */
			ClientStore(const std::size_t) noexcept ;

			/**
			 * @brief add_client - adds client details into ClientStore struct and performs any initialisation tasks such as encryption with endpoint
			 * @param const int sk - representing socket client is using to connect
			 * @return Client* - pointer to the created Client struct. Will return nullpyt is a space for a client couldn't be found OR if encryption failed (in any case, no space in the client struct will be occupied)
			 */
			Client* add(const int) noexcept ;

			/**
			 * @brief find_client - find client details, using a name, into ClientStore struct
			 * @param const razan::msg_t& name - string storing desired name client
			 * @return Client* - pointer to the found Client struct. NULL if not found
			 */
			Client* find(const razan::msg_t& name) noexcept ;

			/**
			 * @brief ~ClientStore (Destructor) - deallocates vector of clients and related details and frees resources within
			 */
			~ClientStore() noexcept ;


			/* Below are the defaulted and deleted methods */
			ClientStore() = delete ;
			ClientStore(const ClientStore&) = delete ;
			ClientStore& operator=(const ClientStore&) = delete ;
			ClientStore(ClientStore&&) = default ;
			ClientStore& operator=(ClientStore&&) = default ;

	} ;


	/**
	 * @brief client_connection - the multi-threadable method ran by a thread for each client connection, facilitating all communication and post-talk cleanup
	 * @param const Client& client - pointer to Client struct instance. Const as we won't change any of the fields
	 */
	void client_connection(const Client& client) noexcept ;

} // namespace razan

#endif // CLIENT_MANAGEMENT_HPP
