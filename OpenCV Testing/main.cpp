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
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;
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
    VideoCapture vcap(0);
    Mat img;

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    
    boost::thread_group threads;
//    const std::string videoStreamAddress =
//        "http://USER:PWD@IPADDRESS:8088/mjpeg.cgi?user=USERNAME&password=PWD&channel=0&.mjpg";
    
//    if (!vcap.open(videoStreamAddress)) {
//        std::cout << "Error opening video stream or file" << std::endl;
//        return;
//    }

    // get 10 frames
    for (int i = 0; i < 4; ++i) {
        if (!vcap.read(img)) {
            std::cout << "No frame" << std::endl;
            break;
        }
        
        threads.add_thread(new boost::thread(detectHot, img));
    }
    
    threads.join_all();
    
    std::chrono::steady_clock::time_point then = std::chrono::steady_clock::now();
    std::chrono::duration<float> time_taken =
        std::chrono::duration_cast<std::chrono::duration<float>>(now - then);
    std::cout << "time taken: " << time_taken.count() << std::endl << std::endl;
}

void printGoalStats()
{
    const int totalGoals(twoGoals + oneGoal + noGoals);
    
    std::cout << std::endl;
    printf("Two goals: %d of %d (%d%%)\n", twoGoals, totalGoals,
           (int)round(100*(float)twoGoals / (float)totalGoals));
    printf("One goal: %d of %d (%d%%)\n", oneGoal, totalGoals,
           (int)round(100*(float)oneGoal / (float)totalGoals));
    printf("No goals: %d of %d (%d%%)\n", noGoals, totalGoals,
           (int)round(100*(float)noGoals / (float)totalGoals));
    std::cout << std::endl;
}

int main()
{
    unsigned short const PORT = 1180; // the port that the server listens on (49153)
    boost::asio::io_service ioService;
    
    // run the main program
    test_hot();
    
    // get results and decide if hot or not
    const int totalGoals(twoGoals + oneGoal + noGoals);
    String message;
    if ((100*(float)twoGoals / (float)totalGoals) > 0.5) message = "1";
    else message = "0";
    
    try {
        tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), PORT));
        
        // listen for clients
        std::cout << "Listening for client..." << std::endl;
        printGoalStats();
        tcp::socket socket(ioService);
        acceptor.accept(socket);
        std::cout << "Client heard..." << std::endl;
        
        // send string to client
        boost::asio::write(socket, boost::asio::buffer(message));
    } catch (Exception e) {
        std::cerr << e.what() << std::endl;
    }
    
	return 0;
}
