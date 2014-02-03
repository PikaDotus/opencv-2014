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
	//fprintf(stderr, "Processing image\n");
    
	/* Convert to HSV */
	Size size(img.cols, img.rows);
	Mat hsv(size, CV_8U);
	cvtColor(img, hsv, CV_BGR2HSV);
    
	/* Generate mask */
	Mat mask(size.height, size.width, CV_8UC1);
    
    // divided by 2 because opencv uses hue from 0-180 instead of 0-360
    inRange(hsv, Scalar(210/2, 25, 25, 0), Scalar(270/2, 100, 100, 0), mask);
    
	//cvReleaseImage(&hsv);
	//IplImage *tmp = cvCreateImage(size, 8, 1);
	//cvCopy(mask, tmp, NULL);
	//return mask;
    
	// Perform morphological ops
	//IplConvKernel *se21 = cvCreateStructuringElementEx(21, 21, 10, 10, CV_SHAPE_RECT, NULL);
	//IplConvKernel *se11 = cvCreateStructuringElementEx(11, 11, 5,  5,  CV_SHAPE_RECT, NULL);
    
    Point anchor1(10, 10), anchor2(5, 5);
    Size size1(21, 21), size2(11, 11);
    
    Mat se21 = getStructuringElement(CV_SHAPE_RECT, size1, anchor1);
    Mat se11 = getStructuringElement(CV_SHAPE_RECT, size2, anchor2);
	close(mask, mask, se21);
	open(mask, mask, se11);
    
	//IplImage *tmp = cvCreateImage(size, 8, 1);
	//cvCopy(mask, tmp, NULL);
	//return mask;
    
	// Hough transform
	//IplImage *hough_in = cvCreateImage(size, 8);
    Mat hough_in(size, CV_8U);
    mask.copyTo(hough_in);
    
	// CvMemStorage *storage = cvCreateMemStorage(0);
    
    //cvSmooth(hough_in, hough_in, CV_GAUSSIAN, 15, 15, 0, 0);
    medianBlur(hough_in, hough_in, 15);
    GaussianBlur(hough_in, hough_in, Size(15, 15), 0, 0);
    
    Mat circles(size.height, size.width, CV_8UC1);
    HoughCircles(hough_in, circles, CV_HOUGH_GRADIENT, 4, size.height/10, 100, 40, 0, 0);
    
//	CvSeq *circles = cvHoughCircles(
//                                    hough_in, storage, // input, storage,
//                                    CV_HOUGH_GRADIENT, 4, size.height/10,
//                                    // type, 1/scale, min center dists
//                                    100, 40,           // params1?, param2?
//                                    0, 0               // min radius, max radius
//                                    );
//	cvReleaseMemStorage(&storage);
    
    circles.resize(1);
    
    for (int i = 0; i < circles.total(); ++i) {
        Mat p = circles.at<Mat>(i);
        Point center(round(p.at<double>(0)), round(p.at<double>(1)));
        Scalar val = cvGet2D(&mask, center.y, center.x);
        if (val.val[0] < 1) continue;
        circle(img, center, 3, CV_RGB(0, 255, 0));
        circle(img, center, round(p.at<double>(2)), CV_RGB(255, 0, 0));
        circle(mask, center, 3, CV_RGB(0, 255, 0));
        circle(mask, center, round(p.at<double>(2)), CV_RGB(255, 0, 0));
    }
    
    
//	// Fancy up output
//	for (int i = 0; i < circles->total; ++i) {
//        float *p = (float*) cvGetSeqElem(circles, i);
//        CvPoint center = cvPoint(cvRound(p[0]), cvRound(p[1]));
//        CvScalar val = cvGet2D(&mask, center.y, center.x);
//        if (val.val[0] < 1) continue;
//        cvCircle(&img,  center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
//        cvCircle(&img,  center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
//        cvCircle(mask, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
//        cvCircle(mask, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
//	}
    
	return hough_in;
}

void test_live(VideoCapture cam)
{
	while (1) {
        Mat img;
        cam.read(img);
        Mat out = process(img);
        
        imshow("img", img);
        imshow("out", out);
        
		if (waitKey(30) == 27)
			return;
	}
}

int main()
{
    namedWindow("out");
    moveWindow("out", 10, 10);
    namedWindow("img");
    moveWindow("img", 10, 10);
    
    VideoCapture cam(0);
    
    test_live(cam);
    
	return 0;
}
