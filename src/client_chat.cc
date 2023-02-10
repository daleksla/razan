#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <ncurses.h>

#include "logger.h"
#include "types.hpp"
#include "client_chat.hpp"
#include "encryption.hpp"

/**
 * @brief Definitions relating to client messaging display and backend
 * @author Salih MSA
 */


void razan::ClientChat::manage_windows() noexcept
{
	// clear main screen & created windows
	wclear(stdscr) ;
	wclear(this->_chat_log) ;
	wclear(this->_chat_box) ;

	int new_x, new_y ;
	getmaxyx(stdscr, new_y, new_x) ; // get terminal window size
	if(new_y != this->_parent_y || new_x != this->_parent_x) // first check if terminal screen has changed size
	{
		// set new max sizes
		this->_parent_x = new_x ;
		this->_parent_y = new_y ;

		// recreated windows with new sizes
		wresize(this->_chat_log, this->_parent_y  - CHAT_BOX_SIZE, this->_parent_x) ;
		wresize(this->_chat_box, CHAT_BOX_SIZE, this->_parent_x) ;
		mvwin(this->_chat_box, this->_parent_y  - CHAT_BOX_SIZE, 0) ;

		// redraw borders
		auto draw_window_borders = [&](WINDOW* window) -> void {
			std::size_t x, y ;
			getmaxyx(window, y, x) ; // load size of provided window based on given curses screen
			for (size_t i = 1 ; i < (x - 1) ; ++i)
				mvwprintw(window, 0, i, "-") ;
			for (size_t i = 1 ; i < (y - 1) ; ++i)
				mvwprintw(window, i, 0, "-") ;
		} ;
		draw_window_borders(this->_chat_log) ;
		draw_window_borders(this->_chat_box) ;
	}

	// useful drawing to our windows
	wattron(this->_chat_log, A_BOLD) ;
	mvwprintw(this->_chat_log, 1, 0, "Chat log\n") ;
	wattroff(this->_chat_log, A_BOLD) ;

	wattron(this->_chat_box, A_BOLD) ;
	mvwprintw(this->_chat_box, 1, 0, "Input: ") ;
	wattroff(this->_chat_box, A_BOLD) ;

	// perform initial refresh to draw franes etc.
	wrefresh(this->_chat_log) ;
	wrefresh(this->_chat_box) ;

	switch(this->_recent_msg)
	{
		case OUTGONE_MSG_RECENT:
			wprintw(this->_chat_log, "%s", this->_incoming_msg.c_str()) ;
			wprintw(this->_chat_log, "%s\n", this->_outgone_msg.c_str()) ; // print outgone message towards the bottom of screen
			break ;
		case INCOMING_MSG_RECENT:
			wprintw(this->_chat_log, "%s\n", this->_outgone_msg.c_str()) ;
			wprintw(this->_chat_log, "%s", this->_incoming_msg.c_str()) ; // print incoming message towards the bottom of screen
			break ;
	}
	wprintw(this->_chat_box, "%s", this->_outgoing_msg.c_str()) ;
}


bool razan::ClientChat::input() noexcept
{
	static std::size_t index = 0 ; // will track between calls the position in the buffer

	if(this->_outgoing_msg.size() == index) // ie we've filled buffer
	{
		const auto old_size = this->_outgoing_msg.size() ;
		this->_outgoing_msg.resize(old_size * 2) ;
		std::fill(this->_outgoing_msg.begin() + old_size - 1, this->_outgoing_msg.end(), '\0') ;
	}

	const char tmp = getch() ;
	switch(tmp)
	{
		case '\r': // allow fallthrough (acts as OR (of \r or \n char))
			[[fallthrough]] ;
		case '\n':
			this->_outgoing_msg[index] = '\0' ;
			return true ; // exit function, prompts user that input is complete & to send
		case ERR: // getch() timeout return
			break ;
		case 127: // backspace key
			if(index) // ie index != 0
				--index ; // reverse position in buffer
			break ;
		default:
			this->_outgoing_msg[index] = tmp ;
			++index ;
			break ;
	}

	return false ;
}


