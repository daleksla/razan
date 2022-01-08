FLAGS = -std=c11 -Wall -Wextra -g
INCLUDED_LIBS = -I include/ -I /usr/include/openssl

LINK_LIBS_SECURITY_LIB=`locate libcrypto.a` -ldl -lm -pthread
LINK_LIBS_SERVER_LIB=lib/encryption.o
LINK_LIBS_CLIENT_LIB=lib/encryption.o -lcurses
LINK_LIBS_SERVER_BIN=lib/server.o lib/encryption.o lib/client_manager.o -pthread
LINK_LIBS_CLIENT_BIN=lib/client.o lib/client_chat.o lib/encryption.o -lcurses

all: security_lib server_lib client_lib bin

security_lib:
	cc $(FLAGS) $(INCLUDED_LIBS) -c src/encryption.c $(LINK_LIBS_SECURITY_LIB) -o lib/encryption.o 

server_lib:
	cc $(FLAGS) $(INCLUDED_LIBS) -c src/server.c $(LINK_LIBS_SERVER_LIB) -o lib/server.o
	cc $(FLAGS) $(INCLUDED_LIBS) -c src/razan_server.c $(LINK_LIBS_SERVER_LIB) -o lib/razan_server.o

client_lib:
	cc $(FLAGS) $(INCLUDED_LIBS) -c src/client.c $(LINK_LIBS_CLIENT_LIB) -o lib/client.o
	cc $(FLAGS) $(INCLUDED_LIBS) -c src/razan_client.c $(LINK_LIBS_CLIENT_LIB) -o lib/razan_client.o

bin:
	cc $(FLAGS) $(INCLUDED_LIBS) $(LINK_LIBS_SERVER_BIN) $(LINK_LIBS_SECURITY_LIB) -o bin/razan_server # server, to be run on one machine only
	cc $(FLAGS) $(INCLUDED_LIBS) $(LINK_LIBS_CLIENT_BIN) $(LINK_LIBS_SECURITY_LIB) -o bin/razan # client application, copy for everyone to use
