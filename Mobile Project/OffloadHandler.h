//
//  OffloadHandler.h
//  OpenCV Tutorial
//
//  Created by Linyes on 4/24/13.
//
//

#include "Socket.h"
#include "opencv2/core/core.hpp"

namespace cv
{
	class OffloadHandler {
	public:
		OffloadHandler() : m_host("localhost"), m_port(30000) { }
		
		OffloadHandler(std::string host, int port) {m_host = host; m_port = port;};
		
		Mat
		imageProcess(Mat input);
		
		static std::string
		getBasePath();
		
		
		void
		stringProcess(std::string input);
		
	public:
		std::string m_host;
		int m_port;
	};
	
	void
	OffloadCanny( Mat& image, Mat& edges,
		     double threshold1, double threshold2,
		     int apertureSize=3, bool L2gradient=false );
	
	void
	OffloadPyrMeanShiftFiltering( Mat& src, Mat& dst,
				   double sp, double sr);
}
