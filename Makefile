FLAGS = -Wall -std=c11 -g

all: security client server link

security:
	clang $(FLAGS) -c -I include/ -I libraries/ src/aes.c -o lib/aes.o
	clang $(FLAGS) -c -I include/ -I libraries/ src/key_sharing.c -o lib/key_sharing.o
		
client:
	clang $(FLAGS) -c -I include/ -I libraries/ -lcurses src/client_chat.c -o lib/client_chat.o
	clang $(FLAGS) -c -I include/ -I libraries/ src/client.c -o lib/client.o

server:
	clang $(FLAGS) -c -I include/ -I libraries/ src/client_manager.c -o lib/client_manager.o
	clang $(FLAGS) -c -I include/ -I libraries/ src/server.c -o lib/server.o

link:	
	clang $(FLAGS) -pthread -lm -I include/ -I libraries/ lib/key_sharing.o lib/aes.o lib/client_manager.o lib/server.o  -o bin/razan_server # server, to be run on one machine only
	clang $(FLAGS) -lm -I include/ -I libraries/ -lcurses lib/key_sharing.o lib/aes.o lib/client_chat.o lib/client.o -o bin/razan # client application, copy for everyone to use
