#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
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
#include <netdb.h>
#include <sys/stat.h>
#include <arpa/inet.h>

using namespace cv;

void meanShiftFiltering(int newsockfd){
ssize_t bytes_r, bytes_w;
		char buffer[1024];
		int out_fd = open("mean_recv.png", O_WRONLY | O_CREAT, 0644);
		if (out_fd == -1) perror("open file error");

		std::cout << "0" << std::endl;

		memset( buffer, 0, 1024 );
		bytes_r = read(newsockfd, &buffer, 1024);
		int readLength = atoi(buffer);
		printf("%s,%d\n", buffer, readLength);
		
		memset( buffer, 0, 1024 );
		while ( (bytes_r = read( newsockfd, &buffer, 1024)) > 0 ) {
			bytes_w = write( out_fd, &buffer, (ssize_t) bytes_r);
			if (bytes_w != bytes_r) perror("write error");
			printf("received %ld bytes\n", bytes_r);
			readLength -= (int)bytes_r;
			if (readLength == 0) break;
		}
		printf("Finished receiving\n");
		//bytes_w = write( out_fd, &buffer, (ssize_t) bytes_r);
		//if (bytes_w != bytes_r) perror("write error");
		//printf("received %ld bytes\n", bytes_r);
		
		close( out_fd );


		//////////////////////////////////////////////////////////////////
		//								//
		//	here goes the OpenCV codes to process images		//
		//								//
		//////////////////////////////////////////////////////////////////

		Mat src, dst;
		Mat grad;

		int scale = 1;
		int delta = 0;
		int ddepth = CV_16S;

		/// Load an image
		src = imread( "mean_recv.png" );

		if( !src.data )
		{ return; }
		
		pyrMeanShiftFiltering(src, dst, 15, 40);

		imwrite( "mean_result.png", dst );  

		//////////////////////////////////////////////////////////////////
		//								//
		//	end of OpenCV code					//
		//								//
		//////////////////////////////////////////////////////////////////


		int in_fd = open("mean_result.png", O_RDONLY );
		if ( in_fd == -1 ) perror( "open read files" );

		memset( buffer, 0, 1024 );
		while ( (bytes_r = read( in_fd, &buffer, 1024 ) ) > 0 ) {
			bytes_w = write( newsockfd, &buffer, (ssize_t)bytes_r );
			if (bytes_w != bytes_r) perror( "socket write error" );
		}
		close( in_fd );
		close( newsockfd );
		std::cout << "Finished writing! " << std::endl;
}
void canny(int newsockfd){
		ssize_t bytes_r, bytes_w;
		char buffer[10240];
		int out_fd = open("canny_recv.png", O_WRONLY | O_CREAT, 0644);
		if (out_fd == -1) perror("open file error");

		std::cout << "0" << std::endl;

		memset( buffer, 0, 10240 );
		bytes_r = read(newsockfd, &buffer, 10240);
		int readLength = atoi(buffer);
		printf("%s,%d\n", buffer, readLength);
		
		memset( buffer, 0, 10240 );
		while ( (bytes_r = read( newsockfd, &buffer, 10240)) > 0 ) {
			bytes_w = write( out_fd, &buffer, (ssize_t) bytes_r);
			if (bytes_w != bytes_r) perror("write error");
			printf("received %ld bytes\n", bytes_r);
			readLength -= (int)bytes_r;
			if (readLength == 0) break;
		}
		printf("Finished receiving\n");
		//bytes_w = write( out_fd, &buffer, (ssize_t) bytes_r);
		//if (bytes_w != bytes_r) perror("write error");
		//printf("received %ld bytes\n", bytes_r);
		
		close( out_fd );


		//////////////////////////////////////////////////////////////////
		//								//
		//	here goes the OpenCV codes to process images		//
		//								//
		//////////////////////////////////////////////////////////////////

		Mat src, src_gray;
		Mat grad;

		int scale = 1;
		int delta = 0;
		int ddepth = CV_16S;

		/// Load an image
		src = imread( "canny_recv.png" );

		if( !src.data )
		{ return; }

		GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

		/// Convert it to gray
		cvtColor( src, src_gray, COLOR_RGB2GRAY );

		/// Generate grad_x and grad_y
		Mat grad_x, grad_y;
		Mat abs_grad_x, abs_grad_y;

		/// Gradient X
		Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
		convertScaleAbs( grad_x, abs_grad_x );

		/// Gradient Y
		Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
		convertScaleAbs( grad_y, abs_grad_y );

		/// Total Gradient (approximate)
		addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

		imwrite( "canny_result.png", grad );  

		//////////////////////////////////////////////////////////////////
		//								//
		//	end of OpenCV code					//
		//								//
		//////////////////////////////////////////////////////////////////


		int in_fd = open("canny_result.png", O_RDONLY );
		if ( in_fd == -1 ) perror( "open read files" );

		memset( buffer, 0, 10240 );
		while ( (bytes_r = read( in_fd, &buffer, 10240 ) ) > 0 ) {
			bytes_w = write( newsockfd, &buffer, (ssize_t)bytes_r );
			if (bytes_w != bytes_r) perror( "socket write error" );
		}
		close( in_fd );
		close( newsockfd );
		std::cout << " loop again " << std::endl;
}

int main ( int argc, char **argv) {
	if ( argc < 2 ) {
		std::cout << "usage: ./server port_number" << std::endl;
		return -1;
	}
	std::cout << "running....\n";
	int port = atoi( argv[1] );
	int sockfd, new_sockfd;

	struct sockaddr_in s_addr, c_addr;
	socklen_t c_len;
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if (sockfd < 0) perror("ERROR opening socket");
	bzero( (char *) &s_addr, sizeof(s_addr) );

	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons( port );

	if ( bind( sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0)
		perror("ERROR on binding");

	listen( sockfd, 5 );

	std::cout << "while start" << std::endl;
	while ( true ) {
		c_len = sizeof( c_addr );

		int newsockfd = accept( sockfd, (struct sockaddr *) &c_addr, &c_len);
		if (newsockfd < 0) perror("ERROR on accept");
		char function[100];
		memset(function, 0, 100);
		read(newsockfd, &function, 100);
		std::string fun = std::string(function);
		std::cout << function << "," << fun << std::endl;
		
		if (fun.compare("canny") == 0){
			canny(newsockfd);
		} else if (fun.compare("PyrMeanShiftFiltering") == 0){
			meanShiftFiltering(newsockfd);
		}
		
	}
	close( sockfd );

	return 0;
}
