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

void drawRectangles(vector<Point> contour, Mat dst) {
    RotatedRect rect(minAreaRect(contour));
    
    // draw the rotated rectangle enclosing it
    Point2f vertices[4];
    rect.points(vertices);
    
    for (int k = 0; k < 4; ++k) {
        line(dst, vertices[k], vertices[(k + 1) % 4], Scalar(0, 255, 255), 2);
    }
}

// (my computer uses, opencv uses)
// H: (0-360, 0-180)
// S: (0-100, 0-255)
// V: (0-100, 0-255)
const float kH = 0.5, kS = 2.55, kV = 2.55;

void detectHot(Mat &img)
{
    // bounding values
    const float   hueLow = 170,   satLow = 17,    valLow = 90,
    hueHigh = 190,  satHigh = 95,   valHigh = 100;
    
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
    vector<float> largestArea(2, 0);
    vector<float> secondLargestArea(2, 0);
    
    findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    
    // per contour...
    for (int i = 0; i < contours.size(); ++i) {
        float area = contourArea(contours[i]);
        
        if (area > largestArea[1]) {
            largestArea[0] = i;
            largestArea[1] = area;
        } else if (area > secondLargestArea[1]) {
            secondLargestArea[0] = i;
            secondLargestArea[1] = area;
        }
    }
    
    if (contours.size() >= 2) {
        vector<Point> largestContour = contours[largestArea[0]];
        vector<Point> secondLargestContour = contours[secondLargestArea[0]];

        drawRectangles(largestContour, img);
        drawRectangles(secondLargestContour, img);
        
        int numGoals(1);
        if (largestArea[1] > 500 && secondLargestArea[1] > 500) {
            numGoals = 2;
        }
        
        putText(img, std::to_string(numGoals), Point(img.cols/2, img.rows/2), FONT_HERSHEY_SIMPLEX, 4, Scalar(30, 30, 255), 3);
    }
}

void test_hot()
{
    int curImgNum(1);
    
    for (;;) {
        Mat img;
        String path(String("/Users/logan/roboimgs/downloaded/img") + std::to_string(curImgNum) + String(".jpg"));
        
        img = imread(path, CV_LOAD_IMAGE_COLOR);
        detectHot(img);
        
        putText(img, std::to_string(curImgNum), Point(img.cols-140, img.rows-30), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(30, 255, 30), 2);
        imshow("img", img);
        
        int pressed(waitKey(10));
        
        if (pressed == 27)
            return;
        else if (pressed != -1 && curImgNum < 6382)
            ++curImgNum;
    }
}

int main()
{
    namedWindow("out");
    namedWindow("img");
    
    test_hot();
    
	return 0;
}
