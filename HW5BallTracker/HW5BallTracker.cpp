// HW5BallTracker.cpp : Defines the entry point for the console application.
// Jingyang Li, Phillip Lopez
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace std;
using namespace cv;

int main()
{
	// Load in video capture
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	// Set some restrictions on the frame
	cap.set(CAP_PROP_FRAME_WIDTH, 320.0);
	cap.set(CAP_PROP_FRAME_HEIGHT, 240.0);
	//cap.set(CAP_PROP_FPS, 10);

	// Setup the display windows
	namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
	namedWindow("CombinedFeats", CV_WINDOW_AUTOSIZE);
	namedWindow("Thresholds", CV_WINDOW_AUTOSIZE);

	// Trackbar default values
	int lowH = 15;
	int highH = 35;
	int lowS = 120;
	int highS = 250;
	int lowV = 150;
	int highV = 255;

	// Create the trackbar
	createTrackbar("Low Hue", "Thresholds", &lowH, 180);
	createTrackbar("High Hue", "Thresholds", &highH, 180);
	createTrackbar("Low Sat", "Thresholds", &lowS, 255);
	createTrackbar("High Sat", "Thresholds", &highS, 255);
	createTrackbar("Low Value", "Thresholds", &lowV, 255);
	createTrackbar("High Value", "Thresholds", &highV, 255);

	// Adjust the window positions so all can be seen
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

	// Loop through each frame provided by the camera
	Mat imgOriginal;
	int numFrames = 0;
	double total_time = 0;
	while (waitKey(1) != 27 && cap.isOpened()) {

		// Start Timer
		clock_t begin = clock();
		bool readSuccess = cap.read(imgOriginal);
		if (!readSuccess) {
			cout << "Error reading camera image";
			return -1;
		}

		Mat imgHSV;
		Mat imgGray;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
		cvtColor(imgOriginal, imgGray, COLOR_BGR2GRAY);

		int lH = getTrackbarPos("Low  Hue", "Thresholds");		// low  Hue
		int uH = getTrackbarPos("High Hue", "Thresholds");		// high Hue
		int lS = getTrackbarPos("Low  Sat", "Thresholds");		// low  Sat
		int uS = getTrackbarPos("High Sat", "Thresholds");		// high Sat
		int lV = getTrackbarPos("Low  Value", "Thresholds");	// low  Value
		int uV = getTrackbarPos("High Value", "Thresholds");	// high Value

		// Apply threshold on HSV image
		Scalar lowerb = Scalar(lH, lS, lV);
		Scalar upperb = Scalar(uH, uS, uV);
		Mat imgThresh;
		inRange(imgHSV, lowerb, upperb, imgThresh);

		// Do some noise removal
		dilate(imgGray, imgGray, Mat::ones(2, 2, CV_32F));
		erode(imgGray, imgGray, Mat::ones(2, 2, CV_32F));
		GaussianBlur(imgGray, imgGray, Size(3, 3), 2, 2);

		Size size = imgThresh.size();
		int nRows = size.height;
		int nCols = size.width;

		// Setup the parameters for the HoughCircles call
		int hough_sub_sampling = 6;
		int min_dist = nRows / 2;
		int canny_max = 200;//200;
		int canny_min = 56;//100;
		int min_radius = 18;//10;
		int max_radius = 85;//120;

		vector<Vec3f> circles;

		HoughCircles(imgGray, circles, HOUGH_GRADIENT, hough_sub_sampling,
			min_dist, canny_max, canny_min, min_radius, max_radius);

		if(circles.size() > 0){
		//for (int i = 0; i < circles.size(); i++) {
			Vec3i c = circles[0];
			if (c[2] <= max_radius) {
				circle(imgOriginal, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), -1, LINE_AA);
				circle(imgOriginal, Point(c[0], c[1]), 2, Scalar(0, 255, 0), -1, LINE_AA);
			}
		}

		// Show the results
		imshow("imgOriginal", imgOriginal);
		imshow("CombinedFeats", imgThresh);

		// Show Timer
		clock_t end = clock();
		if (numFrames <= 1000) {
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			numFrames++;
			total_time += elapsed_secs;
		}
	}

	cout.precision(17);
	cout << "Average Time: " << fixed << (total_time / numFrames) << endl;
	waitKey();
}

