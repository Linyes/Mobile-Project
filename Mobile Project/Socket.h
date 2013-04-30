// Definition of the Socket class

#ifndef Socket_class
#define Socket_class


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include "opencv2/core/core.hpp"



const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
public:
	Socket() {};
	Socket(std::string host, int port);
	virtual ~Socket();
	
	// Server initialization
	bool create();
	bool bind ( const int port );
	bool listen() const;
	bool accept ( Socket& ) const;
	
	// Client initialization
	bool connect ( const std::string host, const int port );
	
	// Data Transimission
	bool send ( const std::string ) const;
	int recv ( std::string& ) const;
	
	
	void set_non_blocking ( const bool );
	
	bool is_valid() const { return m_sock != -1; }
	
	const Socket& operator << ( const std::string& ) const;
	const Socket& operator << ( const cv::Mat&) const;

	const Socket& operator >> ( std::string& ) const;
	const Socket& operator >> ( cv::Mat&) const;
	
public:
	
	int m_sock;
	sockaddr_in m_addr;
	
	
};


#endif
