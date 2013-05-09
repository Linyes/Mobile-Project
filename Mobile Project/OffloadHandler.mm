//
//  OffloadHandler.cpp
//  OpenCV Tutorial
//
//  Created by Linyes on 4/24/13.
//
//

#include "OffloadHandler.h"
#include "SocketException.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

namespace cv
{
void
OffloadPyrMeanShiftFiltering( Mat& src, Mat& dst, double sp, double sr)
{
	std::string basePath = OffloadHandler::getBasePath();
	std::string tmpFile = basePath + "/tmp.png";
	std::string recvFile = basePath +"/recv.png";
	printf("%s\n", tmpFile.c_str());
	
	double serial = 0, transmit = 0;
	clock_t start_time = 0, end_time = 0;
	clock_t start_time_t = 0, end_time_t = 0;
	
	start_time = clock();
	// the whole file is now loaded in the memory buffer.
	std::string host = "160.39.154.113";
	int port = 30002;
	Socket socket (host, port);
	//	Socket socket ("ec2-54-242-96-206.compute-1.amazonaws.com", 9992);
	
	std::string function = "PyrMeanShiftFiltering";
	::write(socket.m_sock, function.c_str(), function.length()+1);
	
	////////////////////////////////////
	// Serialization for trasnmission //
	////////////////////////////////////
	if(imwrite(tmpFile, src)){
		printf("imwrite success!\n");
	} else {
		printf("imwrite failed!\n");
	}
	
	////////////////////////
	// Start sending data //
	////////////////////////
	std::ifstream is (tmpFile, std::ifstream::binary);
	if (!is) {printf("Read error\n"); return;}
	
	
	// get length of file:
	is.seekg (0, is.end);
	int length = is.tellg();
	is.seekg (0, is.beg);
	
	//Send length
	std::string lengthStr = std::to_string(length);
	::write(socket.m_sock, lengthStr.c_str(), lengthStr.length()+1);
	
	char * inBuffer = new char [length];
	printf("lenght is %d", length);
	is.read (inBuffer,length);
	if (is){
		end_time = clock();
		
		serial = (double)end_time - start_time;
		printf("time %f\n", serial);
		start_time_t = clock();
		::write(socket.m_sock, inBuffer, length);
		printf("Send %d successfully!\n", length);
	} else {
		printf("Send error!\n");
		return;
	}
	
	end_time_t = clock();
	transmit += (double)end_time_t - start_time_t;
	/*
	 char buffer[1024];
	 int input_fd;
	 ssize_t bytes_read, bytes_written;
	 input_fd = open("tmp.png", O_RDONLY);
	 if (input_fd == -1) perror("open file");
	 while ((bytes_read = ::read(input_fd, &buffer, 1024)) > 0) {
	 bytes_written = ::write(socket.m_sock, &buffer, (ssize_t) bytes_read);
	 if (bytes_written != bytes_read) perror("write error");
	 }
	 */
	///////////////////////////////
	// receive and write to file //
	///////////////////////////////
	std::ofstream ofs(recvFile, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	ssize_t bytes_read;
	char *buffer = new char[1024];
	memset(buffer, 0, 1024);
	
	while (true) {
		start_time_t = clock();
		bytes_read = ::read(socket.m_sock, buffer, 1024);
		if (bytes_read <= 0) break;
		end_time_t = clock();
		transmit += (double)(end_time_t - start_time_t);
		
		
		ofs.write(buffer, bytes_read);
		//if (bytes_written != bytes_read) perror("write error");
	}
	ofs.close();
	
	
	dst = imread(recvFile, 1);
	if (!dst.data) {std::cout << "imread error\n"; return;}
	end_time = clock();
	serial += (double)(end_time - start_time);
	
	printf("Serialation time is %f\nTransmition time is %f\n", serial, transmit);

	
}
	
void
OffloadCanny(Mat& image, Mat& edges, double threshold1, double threashold2, int apertureSize, bool L2gradient )
{
	
	std::string basePath = OffloadHandler::getBasePath();
	std::string tmpFile = basePath + "/tmp.png";
	std::string recvFile = basePath +"/recv.png";
	printf("%s\n", tmpFile.c_str());

	double serial = 0, transmit = 0;
	clock_t start_time = 0, end_time = 0;
	clock_t start_time_t = 0, end_time_t = 0;
	
	start_time = clock();
	// the whole file is now loaded in the memory buffer.
	std::string host = "160.39.154.113";
	int port = 30002;
	Socket socket (host, port);
//	Socket socket ("ec2-54-242-96-206.compute-1.amazonaws.com", 9992);
	
	std::string function = "canny";
	::write(socket.m_sock, function.c_str(), function.length()+1);
	
	////////////////////////////////////
	// Serialization for trasnmission //
	////////////////////////////////////
	if(imwrite(tmpFile, image)){
		printf("imwrite success!\n");
	} else {
		printf("imwrite failed!\n");
	}
	
	////////////////////////
	// Start sending data //
	////////////////////////
	std::ifstream is (tmpFile, std::ifstream::binary);
	if (!is) {printf("Read error\n"); return;}
	
	
	// get length of file:
	is.seekg (0, is.end);
	int length = is.tellg();
	is.seekg (0, is.beg);
	
	//Send length
	std::string lengthStr = std::to_string(length);
	::write(socket.m_sock, lengthStr.c_str(), lengthStr.length()+1);
	
	char * inBuffer = new char [length];
	printf("lenght is %d", length);
	is.read (inBuffer,length);
	if (is){
		end_time = clock();
		
		serial = (double)end_time - start_time;
		printf("time %f\n", serial);
		start_time_t = clock();
		::write(socket.m_sock, inBuffer, length);
		printf("Send %d successfully!\n", length);
	} else {
		printf("Send error!\n");
		return;
	}

	end_time_t = clock();
	transmit += (double)end_time_t - start_time_t;
	/*
	char buffer[1024];
	int input_fd;
	ssize_t bytes_read, bytes_written;
	input_fd = open("tmp.png", O_RDONLY);
	if (input_fd == -1) perror("open file");
	while ((bytes_read = ::read(input_fd, &buffer, 1024)) > 0) {
		bytes_written = ::write(socket.m_sock, &buffer, (ssize_t) bytes_read);
		if (bytes_written != bytes_read) perror("write error");
	}
*/	
	///////////////////////////////
	// receive and write to file //
	///////////////////////////////
	std::ofstream ofs(recvFile, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	ssize_t bytes_read;
	char *buffer = new char[1024];
	memset(buffer, 0, 1024);
	
	while (true) {
		start_time_t = clock();
		bytes_read = ::read(socket.m_sock, buffer, 1024);
		if (bytes_read <= 0) break;
		end_time_t = clock();
		transmit += (double)(end_time_t - start_time_t);
		
		
		ofs.write(buffer, bytes_read);
		//if (bytes_written != bytes_read) perror("write error");
	}
	ofs.close();
	
	
	edges = imread(recvFile, 1);
	if (!edges.data) {std::cout << "imread error\n"; return;}
	end_time = clock();
	serial += (double)(end_time - start_time);
	
	printf("Serialation time is %f\nTransmition time is %f\n", serial, transmit);
	
	//Canny(image, edges, threshold1, threashold2, apertureSize, L2gradient);
	
}
	
std::string
OffloadHandler::getBasePath()
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	
	std::string *result = new std::string([basePath UTF8String]);
	
	return *result;
	
}
	
 void
OffloadHandler::stringProcess(std::string input)
{
 
	std::string reply;
	Socket socket(m_host, m_port);
	try
	{
		socket << input;
		socket >> reply;
	}
	catch ( SocketException& ) {}
	
	std::cout << "We received this response from the server:\n\"" << reply << "\"\n";
	
}

Mat
OffloadHandler::imageProcess(cv::Mat input)
{
	imwrite("tmp.png", input);
	Socket socket(m_host, m_port);
	return input;
	cv::Mat reply;
	socket << input;
	socket >> reply;
	
	return reply;
	
}

}