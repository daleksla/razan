#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <stddef.h>

#define PORT 4242

#include "key_sharing.h" // to encrypt and decrypt messages
#include "aes.h"

/** @brief Source & Implementation of client for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

void draw_borders(WINDOW* window)
{
    int x, y ;
    getmaxyx(window, y, x) ; // load size of provided window based on given curses screen
    for(size_t i = 1 ; i < (x - 1) ; ++i)
    {
        mvwprintw(window, 0, i, "-") ;
    }
}

void client_chat(void)
{
    int parent_x, parent_y ;
    #define CHAT_BOX_SIZE 8

    initscr() ;
    curs_set(FALSE) ;

    // set up initial windows
    getmaxyx(stdscr, parent_y, parent_x) ; // get maximum x, y of terminal screen
    WINDOW* chat_log = newwin(parent_y - CHAT_BOX_SIZE, parent_x, 0, 0) ; // split full terminal, give most of screen to field window (full x, y-3)
    WINDOW* chat_box = newwin(CHAT_BOX_SIZE, parent_x, parent_y - CHAT_BOX_SIZE, 0) ; // split full terminal, give small amount to score window (full x, y=3)

    // draw initial borders
    draw_borders(chat_log) ;
    draw_borders(chat_box) ;

    while(1) // run endlessly till SIG(INT/TERM/QUIT/etc.)
    {
        int new_x, new_y ;
        // first check if terminal screen has changed size
        getmaxyx(stdscr, new_y, new_x) ; // get terminal window size

        if(new_y != parent_y || new_x != parent_x) // if window has changed
        {
            // set new max sizes 
            parent_x = new_x ;
            parent_y = new_y ;

            // recreated windows with new sizes
            wresize(chat_log, new_y - CHAT_BOX_SIZE, new_x) ;
            wresize(chat_box, CHAT_BOX_SIZE, new_x) ;
            mvwin(chat_box, new_y - CHAT_BOX_SIZE, 0) ;

            // clear main screen & created windows
            wclear(stdscr) ;
            wclear(chat_log) ;
            wclear(chat_box) ;

            // redraw borders
            draw_borders(chat_log) ;
            draw_borders(chat_box) ;
        }

        // useful drawing to our windows
        wattron(chat_log, A_BOLD) ;
        mvwprintw(chat_log, 1, 1, "Chat log") ;
        wattroff(chat_log, A_BOLD) ;
        
        wattron(chat_box, A_BOLD) ;
        mvwprintw(chat_box, 1, 1, "Input") ;
        wattroff(chat_box, A_BOLD) ;
        
        // (after logic calculations) refresh each window
        wrefresh(chat_log) ;
        wrefresh(chat_box) ;
    }

    endwin() ; // kill screen
}

int main(void)
{
	fprintf(stdout, "LOG: %s\n", "Launching razan client") ;
	/* Initialisation part */
	// set up socket, attach to location, attempt connection
	
	// Creating socket file descriptor - communication domain IPv4, TCP communication, Protocol value for Internet Protocol (IP)
	int sock = socket(AF_INET, SOCK_STREAM, 0) ;
	if(sock == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue creating initial socket") ;
		return -1 ;
	}

	struct sockaddr_in serv_addr ;  
	serv_addr.sin_family = AF_INET ; // again, IPv4
	serv_addr.sin_port = htons(PORT) ; // forcefully attaching socket to the port 4242
       
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) == -1) 
	{
		fprintf(stderr, "Error: %s\n", "Invalid address / address not supported") ;
		return -2 ;
	}

	// connect() connects the socket of fd to the address specified by sockaddr struct
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		fprintf(stderr, "Error: %s\n", "Issue connecting to server socket") ;
		return -3 ;
	}

	/* Communication part */
	fprintf(stdout, "LOG: %s\n", "razan client connected to server") ;
 	char buffer[1024] = {'\0'} ; // declare&initialise buffer
 
 	// determine symmetric key & set up encryption (tiny-AES-c)
   	read(sock, buffer, sizeof(two_keys)) ;
 	const two_keys public_keys = {((two_keys*)(buffer))->key_a, ((two_keys*)(buffer))->key_b} ;
	
  	const two_keys secret_keys = generate_secret_keys(&public_keys) ;
	send(sock, &(secret_keys.key_b), sizeof(long long int), 0) ; // send mashed key to other side
	
	long long int her_mashed_key ;
	read(sock, &her_mashed_key, sizeof(long long int)) ; // read in other clients mashed key

	const long long int key = generate_symmetric_key(her_mashed_key, secret_keys.key_a, public_keys.key_a) ; // actual key
	
	struct AES_ctx ctx ;
	AES_init_ctx(&ctx, &key) ;

	while(1)
 	{
 	    /*
 		memset(buffer, '\0', 1024) ; // declare&initialise buffer
		fprintf(stdout, "%s", "Input < ") ; fflush(stdout) ; // inline input prompt (&manual flush)
 		read(1, buffer, 1023) ; // read syscall to get user input from stdin
 		AES_ECB_encrypt(&ctx, buffer) ; // encrypt input
 		if(send(sock, buffer, strlen(buffer), 0) == -1) // send inputted message over socket, break if server hangs / shuts down
 		{
 			fprintf(stderr, "Error: %s\n", "Server unexpectedly closed") ;
			break ;
    		}
    		//if(read(sock, buffer, 1024) == -1) break ; // if EOF
    		// else decrypt then print
    		//fprintf(stdout, "reply: %s\n", buffer) ; // print server reply
    	}
    	*/
    	fprintf(stdout, "LOG: Launching chat window\n") ;
    	client_chat() ;
    }
    
	// E(nd) O(f) P(rogram)
	close(sock) ;
	return 0 ;
}