razan::ClientChat::ClientChat(const razan::Client& client, const razan::msg_t& their_name) noexcept : _client(client), _their_name(their_name), _incoming_msg(), _outgone_msg(), _recent_msg(OUTGONE_MSG_RECENT), _outgoing_msg(), _parent_x(0), _parent_y(0), _chat_log(nullptr), _chat_box(nullptr)
{}


void razan::ClientChat::run() noexcept
{
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//	/* set up ncurses windows */
//	if(initscr() != stdscr)
//	{
//		printl(REPORT, "Error initialising ncurses screen") ;
//		abort() ;
//	}
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//	if(curs_set(0) == ERR)
//	{
//		printl(REPORT, "Error setting cursor mode") ;
//		abort() ;
//	}
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//	noecho() ; // prevent user input echo (we'll manually print)
//
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//
//	// set up initial windows
//	getmaxyx(stdscr, this->_parent_y, this->_parent_x) ; // get maximum x, y of terminal screen
//	this->_chat_log = newwin(this->_parent_y - CHAT_BOX_SIZE, this->_parent_x, 0, 0) ; // split full terminal, give most of screen to field window (full x, y-3)
//	this->_chat_box = newwin(CHAT_BOX_SIZE, this->_parent_x, this->_parent_y - CHAT_BOX_SIZE, 0) ; // split full terminal, give small amount to score window (full x, y=3)
//
//	if(!this->_chat_log || !this->_chat_box)
//	{
//		printl(REPORT, "Unable to multiplex windows") ;
//		abort();
//	}
//
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//
//	nodelay(this->_chat_box, true) ;
//	wtimeout(this->_chat_box, 0) ;
//
//	printl(DEBUG, "I am in the run function detecting blocking. Line %d (function %s, file %s)", __LINE__, __func__, __FILE__) ;
//
//	int byte_count ;
//	while(ioctl(this->_client.socket, FIONREAD, &byte_count) != -1) /* check that the connection is still valid and get bytes */
//	{
//		this->manage_windows() ;
//
//		if(byte_count)
//		{ // if we are able to retrieve data
//			if((std::size_t)byte_count > this->_incoming_msg.size())
//			{
//				const auto old_size = this->_incoming_msg.size() ;
//				this->_incoming_msg.resize(old_size * 2) ;
//				std::fill(this->_incoming_msg.begin() + old_size - 1, this->_incoming_msg.end(), '\0') ;
//			}
//			read(this->_client.socket, this->_incoming_msg.data(), static_cast<std::size_t>(byte_count)) ; // read in encrypted message
//			this->_incoming_msg = razan::decrypt(this->_client.enc_details, this->_incoming_msg) ; // now overwrite encrypted bytes with plaintext
//		}
//
//		if(this->input() == true)
//		{ // if user indicates message complete
//			const auto outgoing_msg_enc = razan::encrypt(this->_client.enc_details, this->_outgoing_msg) ; // encrypt input
//			send(this->_client.socket, outgoing_msg_enc.data(), outgoing_msg_enc.size(), 0) ;
//			this->_outgone_msg = std::move(this->_outgoing_msg) ;
//			this->_outgoing_msg = razan::msg_t() ;
//		}
//	}
//
//	endwin() ; // kill screen


	razan::msg_t msg = reinterpret_cast<const char8_t*>("hi") ;
	printl(DEBUG, "Message to be encrypted. String: %s (in function %s, file %s)", msg.c_str(), __func__, __FILE__) ;
	const auto outgoing_msg_enc = razan::encrypt(this->_client.enc_details, msg) ; // encrypt input
	printl(DEBUG, "Encrypted Message. String: %s (in function %s, file %s)", outgoing_msg_enc.c_str(), __func__, __FILE__) ;
	send(this->_client.socket, outgoing_msg_enc.data(), outgoing_msg_enc.size(), 0) ;
}
