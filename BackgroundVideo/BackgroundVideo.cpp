// BackgroundVideo.cpp : Uses a MOG2 background subtractor
// to automatically remove the background of the input video
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>


using namespace cv;
using namespace std;


void doVideoDemo(String video) {

	VideoCapture cap(video); // open the default camera
	if (!cap.isOpened())
	{
		std::cout << "!!! Failed to open file: " << video;
		return;
	}

	// Setup the display windows
	namedWindow("Red Motion Detection", CV_WINDOW_AUTOSIZE);
	namedWindow("Object", CV_WINDOW_AUTOSIZE);

	resizeWindow("Red Motion Detection", 720, 480);
	resizeWindow("Object", 720, 480);

	moveWindow("Red Motion Detection", 0, 35);
	moveWindow("Object", 720, 35);


	// For Loop declarations
	Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
	Mat frame;

	for (;;)
	{
		if (!cap.read(frame))
			break;

		// Resize the wi
		resize(frame, frame, Size(720, 480));

		// Add the frame to the model generate a mask based on it
		pMOG2->apply(frame, fgMaskMOG2);

		// Erosion settings
		int erosion_size = 1;
		Mat element = getStructuringElement(MORPH_ERODE,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));

		// Erode the image to get rid of the flicker noise
		erode(fgMaskMOG2, fgMaskMOG2, element);
		dilate(fgMaskMOG2, fgMaskMOG2, element);

		// Deep copy the frame
		Mat object = frame.clone();

		// Apply the mask to the frame
		frame.setTo(Scalar(0, 0, 255), fgMaskMOG2);
		object.setTo(Scalar(0, 255, 0), 255 - fgMaskMOG2);

		imshow("Red Motion Detection", frame);
		imshow("Object", object);

		char key = cvWaitKey(10);
		if (key == 27) // ESC
			break;
	}

	return;
}

int main()
{
	doVideoDemo("../data/dog.mp4");
	doVideoDemo("../data/dog1.mp4");
	doVideoDemo("../data/dogs2.mp4");
	doVideoDemo("../data/dogs3.mp4");
	doVideoDemo("../data/dogs4.mp4");
	doVideoDemo("../data/dogs5.mp4");
	doVideoDemo("../data/dogs6.mp4");
	doVideoDemo("../data/dogs7.mp4");
	doVideoDemo("../data/dogs8.mp4");
	doVideoDemo("../data/dogs9.mp4");
	doVideoDemo("../data/dogs10.mp4");
	doVideoDemo("../data/dogs11.mp4");
	doVideoDemo("../data/dogs12.mp4");
	doVideoDemo("../data/dogs13.mp4");
	doVideoDemo("../data/dogs14.mp4");
	doVideoDemo("../data/dogs15.mp4");
	doVideoDemo("../data/dogs16.mp4");
	doVideoDemo("../data/dogs17.mp4");

	return 0;
}

