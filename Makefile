target:
	# Compile
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/client_manager.c -o lib/client_manager.o
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/aes.c -o lib/aes.o
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ src/server.c -o lib/server.o
	clang -Wall -std=c11 -g -c -I include/ -I libraries/ -lcurses src/client.c -o lib/client.o
	clang -Wall -std=c11 -g -c -lm -I include/ -I libraries/ src/key_sharing.c -o lib/key_sharing.o
	
	# Link
	clang -Wall -std=c11 -g -pthread -lm -I include/ -I libraries/ lib/server.o lib/key_sharing.o lib/aes.o lib/client_manager.o -o bin/razan_server # server, to be run on one machine only
	clang -Wall -std=c11 -g -lm -I include/ -I libraries/ -lcurses lib/client.o lib/key_sharing.o lib/aes.o lib/client_manager.o -o bin/razan # client application, copy for everyone to use
