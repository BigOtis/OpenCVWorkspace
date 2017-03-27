// Diving.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;

int getBrightness(Mat frame);
void FindDifference(Mat src1, Mat src2, Mat &dst, int threshold);

int main()
{

	VideoCapture cap("../data/diver/diver1.mov");
	Mat frame;
	Mat gray;

	namedWindow("potential", CV_WINDOW_AUTOSIZE);
	namedWindow("best", CV_WINDOW_AUTOSIZE);


	vector<Mat> brightestFrames;

	bool done = false;
	for (;!done;) {
		Mat brightest;
		int max_lum = 0;
		for (int i = 0; i <= 3; i++) {
			if (!cap.read(frame)) {
				done = true;
				break;
			}
			cvtColor(frame, gray, CV_RGB2GRAY);
			int lum = getBrightness(gray);
			if (lum > max_lum) {
				frame.copyTo(brightest);
				max_lum = lum;
			}
		}
		brightestFrames.push_back(brightest);
	}

	vector< Mat > best;
	vector< Mat >::const_iterator itr = brightestFrames.begin();
	vector< Mat >::const_iterator end = brightestFrames.end();

	for (int i = 0; itr != end; ++itr, ++i)
	{
			Mat pot;
			itr->copyTo(pot);
			imshow("potential", pot);
			int key = waitKey(50);
			if (key == 32 || i == 0) {
				best.push_back(pot);
			}
	}


	vector< Mat >::const_iterator itrBest = best.begin();
	vector< Mat >::const_iterator endBest = best.end();

	Mat im_combined;
	Mat im_first;
	itrBest->copyTo(im_first);
	++itrBest;
	im_first.copyTo(im_combined);
	for (int i = 1; itrBest != endBest; ++itrBest, ++i)
	{
		Mat im;
		itrBest->copyTo(im);

		Mat diff;
		FindDifference(im_first, im, diff, 50);
		imshow("difference", diff);
		imshow("selected", im);
		im.copyTo(im_combined, diff);
		waitKey();
	}

	imshow("combined", im_combined);
	waitKey();
	return 0;
}

int getBrightness(Mat frame) {
	return sum(frame)[0];
}

void FindDifference(Mat src1, Mat src2, Mat &dst, int threshold) {
	dst = cv::abs(src2 - src1);
	cv::threshold(dst, dst, threshold, 255, cv::THRESH_BINARY);
}