#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <argp.h>

#include <pthread.h> // (p)threads

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "server.h" // functionality to manage clients

/** @brief Source & Implementation of multithreaded server for razan terminal messaging **/

const char* argp_program_bug_address = "salih.msa@outlook.com" ;

struct arguments {
    /** @brief struct arguments - this structure is used to communicate with parse_opt (for it to store the values it parses within it) **/
    char* args[1] ;  /* args for params */

    int verbose;  /* The -v flag */

    unsigned short port ;  /* Arguments for -p */

} ;

// OPTIONS FOR ARGP. each entry stores: {NAME, KEY, ARG, FLAGS, DOC}
static struct argp_option options[] =
{
    {"port", 'p', "PORT", 0,  "Assigns communications to be facilitated via given port (if possible)", 0},
    {"verbose", 'v', 0, 0, "Produce verbose output for process", 1},
    {0}
} ;

// description of non-option specified command line arguments
static char args_doc[] = "" ;
// general program documentation
static char doc[] = "razan_server -- a program resposible for faciliating encrypted local network communication between clients" ;

/** @brief parse_opt - deals with given arguments based on given argumentsK
  * @param int - int correlating to char storing argument key
  * @param char* - argument string associated with argument key
  * @param struct argp_state* - pointer to argp_state struct storing information about the state of the option parsing
  * @return error_t - number storing 0 upon successfully parsed values, non-zero exit code otherwise **/
static error_t parse_opt(int key, char *arg, struct argp_state* state)
{
    struct arguments* arguments = state->input;

    switch (key)
    {
        case 'v':
            arguments->verbose = 1;
            break;
        case 'p':
            arguments->port = (unsigned short)atoi(arg) ;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2)
            {
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
//        case ARGP_KEY_END:
//            if (state->arg_num < 1)
//            {
//                argp_usage(state);
//            }
//            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0 ;
}

static struct argp argp = { // argp - The ARGP structure itself
    options, // list containing options
    parse_opt, // callback function to process args
    args_doc, // names of parameters
    doc, // documentation containing general program description
} ;

int main(int argc, char** argv)
{
    /* Initialisation */
    struct arguments arguments;
    
        // set arguments
    arguments.verbose = 0;
    arguments.port = 4242;
    argp_parse(&argp, argc, argv, 0, 0, &arguments); // override default arguments if provided

    /* Main functionality */
    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "Launching razan server") ;

        // Creating socket file descriptor - communication domain IPv4, TCP communication, Protocol value for Internet Protocol (IP)
    int status = socket(AF_INET, SOCK_STREAM, 0) ;
    if(status == -1)
    {
        fprintf(stderr, "Error: %s\n", "Issue creating initial socket") ;
        abort() ;
    }
    const int server_fd = status ;
    if(arguments.verbose) fprintf(stdout, "LOG: Server has created socket\n") ;

    /*
    // Forcefully attaching socket to the port 4242
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        // edit so it starts cycling through available ports and prints out determined ports in a do-while loop
        fprintf(stderr, "Error: %s\n", "Issue connecting to port") ;
        return -2 ;
    }
    */

    // Fill in struct and binds the socket to the address and port number specified in sockaddr struct
    struct sockaddr_in address ;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET ; // again, IPv4
    const char* network = "127.0.0.1" ; 
    address.sin_addr.s_addr = INADDR_ANY ; // localhost
    address.sin_port = htons(arguments.port) ; // forcefully attaching socket to the port 4242

    // 'associate' the socket file descriptor to a name (IP address and port)
    status = bind(server_fd, (struct sockaddr*)&address, sizeof(address)) ;
    if(status == -1)
    {
        fprintf(stderr, "Error: %s\n", "Issue establishing initial address point") ;
        abort() ;
    }
    if(arguments.verbose) fprintf(stdout, "LOG: Server has initialised socket to address %s port %hu\n", network, arguments.port) ;

    // listen function marks  the  socket referred to by sockfd as a passive socket (ie as a socket that will be used to 
    // select & accept incoming connection requests
    status = listen(server_fd, 3) ;
    if(status == -1)
    {
        fprintf(stderr, "Error: %s\n", "Issue establishing listener to current port") ;
        abort() ;
    }
    if(arguments.verbose) fprintf(stdout, "LOG: Server has set listener to incoming communications\n") ;

    /* Client hosting part */
    struct ClientStore client_store ; // now that server is basically set up, create functionality to store client details
    client_store_init(&client_store) ;

	// now create dedicated server-client socket, connecting to user who reached passive socket first
    status = sysconf(_SC_NPROCESSORS_ONLN) ;
	if(status < 1)
	{
        fprintf(stderr, "Error: %s\n", "Issue determining CPU limit") ;
        abort() ;
    }
    const unsigned int MAX_PROCESSORS = status ;
    if(arguments.verbose) fprintf(stdout, "LOG: Server has determined maximum simulatenous connections as %hd\n", MAX_PROCESSORS) ;

    pthread_t* thread_ids = NULL ;
    thread_ids = malloc(MAX_PROCESSORS * sizeof(pthread_t)) ;
    if(!thread_ids)
    {
        fprintf(stderr, "Error: %s\n", "Issue initialising thread tracker storage") ;
        abort() ;
    }

    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "razan server online and awaiting clients") ;
    size_t i = 0 ;
    while(1) // people that connect @ once == max threads
    {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen) ;
        if(new_socket == -1)
        {
            fprintf(stderr, "Error: %s\n", "Issue creating client-server dedicated socket") ;
            break ;
        }
        if(arguments.verbose) fprintf(stdout, "LOG: %s %d\n", "Established new client-server connection over socket", new_socket) ;
        fprintf(stdout, "logg4\n") ;
        struct Client* client = add_client(new_socket, get_client_address(new_socket), &client_store) ;
        fprintf(stdout, "logg5\n") ;
        pthread_create(thread_ids+i, NULL, &client_connection, (void*)client) ;
    }

	// E(nd) O(f) P(rogram) (need to find a way to force program to reach this point upon SIGINT w/o globals
	// not all *nix operating systems have a nice resource cleanup like linux
	fprintf(stdout, "LOG: %s\n", "Disconnecting clients from server") ;
	client_store_fini(&client_store) ; // close all client sockets, free memory
	fprintf(stdout, "LOG: %s\n", "Closing server, goodbye") ;
	close(server_fd) ;
	return 0 ;
}

