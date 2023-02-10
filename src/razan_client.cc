#include <stdio.h>
#include <unistd.h>
#include <argp.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pwd.h> /* Header file for the passwd structure */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <ncurses.h>

#include "logger.h"
#include "types.hpp"
#include "encryption.hpp"
#include "client_chat.hpp"

/**
 * @brief Source of client for razan terminal messaging
 * @author Salih MSA
 */

#ifndef DEFAULT_PORT
	#define DEFAULT_PORT 4242 /* port of ENDPOINT */
#endif /* DEFAULT_PORT */


#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic push
const char* argp_program_bug_address = "salih.msa@outlook.com" ;
static const char args_doc[] = "SERVER_ADDRESS" ; /* description of non-option specified command line arguments */
static const char doc[] = "razan -- client-side program to enable communication with another client" ; /* general program documentation */
static struct argp_option options[] = { /* OPTIONS FOR ARGP. each entry stores: {NAME, KEY, ARG, FLAGS, DOC} */
	{0}
} ;


/**
 * @brief struct arguments - this structure is used to communicate with parse_opt (for it to store the values it parses within it)
 */
struct arguments {
	const char* server_address ; // address of server

	const char* their_name ; // (user)name of person on network we wish to talk to
} ;


/**
 * @brief parse_opt - deals with given arguments based on given argumentsK
 * @param int - int correlating to char storing argument key
 * @param char* - argument string associated with argument key
 * @param struct argp_state* - pointer to argp_state struct storing information about the state of the option parsing
 * @return error_t - number storing 0 upon successfully parsed values, non-zero exit code otherwise
 */
static error_t parse_opt(int key, char* arg, argp_state* state)
{
	struct arguments* arguments = static_cast<struct arguments*>(state->input) ;

	switch (key) {
		case ARGP_KEY_ARG:
			if(state->arg_num == 0)
			{
				arguments->server_address = arg ;
				printl(DEBUG, "Server address given as arg: %s (in function %s, file %s)", arguments->server_address, __func__, __FILE__) ;
			}
			else if(state->arg_num == 1)
			{
				arguments->their_name = arg ;
				printl(DEBUG, "Name given as arg: %s (in function %s, file %s)", arguments->their_name, __func__, __FILE__) ;
			}
			else if(state->arg_num > 1)
			{
				argp_usage(state) ;
			}
			break ;
		case ARGP_KEY_END:
			if(state->arg_num < 2)
				argp_usage(state) ;
			break ;
		default:
			return ARGP_ERR_UNKNOWN ;
	}

	return 0 ;
}


static struct argp argp = { /* argp - The ARGP structure itself */
	options, /* list containing options */
	parse_opt, /* callback function to process args */
	args_doc, /* names of parameters */
	doc, /* documentation containing general program description */
} ;
#pragma GCC diagnostic pop /* end of argp, so end of repressing weird messages */


/* Actual functionality */
int main(int argc, char **argv)
{
	/* Initialisation */
	struct arguments arguments ;
	argp_parse(&argp, argc, argv, 0, 0, &arguments) ; /* override default arguments if provided */

	/* Main functionality */
	printl(INFO, "Launching razan server") ;

	int status ; /* will store status of syscalls throughout code */
	int client_fd ; /* will store fd of client socket. dec */

	status = socket(AF_INET, SOCK_STREAM, 0) ; /* Creating empty socket file descriptor for communication over IP, transported by TCP, specific Protocol of IP */
	if(status == -1)
	{
		printl(REPORT, "Issue creating socket resource (errno %d)", errno) ;
		abort() ;
	}
	printl(DEBUG, "Socket resource created") ;
	client_fd = status ;

	struct sockaddr_in serv_addr ; /* we'll fill in struct and connect socket to net details specified in sockaddr */
	serv_addr.sin_family = AF_INET ; /* again, IPv4 */
	serv_addr.sin_port = htons(DEFAULT_PORT) ; /* forcefully attaching socket to the port 4242 */
	if(inet_pton(AF_INET, arguments.server_address, &serv_addr.sin_addr) != 1) { /* we need to fill in sockadd_in's address field with a binary representation */
		printl(REPORT, "Failure converting address %s into binary form", arguments.server_address, DEFAULT_PORT) ;
		abort() ;
	}

	if(connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{ /* finally connect the socket of fd to the endpoint */
		printl(REPORT, "Issue connecting to server") ;
		abort() ;
	}
	printl(INFO, "Dedicated server-client socket established on address %s, port %d", arguments.server_address, DEFAULT_PORT) ;

	razan::key_t P, G ;
	if(read(client_fd, P.data(), P.size()) != P.size() || read(client_fd, G.data(), G.size()) != G.size())
	{ /* client and server (us) now have access to same public keys - we take what they generated */
		/* TODO sleep for a second - if they haven't sent data / part of data's been sent */
		printl(REPORT, "Unable to receive public keys (P, G) from server") ;
		abort() ;
	}

	razan::key_t her_mashed_key ; /* for clients mashed key */
	if(read(client_fd, her_mashed_key.data(), her_mashed_key.size()) != her_mashed_key.size())
	{ /* get server's mashed version of secret key */
		/* TODO sleep for a second - if they haven't sent data / part of data's been sent */
		printl(REPORT, "Part i of Diffie-Hellman Key exchange failed - unable to get server's mangled key") ;
		abort() ;
	}

	const auto [priv_key, my_mashed_key] = razan::generate_secret_keys(P, G) ; /* server generates own secret keys - a private and a mathematically amalgamated version */
	if(send(client_fd, my_mashed_key.data(), my_mashed_key.size(), 0) != my_mashed_key.size())
	{ /* send our mashed key */
		printl(REPORT, "Part ii of Diffie-Hellman Key exchange failed - unable to send mangled key to server") ;
		abort() ;
	}

	razan::EncryptionDetails enc_details ; /* encryption details store */
	enc_details.key = razan::generate_symmetric_key(her_mashed_key, priv_key, P) ; /* actual AES key is a mix of your private, their mashed and the public key */

	printl(INFO, "Encryption setup complete") ;

	const uid_t uid = getuid() ;
	const struct passwd *const pw = getpwuid(uid) ; /* fetch user name using entry in /etc/passwd - better than nonsudo-modifiable envs */
	if(!pw) {
		printl(REPORT, "Unable to obtain accurate user data for initialisation purposes") ;
		abort() ;
	}
	
	razan::msg_t name = reinterpret_cast<const char8_t*>(pw->pw_name) ;
	razan::msg_t name_enc = razan::encrypt(enc_details, name) ;

	if(send(client_fd, name_enc.c_str(), name_enc.size(), 0) != static_cast<ssize_t>(name_enc.size()))
	{ /* send encrypted name */
		printl(REPORT, "Unable to send user initialisation data") ;
		abort() ;
	}

	const int old_flags = fcntl(client_fd, F_GETFL) ;
	fcntl(client_fd, F_SETFL, old_flags | O_NONBLOCK) ; // allows us to read from socket with a timeout for chat window
	razan::Client me(client_fd, name, enc_details, nullptr) ;
	razan::ClientChat client_chat(me, razan::msg_t(reinterpret_cast<const char8_t*>("hi"))) ;

	printl(INFO, "Launching (ncurses) chat window") ;
	client_chat.run() ;
	printl(REPORT, "Chat closed. Ending connection and terminating") ;

	/* E(nd)O(f)P(rogram) */
	close(client_fd) ;
	return 0 ;
}
