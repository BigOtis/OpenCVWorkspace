// HW5BallTracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
	VideoCapture cap(1);
	// a dasd

	if (!cap.isOpened()) {
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
	namedWindow("CombinedFeats", CV_WINDOW_AUTOSIZE);
	namedWindow("Thresholds", CV_WINDOW_AUTOSIZE);

	//    if


	int lowH = 15;
	int highH = 35;
	int lowS = 120;
	int highS = 250;
	int lowV = 150;
	int highV = 255;

	createTrackbar("Low Hue", "Thresholds", &lowH, 180);
	createTrackbar("High Hue", "Thresholds", &highH, 180);
	createTrackbar("Low Sat", "Thresholds", &lowS, 255);
	createTrackbar("High Sat", "Thresholds", &highS, 255);
	createTrackbar("Low Value", "Thresholds", &lowV, 255);
	createTrackbar("High Value", "Thresholds", &highV, 255);

	moveWindow("imgOriginal", 0, 35);
	moveWindow("CombinedFeats", 0, 600);
	resizeWindow("Thresholds", 640, 380);
	moveWindow("Thresholds", 640, 35);
	
	setTrackbarPos("Low  Hue", "Thresholds", lowH);
	setTrackbarPos("High Hue", "Thresholds", highH);
	setTrackbarPos("Low  Sat", "Thresholds", lowS);
	setTrackbarPos("High Sat", "Thresholds", highS);
	setTrackbarPos("Low  Value", "Thresholds", lowV);
	setTrackbarPos("High Value", "Thresholds", highV);

	Mat imgOriginal;
	while (waitKey(1) != 27 && cap.isOpened()) {

		bool readSuccess = cap.read(imgOriginal);
		if (!readSuccess) {
			cout << "Error reading camera image";
			return -1;
		}

		Mat imgHSV;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

		int lH = getTrackbarPos("Low  Hue", "Thresholds");		// low  Hue
		int uH = getTrackbarPos("High Hue", "Thresholds");		// high Hue
		int lS = getTrackbarPos("Low  Sat", "Thresholds");		// low  Sat
		int uS = getTrackbarPos("High Sat", "Thresholds");		// high Sat
		int lV = getTrackbarPos("Low  Value", "Thresholds");	// low  Value
		int uV = getTrackbarPos("High Value", "Thresholds");	// high Value

		// "Method from the other web page"
		Scalar lowerb = Scalar(lH, lS, lV);
		Scalar upperb = Scalar(uH, uS, uV);
		Mat imgThresh;
		inRange(imgHSV, lowerb, upperb, imgThresh);

		dilate(imgThresh, imgThresh, Mat::ones(2, 2, CV_32F));
		erode(imgThresh, imgThresh, Mat::ones(2, 2, CV_32F));
		GaussianBlur(imgThresh, imgThresh, Size(3, 3), 2, 2);

		Size size = imgThresh.size();
		int nRows = size.height;
		int nCols = size.width;

		int hough_sub_sampling = 4;
		int min_dist = nRows / 5;
		int canny_max = 200;
		int canny_min = 56;
		int min_radius = 18;
		int max_radius = 85;

		vector<Vec3f> circles;
		HoughCircles(imgThresh, circles, HOUGH_GRADIENT, hough_sub_sampling,
			min_dist, canny_max, canny_min);

		if (circles.size() > 0) {
			Vec3i c = circles[0];
			if (c[2] > max_radius) {
				circle(imgOriginal, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, LINE_AA);
				circle(imgOriginal, Point(c[0], c[1]), 2, Scalar(0, 255, 0), 3, LINE_AA);
			}
		}

		imshow("imgOriginal", imgOriginal);
	}
}

