// Diving.cpp : A program that takes an input video
// and then allows the user to select frames to add
// to a composite image. The background of the video
// is subtracted and only the large objects in motion
// are included in the final output image.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;

int getBrightness(Mat frame);
void FindDifference(Mat src1, Mat &dst);

// Mat to hold the average value of pixels across all frames
Mat im_avg;

// Mat to hold the std deviation of all pixels across all frames
Mat im_std;

// How many standard deviations we will use
// to check for motion in the image
int STD_THRESHOLD = 2;

// Main method for Diver
int main()
{
	cout << "Starting diver stop-motion collage creator...\n";
	cout << "Using the following file [../data/diver/diver1.mov]\n";

	// Load in the video
	VideoCapture cap("../data/diver/diver1.mov");
	Mat frame;
	Mat gray;

	// Setup a named window for our potential images
	namedWindow("Potential Images - Press spacebar to select an image to be included in the collage", CV_WINDOW_AUTOSIZE);

	// A list of the brightest frames
	vector<Mat> brightestFrames;

	// Total number of frams we decided to keep
	int numFrames = 0;

	// First find the brightest frames and 
	// compute their average value as we go
	bool first = true;
	bool done = false;

	cout << "Removing flicker from slow-motion video, keeping brightest frames...\n";
	// Loop through the entire video
	for (;!done;) {
		Mat brightest;
		int max_lum = 0;
		for (int i = 0; i <= 3; i++) {
			// If we hit the end of the video, quit
			if (!cap.read(frame)) {
				done = true;
				break;
			}
			// Get the overall brightness of this frame
			cvtColor(frame, gray, CV_RGB2GRAY);
			int lum = getBrightness(gray);
			// Check if it is the brightest out of the last 3 we've seen
			if (lum > max_lum) {
				frame.copyTo(brightest);
				max_lum = lum;
			}
		}
		// Add the brightest frames to our vector 
		brightestFrames.push_back(brightest);
		numFrames++;
		// Now add it's average intensity to the average Mat
		if (first) {
			first = false;
			brightest.copyTo(im_avg);
			cvtColor(im_avg, im_avg, CV_RGB2GRAY);
			im_avg.convertTo(im_avg, CV_32F);
		}
		else {
			Mat gray;
			cvtColor(brightest, gray, CV_RGB2GRAY);
			gray.convertTo(gray, CV_32F);
			add(im_avg, gray, im_avg);
		}
	}
	
	// Show the average
	im_avg = im_avg / (numFrames);

	// Setup an iterator to go through the brightest frames gathered previously
	vector< Mat > best;
	vector< Mat >::const_iterator itr = brightestFrames.begin();
	vector< Mat >::const_iterator end = brightestFrames.end();
	itr->copyTo(im_std);
	++itr;

	// Setup the Mat that will hold our standard deviations per pixel
	cvtColor(im_std, im_std, CV_RGB2GRAY);
	im_std.convertTo(im_std, CV_32F);
	subtract(im_std, im_avg, im_std);
	pow(im_std, 2, im_std);

	cout << "Showing best possible frames...";
	cout << "Press spacebar to include the current frame in the collage...\n";
	int i;
	// Loop through the brightest frames, compute the standard deviation along the way
	// Allow the user to select the best frames to be included in the collage
	for (i = 1; itr != end; ++itr, ++i)
	{
			Mat pot;
			itr->copyTo(pot);
			imshow("Potential Images - Press spacebar to select an image to be included in the collage", pot);

			// add to variance/std
			Mat gray;
			cvtColor(pot, gray, CV_RGB2GRAY);
			gray.convertTo(gray, CV_32F);

			Mat tmp_std;
			subtract(gray, im_avg, tmp_std);
			pow(tmp_std, 2, tmp_std);
			add(im_std, tmp_std, im_std);

			// Frame was selected, add to list of best frames
			int key = waitKey(50);
			if (key == 32 || i == 0) {
				best.push_back(pot);
			}
	}
	
	// Compute standard deviation
	im_std = im_std / (numFrames);
	pow(im_std, .5, im_std);

	// Loop through the selected frames
	vector< Mat >::const_iterator itrBest = best.begin();
	vector< Mat >::const_iterator endBest = best.end();

	Mat im_first;
	Mat im_combined;
	itrBest->copyTo(im_first);
	++itrBest;
	im_first.copyTo(im_combined);

	cout << "Combining selected images into collage...\n";
	for (int i = 1; itrBest != endBest; ++itrBest, ++i)
	{
		Mat im;
		itrBest->copyTo(im);
		Mat diff;
		FindDifference(im, diff);
		im.copyTo(im_combined, diff);
	}

	cout << "Displaying results:\n";
	// Show the standard deviation image
	im_std.convertTo(im_avg, CV_8U);
	imshow("std image", im_avg);

	// Show the avg background image
	im_avg.convertTo(im_avg, CV_8U);
	imshow("background average", im_avg);

	// Show the combined image
	imshow("combined", im_combined);

	waitKey();
	return 0;
}

int getBrightness(Mat frame) {
	return sum(frame)[0];
}

void FindDifference(Mat src1, Mat &dst) {

	Mat gray;
	cvtColor(src1, gray, CV_RGB2GRAY);
	gray.convertTo(gray, CV_32F);

	Mat sub;
	sub = abs(im_avg - gray);

	Mat zeros;
	sub.copyTo(zeros);
	zeros.setTo(0);
	zeros.setTo(1, sub > (im_std * STD_THRESHOLD));
	dst = zeros;

	Mat kernel = Mat::ones(4, 4, CV_64F);
	erode(dst, dst, kernel);

	kernel = Mat::ones(21, 21, CV_64F);
	dilate(dst, dst, kernel);

	dst.convertTo(dst, CV_8U);
}