#include <map>
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
#include <sys/stat.h>
#include <arpa/inet.h>

using namespace std;

#define BUFFER_SIZE 1024
#define FILE_LIMIT 31457280 
#define CACHE_LIMIT 62914560


/* error msg handling */
void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char filename[10240];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)	error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
	listen(sockfd,5);
	
	while (1) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) error("ERROR on accept");
		bzero(filename,1024);
		while ((n = read(newsockfd,filename,10240)) == 10240) {
		//if (n < 0) error("ERROR reading from socket");
			printf("received %d bytes\n", n);
		}
		printf("finished receiving\n");
		
		char buffer[10240];
		int input_fd;
		ssize_t bytes_read, bytes_written;
		input_fd = open("tmp.png", O_RDONLY);
		if (input_fd == -1) perror("open file");
		while ((bytes_read = read(input_fd, buffer, 10240)) > 0) {
			bytes_written = write(newsockfd, buffer, (ssize_t) bytes_read);
			if (bytes_written != bytes_read) perror("write error");
			else printf("send %d bytes\n", bytes_written);
		}
		close(input_fd);
		close(newsockfd);
	}
	close(sockfd);

	return 0; 
}
