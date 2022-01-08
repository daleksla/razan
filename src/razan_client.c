#include <stdio.h>
#include <unistd.h>
#include <argp.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <ncurses.h>

#include "encryption.h"
#include "client.h"

/** @brief Source & Implementation of client for razan terminal messaging **/

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
static char doc[] = "razan -- a program to enable communication between a user and another client running another instance of the application" ;

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
    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "Launching razan client") ;

        // Creating socket file descriptor - communication domain IPv4, TCP communication, Protocol value for Internet Protocol (IP)
    int sock = socket(AF_INET, SOCK_STREAM, 0) ;
    if(sock == -1)
    {
        fprintf(stderr, "Error: %s\n", "Issue creating initial socket") ;
        abort() ;
    }
    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "Created initial socket") ;

        // Convert IPv4 and IPv6 addresses from text to binary form
    struct sockaddr_in serv_addr ;
    serv_addr.sin_family = AF_INET ; // again, IPv4
    serv_addr.sin_port = htons(arguments.port) ; // forcefully attaching socket to the port 4242
    const char* network = "127.0.0.1" ; 
    if(inet_pton(AF_INET, network, &serv_addr.sin_addr) == -1) 
    {
        fprintf(stdout, "Error: Failure connecting to address %s on port %hu\n", network, arguments.port) ;
        abort() ;
    }
    if(arguments.verbose) fprintf(stdout, "LOG: Connected to address %s on port %hu\n", network, arguments.port) ;

        // connect() connects the socket of fd to the address specified by sockaddr struct
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        fprintf(stderr, "Error: %s\n", "Issue connecting to server socket") ;
        abort() ;
    }
    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "Connected to dedicated server-client socket") ;

    /* Communication part */
    if(arguments.verbose) fprintf(stdout, "LOG: %s\n", "razan client connected to server") ;
    struct EncryptionDetails enc_details = encryption_setup(sock, CLIENT) ;

    if(arguments.verbose) fprintf(stdout, "LOG: Launching chat window\n") ;
    client_chat(&enc_details, sock) ;
    if(arguments.verbose) fprintf(stdout, "LOG: Chat closed. Ending connecting and terminating\n") ;

    // E(nd)O(f)P(rogram)
    close(sock) ;
    return 0 ;
}
