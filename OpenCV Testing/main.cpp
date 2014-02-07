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

float hueLow = 200, satLow = 40, valLow = 20, hueHigh = 245, satHigh = 80, valHigh = 65;

Mat process(Mat &img)
{
    // convert to HSV
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
    // generate mask
	Mat mask(size.height, size.width, CV_8UC1);
    
    // (my computer uses, opencv uses)
    // H: (0-360, 0-180)
    // S: (0-100, 0-255)
    // V: (0-100, 0-255)
    float kH = 0.5, kS = 2.55, kV = 2.55;
    inRange(hsv,
            Scalar(kH * hueLow, kS * satLow, kV * valLow),
            Scalar(kH * hueHigh, kS * satHigh, kV * valLow),
            mask);

    Point anchor1(10, 10), anchor2(5, 5);
    Size size1(21, 21), size2(11, 11);
    
    Mat se21 = getStructuringElement(CV_SHAPE_RECT, size1, anchor1);
    Mat se11 = getStructuringElement(CV_SHAPE_RECT, size2, anchor2);
	close(mask, mask, se21);
	open(mask, mask, se11);

    Mat hough_in(size, CV_8U);
    mask.copyTo(hough_in);

    GaussianBlur(hough_in, hough_in, Size(15, 15), 2, 2); // last 2 were 0, 0
    
//    vector<Vec3f> circles;
//    HoughCircles(hough_in, circles, CV_HOUGH_GRADIENT, 4, size.height/10, 100, 140, 20, 0);
    
    vector<vector<Point>> contours;
    findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    
    int contourNum = -1;
    double area = 0;
    
    for (int i = 0; i < contours.size(); ++i) {
        double a = contourArea(contours[i]);
        if (a > area) contourNum = i;
    }
    
    if (contourNum == -1) return mask;
    
    vector<Point> masterContour(contours[contourNum]);
    polylines(mask, masterContour, true, Scalar(255/2));
    
    Point2f centerEnclosing;
    float radiusEnclosing = 0;
    minEnclosingCircle(Mat(masterContour), centerEnclosing, radiusEnclosing);
    
    circle(mask, centerEnclosing, round(radiusEnclosing), 130);
    
//    for (int i = 0; i < circles.size(); ++i) {
//        try {
//            float x = circles[i][0], y = circles[i][1];
//            if (x < 0 || y < 0) continue;
//            if (x > mask.cols || y > mask.rows) continue;
//            Scalar val(mask.at<Scalar>(x, y));
//
//            if (val.val[0] < 1) continue;
//            
//            // drawing out of range on image – drawing off image therefore fails
//            // TRY USING center of gravity to focus more on the right circle
//            // also: tweak ALL parameters of houghCircles to find optimal
//            Point2f center(x, y);
//            
//            circle(img, center, 3, CV_RGB(0, 255, 0));
//            circle(img, center, circles[i][2], CV_RGB(255, 0, 0));
//            circle(mask, center, 3, CV_RGB(0, 255, 0));
//            circle(mask, center, circles[i][3], CV_RGB(255, 0, 0));
//        } catch (int e) {
//            std::cout << "Exception occured: " << e << std::endl;
//        }
//    }
    
    return mask;
}

void test_live()
{
    VideoCapture cam(0);
//    char    hueLowSlide[50],    satLowSlide[50],    valLowSlide[50],
//            hueHighSlide[50],   satHighSlide[50],   valHighSlide[50];
//    const int hueSlideMax = 180, satSlideMax = 255, valSlideMax = 255;
//    double hueSlide, satSlide, valSlide;
    
    
	for (;;) {
        Mat img;
        cam.read(img);
        Mat out = process(img);
        
        flip(img, img, 1);
        flip(out, out, 1);
        
        //imshow("img", img);
        imshow("out", out); // error here
        
        //createTrackbar(hueLowSlide, "hue low", <#int *value#>, <#int count#>)
        
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
