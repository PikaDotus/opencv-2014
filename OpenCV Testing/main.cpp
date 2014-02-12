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

void detectHot(Mat &img)
{
    // bounding values
    const float   hueLow = 160,   satLow = 25,    valLow = 90,
    hueHigh = 190,  satHigh = 75,   valHigh = 100;
    
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
    
    // per contour...
    for (int i = 0; i < contours.size(); ++i) {
        // draw it
        vector<Point> a = contours[i];
        polylines(img, a, false, Scalar(0, 0, 255), 1, CV_AA);
        
        RotatedRect rect(minAreaRect(a));
        
        // draw the rotated rectangle enclosing it
        Point2f vertices[4];
        rect.points(vertices);
        for (int k = 0; k < 4; ++k) {
            line(img, vertices[k], vertices[(k + 1) % 4], Scalar(0, 255, 255), 2);
        }
    }
}

void detectBall(Mat &img)
{
    // bounding values
    const float   hueLow = 220,   satLow = 50,    valLow = 15,
    hueHigh = 235,  satHigh = 80,   valHigh = 88;
    
    // converts img to HSV colors
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
    // creates B&W mask
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
    
    // if no contours, return
    if (contourNum == -1) return;
    
    
    // DANNYIDEA: do expensive ball find, then track the ball
    
    
    // grab the biggest contour
    vector<Point> masterContour(contours[contourNum]);
    
    // find the circle that encloses the biggest blob
    Point2f centerEnclosing;
    float radiusEnclosing = 0;
    minEnclosingCircle(Mat(masterContour), centerEnclosing, radiusEnclosing);
    
    // draw both the circle and the largest blob
    circle(img, centerEnclosing, round(radiusEnclosing), Scalar(0, 255, 0), 2, CV_AA);
    polylines(img, masterContour, true, Scalar(0, 255, 255), 2, CV_AA);
}

void detectBumper(Mat &img)
{
    // bounding values
    const float hueLow = 220,   satLow = 50,    valLow = 15,
                hueHigh = 245,  satHigh = 77,   valHigh = 39;
    
    // converts img to HSV colors
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
    // creates B&W mask
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
    
    // if no contours, return
    if (contourNum == -1) return;
    
    
    // DANNYIDEA: do expensive ball find, then track the ball
    
    
    // grab the biggest contour
    vector<Point> masterContour(contours[contourNum]);
    
    polylines(img, masterContour, false, Scalar(0, 0, 255), 1, CV_AA);
    
    RotatedRect rect(minAreaRect(masterContour));
    Point2f vertices[4];
    rect.points(vertices);
    
    circle(img, rect.center, 3, Scalar(0, 255, 0), 1, CV_AA);
    for (int k = 0; k < 4; ++k) {
        line(img, vertices[k], vertices[(k+1) % 4], Scalar(0, 255, 255), 2);
    }
}

void test_hot()
{
    VideoCapture cam(0);
    
    for (;;) {
        Mat img;
        cam.read(img);
        detectHot(img);
        
        // reverse direction so movement matches more naturally
        flip(img, img, 1);
        imshow("img", img);
            
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
        detectBall(img);
        
        // reverse direction so movement matches more naturally
        flip(img, img, 1);
        imshow("img", img);
        
		if (waitKey(10) == 27)
			return;
	}
}

void test_bumper()
{
    VideoCapture cam(0);
    
    for (;;) {
        Mat img;
        cam.read(img);
        detectBumper(img);
        
        // reverse direction so movement matches more naturally
        flip(img, img, 1);
        imshow("img", img);
        
        if (waitKey(10) == 27)
            return;
    }
}

int main()
{
    namedWindow("out");
    namedWindow("img");
    
    //test_hot();
    //test_ball();
    test_bumper();
    
	return 0;
}
