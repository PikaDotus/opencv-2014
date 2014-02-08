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

// (my computer uses, opencv uses)
// H: (0-360, 0-180)
// S: (0-100, 0-255)
// V: (0-100, 0-255)
const float kH = 0.5, kS = 2.55, kV = 2.55;

Mat detectHot(Mat &img)
{
    const float   hueLow = 160,   satLow = 90,    valLow = 55,
    hueHigh = 175,  satHigh = 100,   valHigh = 65;
    
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
        polylines(mask, a, false, Scalar(255));
        
        RotatedRect rect(minAreaRect(a));
        //circle(mask, rect.center, 2, Scalar(255));
    }
    
    // if there are no contours, just return
    if (contourNum == -1) return mask;
    
    polylines(mask, contours, false, Scalar(255));
    
    return mask;
}

Mat detectBall(Mat &img)
{
    const float   hueLow = 220,   satLow = 50,    valLow = 15,
    hueHigh = 235,  satHigh = 80,   valHigh = 88;
    
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
    
    // stuff to shrink then grow the image
    Point anchor1(10, 10), anchor2(5, 5);
    Size size1(21, 21), size2(11, 11);
    
    Mat se21 = getStructuringElement(CV_SHAPE_RECT, size1, anchor1);
    Mat se11 = getStructuringElement(CV_SHAPE_RECT, size2, anchor2);
	close(mask, mask, se21);
	open(mask, mask, se11);
    
    // blur image
    GaussianBlur(mask, mask, Size(15, 15), 2, 2); // try 2,2 for last
    
    // finds contours of all blobs
    vector<vector<Point>> contours;
    findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    
    // finds max area of any contour and gives its position
    int contourNum = -1;
    double area = 0;
    for (int i = 0; i < contours.size(); ++i) {
        double a = contourArea(contours[i]);
        if (a > area) contourNum = i;
    }
    
    // if there are no contours, just return
    if (contourNum == -1) return mask;
    
    
    // DANNYIDEA: do expensive ball find, then track the ball
    
    
    // grab the biggest contour
    vector<Point> masterContour(contours[contourNum]);
    
    // find the circle that encloses the biggest blob
    Point2f centerEnclosing;
    float radiusEnclosing = 0;
    minEnclosingCircle(Mat(masterContour), centerEnclosing, radiusEnclosing);
    
    // draw the enclosing circle and WHAT ARE POLYLINES THERE FOR
    circle(mask, centerEnclosing, round(radiusEnclosing), Scalar(255), 1, CV_AA);
    polylines(mask, masterContour, true, Scalar(200));
    
    return mask;
}

void test_hot()
{
    Mat img = imread("/Users/logan/image2.png");
    Mat out = detectHot(img);
    
    for (;;) {
        //imshow("img", img);
        imshow("out", out);
            
        if (waitKey(10) == 27)
            return;
    }
}

void test_ball()
{
    VideoCapture cam(0);
    
	for (;;) {
        Mat img;
        cam.read(img);
        Mat out = detectBall(img);
        
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
    
    test_hot();
    //test_ball();
    
	return 0;
}
