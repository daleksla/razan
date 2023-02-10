#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdnoreturn.h>
#include <errno.h>
#include <argp.h>

#include <thread>
#include <vector>

#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "logger.h"
#include "client_management.hpp"

/**
 * @brief Source of multithreaded server for razan terminal messaging
 * @author Salih MSA
 */


#ifndef DEFAULT_PORT
	#define DEFAULT_PORT 4242 /* port for THIS system, for others to access via */
#endif /* DEFAULT_PORT */


/* ARGP */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic push
const char *argp_program_bug_address = "salih.msa@outlook.com" ;
static const char args_doc[] = "LIMIT" ; /* description of non-option specified command line arguments */
static const char doc[] = "razan_server -- server program resposible for faciliating encrypted local network communication between clients" ; /* general program documentation */
static struct argp_option options[] = { /* OPTIONS FOR ARGP. each entry stores: {NAME, KEY, ARG, FLAGS, DOC} */
	{0}
} ;


/**
 * @brief struct arguments - this structure is used to communicate with parse_opt (for it to store the values it parses within it)
 */
struct arguments {
	std::size_t limit ;
} ;


/**
 * @brief parse_opt - deals with given arguments based on given arguments
 * @param int key - int correlating to char storing argument key
 * @param char *arg - argument string associated with argument key
 * @param struct argp_state *state - pointer to server_argp_state struct storing information about the state of the option parsing
 * @return error_t - number storing 0 upon successfully parsed values, non-zero exit code otherwise
 */
static error_t parse_opt(int key, char* arg, argp_state* state)
{
	struct arguments* arguments = static_cast<struct arguments*>(state->input) ;

	switch(key)
	{
		case ARGP_KEY_ARG:
			/* note: args exclude program name - these are mandatory args only */
			if(state->arg_num == 0)
			{ /* limit arg */
				long signed tmp_limit = strtol(arg, NULL, 10) ; /* convert arg into base10 (deenary) value */
				printl(DEBUG, "User limit as string: %s. User limit as ssize_t: %ld (function %s, file %s)", arg, tmp_limit, __func__, __FILE__) ;
				if(tmp_limit == LONG_MIN || tmp_limit == LONG_MAX || tmp_limit < 0)
				{ /* in case of under/overflows or an invalid user limit */
					printl(REPORT, "(User) Limit value is either too large / small to store in size_t or invalid (must be larger than 0) (errno %d)", errno) ;
					return ARGP_ERR_UNKNOWN ;
				}
				arguments->limit = (size_t)tmp_limit ;
			}
			else if(state->arg_num >= 1)
			{ /* i.e. an unexpected key arg */
				argp_usage(state) ;
			}
			break ;
		case ARGP_KEY_END:
			if(state->arg_num < 1) /* if this arg, given that it's the last arg we're processing, is in the ith place of the array, where i is smaller than x */
				argp_usage(state) ;
			break ;
		default:
			return ARGP_ERR_UNKNOWN ;
	}

	return 0 ;
}


static struct argp argp = { /* argp - definition of ARGP structure itself */
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
	argp_parse(&argp, argc, argv, 0, 0, &arguments) ;

	/* Main functionality */
	printl(INFO, "Launching razan server") ;

	int server_fd ; /* will store fd of server socket. dec */
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* Creating empty socket file descriptor for communication over IP, transported by TCP, specific Protocol of IP */
		printl(REPORT, "Issue creating socket resource (errno %d)", errno) ;
		abort() ;
	}
	printl(INFO, "Socket resource created") ;

	int opt = 1 ;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
		printl(REPORT, "Issue setting options for socket (errno %d)", errno) ;
		abort() ;
	}
	printl(DEBUG, "Socket options set (in function %s, file %s)", __func__, __FILE__) ;

	struct sockaddr_in address ; /* we'll fill in struct and binds socket to net details specified in sockaddr */
	address.sin_family = AF_INET ; /* again, IP */
	address.sin_addr.s_addr = INADDR_ANY ; /* all IPs of the machine. therefore IP on network, 127.0.0.1, etc. */
	address.sin_port = htons(DEFAULT_PORT) ; /* bind to specific port */
	socklen_t addrlen = sizeof(address) ; /* seems silly when we could just do sizeof but tldr we need this later for arg */
	if(bind(server_fd, (struct sockaddr*)&address, addrlen) == -1) { /* 'associate' the socket file descriptor to a name (IP address and port) */
		printl(REPORT, "Issue binding socket to address / port %d (errno %d)", DEFAULT_PORT, errno) ;
		abort() ;
	}
	printl(INFO, "Server has initialised socket to port %d", DEFAULT_PORT) ;
	if(listen(server_fd, 3) == -1)
	{ /* listen function marks the socket referred to by sockfd as a passive socket */
		printl(REPORT, "Issue establishing listener to current port (errno %d)", errno) ;
		abort() ;
	}
	printl(INFO, "Server listener set for incoming communications") ;

	razan::ClientStore client_store(arguments.limit) ; /* now that server is basically set up, create functionality to store client details */
	std::vector<std::thread> thread_ids(arguments.limit) ; /* create thread objects (but not active threads) for each potential user */
	printl(DEBUG, "Using %lu as maximum number simulatenous connections (in function %s, file %s)", arguments.limit, __func__, __FILE__) ;

	printl(INFO, "Awaiting clients...") ;
	while(true)
	{ /* whilst SIGINT hasn't been sent */
		const int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen) ;
		if(new_socket == -1)
		{
			printl(WARN, "Unexpected issue when waiting to create new client-server dedicated socket (errno %d)", errno) ;
			continue ;
		}
		printl(INFO, "Established new client-server connection") ;

		razan::Client* client = client_store.add(new_socket) ; /* for now, we create a user by setting the socket which thread-safely reserves a socket */
		int timeout = TIMEOUT_DURATION ; /* we'll use this to continuinally sleep this thread for x seconds, starting with 10 */
		printl(DEBUG, "Client instance address: %p (in function %s, file %s)", &client, __func__, __FILE__) ;
		while(!client)
		{
			printl(WARN, "User limit reached. Will keep retrying") ;
			sleep(timeout) ;
			timeout *= 2 ;
			client = client_store.add(new_socket) ; /* for now, we create a user by setting the socket. other details later */
		}

		// since we have AS MANY threads as we do client slots, there will never be an issue we can assign the latter but not the former
		std::thread client_thread(razan::client_connection, std::cref(*client)) ;
		for(auto& thread_id : thread_ids)
		{ /* this loop will identify a thread ID esque which isn't in use */
			if(thread_id.joinable())
			{
				thread_id = std::move(client_thread) ;
				break ;
			}
		}
	}

	/* E(nd) O(f) P(rogram) */
	printl(REPORT, "Closing server, goodbye") ;
	close(server_fd) ;
	return 0 ;
}
