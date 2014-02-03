//
//  main.cpp
//  OpenCV Testing
//
//  Created by Logan Howard on 1/31/14.
//  Copyright (c) 2014 Logan Howard. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include <stdio.h>

using namespace cv;

void open(const Mat src, Mat dst, Mat element)
{
    erode(src, dst, element);
    dilate(src, dst, element);
}

void close(const Mat src, Mat dst, Mat element)
{
	dilate(src, dst, element);
	erode (src, dst, element);
}

Mat process(Mat &img)
{
    // convert to HSV
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
    // generate mask
	Mat mask(size.height, size.width, CV_8UC1);
    
    // divided by 2 because opencv uses hue from 0-180 instead of 0-360
    inRange(hsv, Scalar(210/2, 25, 25, 0), Scalar(270/2, 100, 100, 0), mask);
    
    Point anchor1(10, 10), anchor2(5, 5);
    Size size1(21, 21), size2(11, 11);
    
    Mat se21 = getStructuringElement(CV_SHAPE_RECT, size1, anchor1);
    Mat se11 = getStructuringElement(CV_SHAPE_RECT, size2, anchor2);
	close(mask, mask, se21);
	open(mask, mask, se11);
//
//    Mat hough_in(size, CV_8U);
//    mask.copyTo(hough_in);
//    
//    medianBlur(hough_in, hough_in, 15);
//    GaussianBlur(hough_in, hough_in, Size(15, 15), 0, 0);
//    
//    Mat circles(size.height, size.width, CV_8UC1);
//    HoughCircles(hough_in, circles, CV_HOUGH_GRADIENT, 4, size.height/10, 100, 40, 0, 0);
    
//    for (int i = 0; i < circles.total(); ++i) {
//        Point2f p = circles.at<Point2f>(i, 0);
//        Point center(p.x, p.y);
//        Scalar val = mask.at<Scalar>(center.y, center.x);
//
//        // note: whenever this next part fails, it jumps the memory up by about 30MB. perhaps this is related to memory climbs? memory stays constant when this whole for loop is commented.
//        if (val.val[0] < 1) continue;
//        
//        circle(img, center, 3, CV_RGB(0, 255, 0));
//        //circle(img, center, roundf(p[2]), CV_RGB(255, 0, 0));
//        circle(mask, center, 3, CV_RGB(0, 255, 0));
//        //circle(mask, center, roundf(p[2]), CV_RGB(255, 0, 0));
//    }
    
	//return hough_in;
    return mask;
}

void test_live()
{
    VideoCapture cam(0);
	for (;;) {
        Mat img;
        cam.read(img);
        Mat out = process(img);
        
        flip(img, img, 1);
        flip(out, out, 1);
        
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
