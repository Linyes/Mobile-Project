//
//  ViewController.m
//  Mobile Project
//
//  Created by Linyes on 4/25/13.
//  Copyright (c) 2013 Columbia. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
	//UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Hello!" message:@"Welcome to OpenCV" delegate:self cancelButtonTitle:@"Continue" otherButtonTitles:nil];
	//[alert show];
	
	_srcImage = [UIImage imageNamed:@"Lenna.png"];
	
	[_imageView setImage:_srcImage];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_imageView release];
	[_greyButton release];
	[_originalButton release];
	[_edgeButton release];
    [super dealloc];
}

#pragma mark - Handling user interaction
- (IBAction)changeToGray:(id)sender {
	
	cv::Mat inputMat = [self cvMatFromUIImage:_srcImage];
	// read image into mat
//	cv::Mat inputMat = cv::imread("tmp.png", 1);
//	if (!inputMat.data) {std::cout << "imread error\n"; return;}

	
	cv::Mat grayMat, edges, outputMat;
	
	cv::cvtColor(inputMat, grayMat, CV_BGR2GRAY);
	
	cv::Canny(grayMat, edges, 250, 250, 3);

	
	
	//Show output
	cv::cvtColor(edges, outputMat, CV_GRAY2BGRA);

	[_imageView setImage:[self UIImageFromCVMat:outputMat]];
	
	//UIImage *grayImage = [self UIImageFromCVMat:grayMat];
	//[_imageView setImage:grayImage];
	
}

- (IBAction)changeToOrin:(id)sender {
	[_imageView setImage:[UIImage imageNamed:@"Lenna.png"]];
}

- (IBAction)edgeDetection:(id)sender {
	cv::Mat src, gray, edges, outputMat;
	
	src = [self cvMatFromUIImage:_srcImage];
	cv::cvtColor(src, gray, CV_BGR2GRAY);
	
	int m_cannyLoThreshold = 250;
	int m_cannyHiThreshold = 250;
	
	///////////////////////
	// Local computation //
	///////////////////////
	NSDate *start = [NSDate date];
	cv::Canny(gray, edges, m_cannyLoThreshold, m_cannyHiThreshold, 3);
	
	NSDate *end = [NSDate date];
	
	NSTimeInterval localTime = [end timeIntervalSince1970] - [start timeIntervalSince1970];
	
	NSLog(@"Local time is:%f", localTime);

	
	////////////////////////
	// Remote computation //
	////////////////////////
	start = [NSDate date];
//	for (int i = 0; i < 1000; i++) {
		cv::OffloadCanny(gray, edges, m_cannyLoThreshold, m_cannyHiThreshold);
//	}
	
	end = [NSDate date];
	NSTimeInterval networkTime = [end timeIntervalSince1970] - [start timeIntervalSince1970];
	
	NSLog(@"Network time is:%f", networkTime);
	
	NSString *message = [NSString stringWithFormat:@"Local time is: %f\nNetwork time is: %f",localTime, networkTime];
	UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"!!Result!!" message:message delegate:self cancelButtonTitle:@"Continue" otherButtonTitles:nil];
	[alert show];
	
	
	[_imageView setImage:[self UIImageFromCVMat:edges]];
}

#pragma mark - openCV helper
- (cv::Mat)cvMatFromUIImage:(UIImage *)image
{
	CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
	CGFloat cols = image.size.width;
	CGFloat rows = image.size.height;
	
	cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels
	
	CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to  data
							cols,                       // Width of bitmap
							rows,                       // Height of bitmap
							8,                          // Bits per component
							cvMat.step[0],              // Bytes per row
							colorSpace,                 // Colorspace
							kCGImageAlphaNoneSkipLast |
							kCGBitmapByteOrderDefault); // Bitmap info flags
	
	CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
	CGContextRelease(contextRef);
	CGColorSpaceRelease(colorSpace);
	
	return cvMat;
}

-(UIImage *)UIImageFromCVMat:(cv::Mat)cvMat
{
	NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
	CGColorSpaceRef colorSpace;
	
	if (cvMat.elemSize() == 1) {
		colorSpace = CGColorSpaceCreateDeviceGray();
	} else {
		colorSpace = CGColorSpaceCreateDeviceRGB();
	}
	
	CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data);
	
	// Creating CGImage from cv::Mat
	CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
					    cvMat.rows,                                 //height
					    8,                                          //bits per component
					    8 * cvMat.elemSize(),                       //bits per pixel
					    cvMat.step[0],                            //bytesPerRow
					    colorSpace,                                 //colorspace
					    kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
					    provider,                                   //CGDataProviderRef
					    NULL,                                       //decode
					    false,                                      //should interpolate
					    kCGRenderingIntentDefault                   //intent
					    );
	
	
	// Getting UIImage from CGImage
	UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
	CGImageRelease(imageRef);
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(colorSpace);
	
	return finalImage;
}

@end

