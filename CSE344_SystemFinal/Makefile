CC = gcc
CFLAGS = -ansi -g -Wall -std=gnu99 -lm -lpthread  -lrt 



makefile: ./Servant/servant.c ./Servant/lib.c ./Client/client.c ./Server/server.c  ./Server/queue.c
	$(CC) -o servant ./Servant/servant.c  ./Servant/lib.c  $(CFLAGS)
	$(CC) -o client ./Client/client.c  $(CFLAGS)
	$(CC) -o server ./Server/server.c  ./Server/queue.c $(CFLAGS)

clean:
	rm -f *.o servant
	rm -f *.o client
	rm -f *.o server


#calistirma  ./client -r requestFile -q PORT -s 127.0.0.1
#

