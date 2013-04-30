//
//  ViewController.h
//  Mobile Project
//
//  Created by Linyes on 4/25/13.
//  Copyright (c) 2013 Columbia. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <opencv2/highgui/cap_ios.h>
#import "OffloadHandler.h"

@interface ViewController : UIViewController

@property (retain, nonatomic) UIImage *srcImage;

@property (retain, nonatomic) IBOutlet UIButton *originalButton;
@property (retain, nonatomic) IBOutlet UIImageView *imageView;
@property (retain, nonatomic) IBOutlet UIButton *greyButton;
@property (retain, nonatomic) IBOutlet UIButton *edgeButton;

@end

