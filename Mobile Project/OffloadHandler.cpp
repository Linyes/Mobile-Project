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
OffloadCanny(Mat& image, Mat& edges, double threshold1, double threashold2, int apertureSize, bool L2gradient )
{
	//Canny(image, edges, threshold1, threashold2, apertureSize, L2gradient);
	//return;
	double serial = 0, transmit = 0;
	clock_t start_time = 0, end_time = 0;
	clock_t start_time_t = 0, end_time_t = 0;
	
	start_time = clock();
	// the whole file is now loaded in the memory buffer.
	Socket socket ("209.2.47.149", 30001);
//	Socket socket ("207.10.137.188", 9991);
	imwrite("tmp.png", image);
	
	std::ifstream is ("tmp.png", std::ifstream::binary);
	if (!is) {printf("Read error\n"); return;}
	
	// get length of file:
	is.seekg (0, is.end);
	int length = is.tellg();
	is.seekg (0, is.beg);
	
	char * inBuffer = new char [length];
	
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
	std::ofstream ofs("./recv.png", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
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
	
	
	edges = imread("recv.png", 1);
	if (!edges.data) {std::cout << "imread error\n"; return;}
	end_time = clock();
	serial += (double)(end_time - start_time);
	
	printf("Serialation time is %f\nTransmition time is %f\n", serial, transmit);
	
	//Canny(image, edges, threshold1, threashold2, apertureSize, L2gradient);
	
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