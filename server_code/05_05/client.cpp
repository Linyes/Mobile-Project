#include <map>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <arpa/inet.h>

using namespace std;

int main ( int argc, char **argv ) {
	if (argc < 2) {
		cout << "Usage: ./client server_port" << endl;
		return -1;
	}
	int s_port = atoi( argv[1] );
	struct sockaddr_in s_addr;
	struct hostent *server;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)	perror("ERROR opening socket");
	server = gethostbyname("127.0.0.1");
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero( (char *) &s_addr, sizeof( s_addr ) );
	s_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&s_addr.sin_addr.s_addr, server->h_length);
	s_addr.sin_port = htons( s_port );

	if ( connect(sockfd,(struct sockaddr *) &s_addr ,sizeof(s_addr) ) < 0) 
		perror("ERROR connecting");


	////////////////////
	// send to server //
	////////////////////
	std::ifstream is ("tmp.png", std::ifstream::binary);
	if (!is) {printf("Read error\n"); return;}
	
	// get length of file:
	is.seekg (0, is.end);
	int length = is.tellg();
	is.seekg (0, is.beg);
	
	char * inBuffer = new char [length];
	
	is.read (inBuffer,length);
	if (is){
		write(sockfd, inBuffer, length);
		printf("Send %d successfully!\n", length);
	} else {
		printf("Send error!\n");
		return;
	}


	///////////////////////////////
	// receive and write to file //
	///////////////////////////////
	std::ofstream ofs("recv.png", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	ssize_t bytes_read;
	char *buffer = new char[1024];
	memset(buffer, 0, 1024);
	while (true) {
		bytes_read = ::read(sockfd, buffer, 1024);
		if (bytes_read <= 0) break;
		
		
		ofs.write(buffer, bytes_read);
		//if (bytes_written != bytes_read) perror("write error");
	}
	ofs.close();
	close( sockfd );
	cout << "finish receiving from server" << endl;

	return 0;
}
