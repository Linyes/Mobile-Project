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

	char buffer[101];

	ssize_t bytes_r, bytes_w;


	
	int in_fd = open( "lena.jpg", O_RDONLY );
	if ( in_fd == -1 ) perror("open read file");

	int kk = 0;
	while ( (bytes_r = read( in_fd, &buffer, 100 ) ) > 0 ) {
		cout << kk++ << endl;
		bytes_w = write( sockfd, &buffer, bytes_r );
		if (bytes_w != bytes_r) perror("write error");
	}

	close( in_fd );

	cout << "finish sending to server" << endl;



	int out_fd = open("from_server.jpg", O_WRONLY | O_CREAT, 0644);
	if (out_fd == -1) perror("open file error");

	memset( buffer, 0, 101 );
	while ( (bytes_r = read( sockfd, &buffer, 100)) > 0 ) {
		bytes_w = write( out_fd, &buffer, (ssize_t) bytes_r);
		if (bytes_w != bytes_r) perror("write error");
	}
	close( out_fd );
	close( sockfd );
	cout << "finish receiving from server" << endl;

	return 0;
}
