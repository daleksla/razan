#ifndef CLIENT_CHAT_H
#define CLIENT_CHAT_H
#pragma once

#include <ncurses.h>
#include <stddef.h>

/** @brief Definitions relating to client messaging **/

struct InputBuffer {
    /** @brief InputBuffer - struct to store properties of user input **/
    char* buffer ;
    
    size_t size ;
    
    size_t pos ;    
} ;

/** @brief input_buffer_init - function to initialise properties of struct (such as array, size-storing variable)
  * @param struct InputBuffer* - pointer to InputBuffer struct **/
void input_buffer_init(struct InputBuffer*) ;

/** @brief input_buffer_grow - function to grow internal buffer
  * @param struct InputBuffer* - pointer to InputBuffer struct which values' are realloc'd **/
void input_buffer_grow(struct InputBuffer*) ;

/** @brief input_buffer_clear - function to clear a strings buffer
  * @param struct InputBuffer* - pointer to InputBuffer struct which values' are cleared **/
void input_buffer_clear(struct InputBuffer*) ;

/** @brief input_buffer_input - function to input a string *safely* into a buffer (resizing if necessary)
  * @param struct InputBuffer* - pointer to InputBuffer struct which is read into
  * @return int - Bool as to whether user has hit carriage return (ie is message to be sent) **/
int input_buffer_input(struct InputBuffer*) ;

/** @brief draw_window_borders - function to draw borders around a given ncurses Window
  * @param WINDOW* - ncurses window (note: opaque type) **/
void draw_window_borders(WINDOW*) ;

/** @brief client_chat - function to handle user input, show chat history, send & receieve messages, etc.
  * @param const int - socket handle of person to message
  * @param struct AES_ctx* - pointer to tiny-AES-c's struct used to encrypt and decrypt messages **/
void client_chat(const int sock, struct AES_ctx*) ;

#endif // CLIENT_CHAT_H
