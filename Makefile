target:
	# Compile
	clang -Wall -std=c11 -g -c -I include/ src/client_manager.c -o lib/client_manager.o
	clang -Wall -std=c11 -g -c -I include/ src/server.c -o lib/server.o
	clang -Wall -std=c11 -g -c -I include/ src/client.c -o lib/client.o

	# Link
	clang -Wall -std=c11 -g -pthread -I include/ lib/server.o lib/client_manager.o -o bin/razan_server # server, to be run on one machine only
	clang -Wall -std=c11 -g -I include/ lib/client.o lib/client_manager.o -o bin/razan # client application, copy for everyone to use
