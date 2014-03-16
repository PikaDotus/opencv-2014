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

vector<Point> detectLargestBallContour(Mat &img)
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
    if (contourNum == -1) return vector<Point>(NULL);
    
    return vector<Point>(contours[contourNum]);
}

Point2f detectBallCenter(vector<Point> masterContour)
{
    if (masterContour == vector<Point>(NULL)) return Point2f(-1, -1);
    
    Point2f centerEnclosing;
    float radiusEnclosing = 0;
    minEnclosingCircle(Mat(masterContour), centerEnclosing, radiusEnclosing);
    
    return centerEnclosing;
}

float detectBallRadius(vector<Point> masterContour)
{
    if (masterContour == vector<Point>(NULL)) return -1;
    
    Point2f centerEnclosing;
    float radiusEnclosing = 0;
    minEnclosingCircle(Mat(masterContour), centerEnclosing, radiusEnclosing);
    
    return radiusEnclosing;
}

void test_ball()
{
    VideoCapture cam(0);
    
	for (;;) {
        Mat img;
        cam.read(img);
        
        vector<Point> masterContour(detectLargestBallContour(img));
        
        Point2f center(detectBallCenter(img));
        float radius(detectBallRadius(img));
        
        std::cout << "\ncenter: " << center << std::endl;
        std::cout << "radius: " << radius << std::endl;
	}
}

int main(int argc, const char* argv[])
{
    test_ball();
    
	return 0;
}
