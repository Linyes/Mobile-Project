#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using namespace std;

#define BUFFER_SIZE 1000

/* error msg */
void error(const char *msg) {
	perror(msg);
	exit(0);
}

/* format the path entered by user */
string get_path( string path) {
	if (path[path.length()-1]!='/') {
		path += "/";
	}
	for (unsigned int i=0; i<path.length(); i++) {
		if (path[i] == '/') {
			path.insert(i, "/");
		}
		i++;
	}
	return path;
}

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	if (argc < 5) {
		fprintf(stderr,"usage %s hostname port directory\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)	error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");
	n = write(sockfd,argv[3],strlen(argv[3]));
	if (n < 0) error("ERROR writing to socket");

	char buffer[101];
	int output_fd;

	ssize_t bytes_read, bytes_written;

	string dir = get_path(argv[4]);
	dir += argv[3];

	dir = dir.substr(0, dir.length()-1);
	char ne_msg[100];
	bzero(ne_msg, 100);
	
	output_fd = open(dir.c_str(), O_WRONLY | O_CREAT, 0644);
	if (output_fd == -1) perror("open write file");
	while ((bytes_read = read(sockfd, &buffer, 100)) > 0) {
		bytes_written = write(output_fd, &buffer, (ssize_t) bytes_read);
		if (bytes_written != bytes_read) perror("write error");
	}

	printf("%s saved\n", argv[3]);

	close(sockfd);
	return 0;
}
