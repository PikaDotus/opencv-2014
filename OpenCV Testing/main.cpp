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
#include <chrono>
#include <boost/thread.hpp>

using namespace cv;

int twoGoals(0);
int oneGoal(0);
int noGoals(0);

// (my computer uses, opencv uses)
// H: (0-360, 0-180)
// S: (0-100, 0-255)
// V: (0-100, 0-255)
const float kH = 0.5, kS = 2.55, kV = 2.55;

void detectHot(Mat &img)
{
    int imgType = 0;
    
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
        int numGoals(1);
        if (largestArea[1] > 500 && secondLargestArea[1] > 500) {
            numGoals = 2;
        }
        
        imgType = numGoals;
    }
    
    switch (imgType) {
        case 2:
            ++twoGoals;
            break;
        case 1:
            ++oneGoal;
        case 0:
            ++noGoals;
        default:
            break;
    }
    
    return;
}

void test_hot()
{
    const int totalGoals(6382); // 6382 total
    
    boost::thread_group threadGroup;
    
    for (int curImgNum = 1; curImgNum <= totalGoals; ++curImgNum) {
        Mat img;
        String path(String("/Users/logan/roboimgs/downloaded/img") + std::to_string(curImgNum) + String(".jpg"));
        
        img = imread(path, CV_LOAD_IMAGE_COLOR);
        
        threadGroup.add_thread(new boost::thread(detectHot, img));
        
        printf("img: %d\n", curImgNum);
        
        int pressed(waitKey(10));
        
        if (pressed == 27)
            return;
    }
    
    threadGroup.join_all();
    
    std::cout << std::endl;
    printf("Two goals: %d of %d (%d%%)\n", twoGoals, totalGoals, (int)round(100*(float)twoGoals / (float)totalGoals));
    printf("One goal: %d of %d (%d%%)\n", oneGoal, totalGoals, (int)round(100*(float)oneGoal / (float)totalGoals));
    printf("No goals: %d of %d (%d%%)\n", noGoals, totalGoals, (int)round(100*(float)noGoals / (float)totalGoals));
}

int main()
{
    auto beginTime = std::chrono::high_resolution_clock::now();
    
    test_hot();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = endTime - beginTime;
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    printf("time elapsed (milliseconds): %lld\n", ms);
    
	return 0;
}
