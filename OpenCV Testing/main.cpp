//
//  hot-goal.cpp
//  OpenCV Testing
//
//  Created by Logan Howard on 2/7/14.
//  Copyright (c) 2014 Logan Howard. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include <stdio.h>

using namespace cv;

// (my computer uses, opencv uses)
// H: (0-360, 0-180)
// S: (0-100, 0-255)
// V: (0-100, 0-255)
const float   hueLow = 160,   satLow = 90,    valLow = 55,
hueHigh = 175,  satHigh = 100,   valHigh = 65;
const float kH = 0.5, kS = 2.55, kV = 2.55;

Mat process(Mat &img)
{
    // converts img to HSV colors
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
    // creates mask (returned)
	Mat mask(size.height, size.width, CV_8UC1);
    
    // filters based on simple range bounds
    inRange(hsv,
            Scalar(kH * hueLow,     kS * satLow,    kV * valLow),
            Scalar(kH * hueHigh,    kS * satHigh,   kV * valHigh),
            mask);
    
    // finds contours of all blobs
    vector<vector<Point>> contours;
    findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    
    int contourNum = -1;
    for (int i = 0; i < contours.size(); ++i) {
        vector<Point> a = contours[i];
        RotatedRect rect = minAreaRect(mask);
        Mat box;
        cvBoxPoints(rect, box);
    }
    
    // if there are no contours, just return
    if (contourNum == -1) return mask;
    
    return mask;
}

void test_live()
{
    std::cout << kH * hueLow << " " << kS * satLow << " " << kV * valLow << std::endl;
    std::cout << kH * hueHigh << " " << kS * satHigh << " " << kV * valHigh << std::endl;
    
	for (;;) {
        Mat img = imread("/Users/logan/image2.png");
        Mat out = process(img);
        
        //imshow("img", img);
        imshow("out", out);
        
		if (waitKey(10) == 27)
			return;
	}
}

int main()
{
    namedWindow("out");
    namedWindow("img");
    
    test_live();
    
	return 0;
}
