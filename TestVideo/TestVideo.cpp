/**
@file videocapture_basic.cpp
@brief A very basic sample for using VideoCapture and VideoWriter
@author PkLab.net
@modified Phillip Lopez - pgl5711@rit.edu
@date 2/1/2017
*/

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) {  // check if we succeeded
		cout << "Unable to open the default camera";
		return -1;
	}

	// Make the resolution bigger
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

	// Video writer setup DIVX (For Windows)
	Mat src;
	cap >> src;
	bool isColor = (src.type() == CV_8UC3);
	VideoWriter writer;
	int codec = CV_FOURCC('M', 'P', '4', 'V');  // select desired codec (must be available at runtime)
	double fps = 25.0;                          // framerate of the created video stream
	string filename = "./matrix_test.mp4";             // name of the output video file
	writer.open(filename, codec, fps, src.size(), isColor);

	// Find edges, display them to user, save them to a file
	Mat edges;
	namedWindow("edges", 1);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		cvtColor(frame, edges, COLOR_BGR2GRAY);

		// Apply Canny to the new frame
		GaussianBlur(edges, edges, Size(5, 5), 3, 1.5);
		Canny(edges, edges, 30, 40, 3);

		// Remove the B and R channels to make this look like the matrix
		cvtColor(edges, edges, CV_GRAY2BGR);
		Mat BGRChannels[3];
		split(edges, BGRChannels); // split the BGR channesl
		BGRChannels[0] = Mat::zeros(edges.rows, edges.cols, CV_8UC1);// removing Green channel
		BGRChannels[2] = Mat::zeros(edges.rows, edges.cols, CV_8UC1);// removing Green channel
		merge(BGRChannels, 3, edges); // pack the image 

		writer.write(edges);
		imshow("edges", edges);
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}