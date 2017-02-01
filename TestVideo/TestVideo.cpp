/**
@file videocapture_basic.cpp
@brief A very basic sample for using VideoCapture and VideoWriter
@author PkLab.net
@modified Phillip Lopez - pgl5711@rit.edu
@date Aug 24, 2016
*/

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
	Mat frame;
	//--- INITIALIZE VIDEOCAPTURE
	VideoCapture cap;
	// open the default camera using default API
	cap.open(0);
	// OR advance usage: select any API backend
	int deviceID = 1;             // 0 = open default camera
	int apiID = cv::CAP_ANY;      // 0 = autodetect default API
								  // open selected camera using selected API
	cap.open(deviceID + apiID);
	// check if we succeeded
	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	//--- GRAB AND WRITE LOOP
	cout << "Start grabbing" << endl
		<< "Press any key to terminate" << endl;
	for (;;)
	{
		// wait for a new frame from camera and store it into 'frame'
		cap.read(frame);
		// check if we succeeded
		if (frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}

		// apply a blur to the frame
		Mat dst;
		boxFilter(frame, dst, -1, Size(4, 4));

		// Find Canny edges detector
		Mat gray;
		cvtColor(dst, gray, COLOR_BGR2GRAY);

		Mat edge1;
		Mat cedge;
		Canny(gray, edge1, 30, 40, 3);

		Mat color;
		cvtColor(edge1, color, CV_GRAY2BGR);

		// Remove the B and R channels to make this look like the matrix
		Mat BGRChannels[3];
		split(color, BGRChannels); // split the BGR channesl
		BGRChannels[0] = Mat::zeros(color.rows, color.cols, CV_8UC1);// removing Green channel
		BGRChannels[2] = Mat::zeros(color.rows, color.cols, CV_8UC1);// removing Green channel
		merge(BGRChannels, 3, color); // pack the image 
		// minor change
		// show live and wait for a key with timeout long enough to show images
		imshow("Live", color);
		if (waitKey(5) >= 0)
			break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}