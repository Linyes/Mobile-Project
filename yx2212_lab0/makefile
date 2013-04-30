#specify the compiler
GXX = g++ -g -Wall

#specify the target
all: tcp_server tcp_client

tcp_server: server.o
	$(GXX) server.o -o tcp_server

server.o: server.cpp
	$(GXX) -c server.cpp

tcp_client: client.o
	$(GXX) client.o -o tcp_client

client.o: client.cpp
	$(GXX) -c client.cpp

TURNIN = client.cpp server.cpp README makefile

turnin: yx2212_lab0.tar.gz

yx2212_lab0.tar.hz: $(TURNIN) 
	tar cvfz yx2212_lab0.tar.gz $(TURNIN)

clean:
	rm -f *.o tcp_server tcp_client yx2212_lab0.tar.gz
