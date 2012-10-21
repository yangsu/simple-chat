DESTDIR = bin/

all:	main

utils.o:	utils.h utils.cpp
	g++ -c utils.cpp

socket.o:	socket.h socket.cpp utils.o
	g++ -c socket.cpp

client.o:	client.h client.cpp socket.o
	g++ -c client.cpp

server.o:	server.h server.cpp socket.o
	g++ -c server.cpp

chatclient.o:	chatclient.h chatclient.cpp client.o
	g++ -c chatclient.cpp

chatserver.o:	chatserver.h chatserver.cpp server.o
	g++ -c chatserver.cpp

main: utils.o chatclient.o chatserver.o socket.o main.cpp
	g++ -o main main.cpp utils.o socket.o client.o server.o chatclient.o chatserver.o

clean:
	rm *.o main