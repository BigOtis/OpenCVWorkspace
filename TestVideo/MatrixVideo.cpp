/**
@file videocapture_basic.cpp
@brief Reads in video from the default camera and makes it look like the matrix. 
	   Data is saved to a file named matrix.mp4
@author Phillip Lopez - pgl5711@rit.edu
@date 2/1/2017
 Made with the help of examples from: https://github.com/opencv/opencv/tree/master/samples/cpp
 See "videocapture_basic.cpp"
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
		cout << "Unable to open the default camera. Press enter to quit.";
		waitKey();
		return -1;
	}

	// Make the resolution bigger
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

	// Video writer setup DIVX (For Windows AVI) or MP4V
	Mat src;
	cap >> src;
	bool isColor = (src.type() == CV_8UC3);
	VideoWriter writer;
	int codec = CV_FOURCC('M', 'P', '4', 'V');  // select desired codec (must be available at runtime)
	double fps = 17.0;                          // framerate of the created video stream
	string filename = "./matrix.mp4";             // name of the output video file
	writer.open(filename, codec, fps, src.size(), isColor);

	// Find edges, display them to user, save them to a file
	Mat edges;
	namedWindow("Live", WINDOW_AUTOSIZE);
	cout << "Press any key to stop recording...";
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		cvtColor(frame, edges, COLOR_BGR2GRAY);
		
		// Apply Canny to the new frame
		GaussianBlur(edges, edges, Size(5, 5), 3, 1.5);
		Canny(edges, edges, 10, 27, 3);
		
		// Remove the B and R channels to make this look like the matrix
		cvtColor(edges, edges, CV_GRAY2BGR);
		Mat BGRChannels[3];
		split(edges, BGRChannels); // split the BGR channesl
		BGRChannels[0] = Mat::zeros(edges.rows, edges.cols, CV_8UC1);// removing Green channel
		BGRChannels[2] = Mat::zeros(edges.rows, edges.cols, CV_8UC1);// removing Green channel
		merge(BGRChannels, 3, edges); // pack the image 
		
		
		writer.write(edges);
		imshow("Live", edges);
		if (waitKey(30) >= 0) {
			break;
		}
	}

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}