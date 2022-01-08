#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "client.h"
#include "encryption.h"

/** @brief Functionality implementations relating to client messaging **/

void input_buffer_init(struct InputBuffer* input_buffer)
{
    input_buffer->size = 100 ;
    input_buffer->buffer = malloc(sizeof(char) * input_buffer->size) ;
    input_buffer->buffer[input_buffer->size-1] = '\0' ;
    input_buffer_clear(input_buffer) ;
}

void input_buffer_grow(struct InputBuffer* input_buffer)
{
    input_buffer->size *= 2 ;
    input_buffer->buffer = realloc(input_buffer->buffer, input_buffer->size) ;
    assert(input_buffer->buffer) ;
    memset(input_buffer->buffer + (input_buffer->size/2), ' ', input_buffer->size / 2) ;
}

void input_buffer_clear(struct InputBuffer* input_buffer)
{
    input_buffer->pos = 0 ;
    memset(input_buffer->buffer, ' ', input_buffer->size-1) ;   
}

int input_buffer_input(struct InputBuffer* input_buffer)
{
    if(input_buffer->pos == input_buffer->size)
    {
        input_buffer_grow(input_buffer) ;
    }
    
    char tmp = getch() ;
    switch(tmp)
    {
        case '\r': // allow fallthrough (acts as OR (of \r or \n char))
        case '\n':
        {
            input_buffer->buffer[input_buffer->pos] = '\0' ;
            return 1 ; // exit function, prompts user that input is complete & to send
        }
        case ERR: // getch() timeout return
        {
            break ;
        }
        case 127: // delete key
        {
            input_buffer->buffer[input_buffer->pos] = ' ' ;
            if(input_buffer->pos) 
            {
                --input_buffer->pos ; // reverse position in buffer
            }
            break ;
        }
        default:
        {
            input_buffer->buffer[input_buffer->pos] = tmp ;
            ++input_buffer->pos ;
            break ;
        }
    }
    return 0 ;
}

void input_buffer_fini(struct InputBuffer* input_buffer)
{
    free(input_buffer->buffer) ;
    input_buffer->pos = 0 ;
    input_buffer->size = 0 ;
}

void draw_window_borders(WINDOW* window)
{
    size_t x, y ;
    getmaxyx(window, y, x) ; // load size of provided window based on given curses screen
    for(size_t i = 1 ; i < (x - 1) ; ++i)
    {
        mvwprintw(window, 0, i, "-") ;
    }
    for(size_t i = 1 ; i < (y - 1) ; ++i)
    {
        mvwprintw(window, i, 0, "-") ;
    }
}

void client_chat(const struct EncryptionDetails* enc_details, const int sock)
{
    struct TextData text_data ;
    text_data_init(&text_data) ;

    struct InputBuffer input_buffer ;
    input_buffer_init(&input_buffer) ;
    
    const int old_flags = fcntl(sock, F_GETFL) ;
    fcntl(sock, F_SETFL, old_flags | O_NONBLOCK) ; // allows us to read from socket with a timeout

    /* set up ncurses windows */

    int parent_x, parent_y ;
    #define CHAT_BOX_SIZE 8

    initscr() ;
    curs_set(FALSE) ;
    noecho() ; // prevent user input echo (we'll manually print)

    // set up initial windows
    getmaxyx(stdscr, parent_y, parent_x) ; // get maximum x, y of terminal screen
    WINDOW* chat_log = newwin(parent_y - CHAT_BOX_SIZE, parent_x, 0, 0) ; // split full terminal, give most of screen to field window (full x, y-3)
    WINDOW* chat_box = newwin(CHAT_BOX_SIZE, parent_x, parent_y - CHAT_BOX_SIZE, 0) ; // split full terminal, give small amount to score window (full x, y=3)

    nodelay(chat_box, 1) ;
    wtimeout(chat_box, 750) ;

    // draw initial borders
    draw_window_borders(chat_log) ;
    draw_window_borders(chat_box) ;

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
            draw_window_borders(chat_log) ;
            draw_window_borders(chat_box) ;
        }

        // useful drawing to our windows
        wattron(chat_log, A_BOLD) ;
        mvwprintw(chat_log, 1, 0, "Chat log\n") ;
        wattroff(chat_log, A_BOLD) ;
        
        wattron(chat_box, A_BOLD) ;
        mvwprintw(chat_box, 1, 0, "Input ") ;
        wattroff(chat_box, A_BOLD) ;
        
        // perform initial refresh to draw franes etc.
        wrefresh(chat_log) ;
        wrefresh(chat_box) ;

        // fetch server messages, display
        unsigned long long byte_count = 0 ;
        read(sock, &byte_count, sizeof(unsigned long long)) ;
        if(byte_count) // if we managed to retrieve the data
        {
            if(byte_count > text_data.max_len)
            {
                text_data_grow(&text_data) ;
            }
            read(sock, text_data.cipher_text, byte_count) ;
            text_data.cipher_text_len = byte_count ;
            decrypt(enc_details, &text_data) ;
        }

        // get user input, timeout if user takes too long, receive logs, display. refresh after each screen
        int user_input = input_buffer_input(&input_buffer) ;
        if(user_input == 1) // if carriage hit
        {
            if(input_buffer.size > text_data.max_len)
            {
                text_data_grow(&text_data) ;
            }
            strncpy((char*)text_data.plain_text, input_buffer.buffer, input_buffer.size) ;
            text_data.plain_text_len = input_buffer.size ;
            encrypt(enc_details, &text_data) ; // encrypt input

            send(sock, &text_data.cipher_text_len, sizeof(unsigned long long), 0) ;
            send(sock, text_data.cipher_text, strlen((const char*)text_data.cipher_text), 0) ;

            input_buffer_clear(&input_buffer) ;
            wprintw(chat_box, "%s", input_buffer.buffer) ; // clear chatbox
        }
        else {
            wprintw(chat_box, "%s", input_buffer.buffer) ;
        }
        wrefresh(chat_box) ;
    }

    endwin() ; // kill screen
    input_buffer_fini(&input_buffer) ;
}
