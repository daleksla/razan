target:
	# Compile
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/client_manager.c -o lib/client_manager.o
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/server.c -o lib/server.o
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/client.c -o lib/client.o
	clang -Wall -std=c11 -g -c -lm -I include/ -I libraries/ src/crypt.c -o lib/crypt.o
	
	# Link
	clang -Wall -std=c11 -g -pthread -lm -I include/ -I libraries/ lib/server.o lib/crypt.o lib/client_manager.o -o bin/razan_server # server, to be run on one machine only
	clang -Wall -std=c11 -g -lm -I include/ -I libraries/ lib/client.o lib/crypt.o  lib/client_manager.o -o bin/razan # client application, copy for everyone to use
