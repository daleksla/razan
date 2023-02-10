#ifndef CLIENT_CHAT_HPP
#define CLIENT_CHAT_HPP
#pragma once

#include <stddef.h>

#include "encryption.hpp"
#include "client_management.hpp"

/**
 * @brief Declarations relating to client messaging display and backend
 * @author Salih MSA
 */


#define CHAT_BOX_SIZE 8


namespace razan {

	enum recent_msg_type {
		INCOMING_MSG_RECENT = 0,
		OUTGONE_MSG_RECENT
	} ;

	/**
	 * @brief ClientChat (class) - class to store, manage and run ncurses-based chat window
	 */
	class ClientChat {
		private:
			const razan::Client& _client ; // my connection with server (includes socket to communicate between me and server, my name, the needed encryption details)

			razan::msg_t _their_name ; // for chat purposes, a plain string storing their name (no way or even need or storing their details as above, as razan_server handles that)

			razan::msg_t _incoming_msg, _outgone_msg ; // last msg sent by dst and to dst

			recent_msg_type _recent_msg ; // INCOMING_MSG_RECENT if incoming message is more recent, if outgone msg is more recent then OUTGONE_MSG_RECENT

			razan::msg_t _outgoing_msg ; // message currently being sent

			int _parent_x, _parent_y ; // set sizes for ncurses windows

			WINDOW* _chat_log ; // displays last incoming + outgone msg

			WINDOW* _chat_box ; // displays outgoing msg

			/**
			 * @brief manage_windows - private method to manage window displays
			 */
			void manage_windows() noexcept ;

			/**
			 * @brief input - private method to read a character to the outgoing msg string. Times out if no input is available
			 * @return bool - whether string typed is completed (indicated by character input of '\n' or '\r')
			 */
			bool input() noexcept ;

		public:
			/**
			 * @brief ClientChat (Constructor) - initialises chat properties & window
			 * @param const Client& client - const ref to client which will be *sending* msgs
			 * @param const razan::msg_t& their_name - plain string storing name of person we're talking to
			 */
			ClientChat(const Client& client, const razan::msg_t& their_name) noexcept ;

			/**
			 * @brief run - actually opens chat and serves as loop
			 */
			void run() noexcept ;

			/* Below are the defaulted and deleted methods */
			ClientChat() = delete ;
			ClientChat(const ClientChat&) = delete ;
			ClientChat& operator=(const ClientChat&) = delete ;
			ClientChat(ClientChat&&) = delete ;
			ClientChat& operator=(ClientChat&&) = delete ;
			~ClientChat() = default ;
	} ;

} // namespace razan


#endif // CLIENT_CHAT_HPP
