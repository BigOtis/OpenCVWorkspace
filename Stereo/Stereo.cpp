// Phillip Lopez - Stereo Depth Project
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>



using namespace std;
using namespace cv;

// Generates stereo depth map from two images
static void StereoCalib()
{

	/* Setup for finding stereo corrrespondences  */
	// Bike
	Ptr < StereoSGBM > stereo = StereoSGBM::create(
		-64, 144, 1, 1, 1, 1, 1, 5, 50, 100, StereoSGBM::MODE_HH
	);

	/* Setup for finding stereo corrrespondences  */
	// Lucky
	//Ptr < StereoSGBM > stereo = StereoSGBM::create(
	//	-128, 144, 11, 50, 200, 10, 10, 30, 40, 32, StereoSGBM::MODE_HH
	//);

	Mat img1 = imread("../data/stereo/umb1.png", 0);
	Mat img2 = imread("../data/stereo/umb2.png", 0);

	//Mat img1 = imread("../data/stereo/lucky1.jpg", 0);
	//Mat img2 = imread("../data/stereo/lucky2.jpg", 0);

	// Resize the images so we don't use up too much memory
	resize(img1, img1, img1.size() / 4);
	resize(img2, img2, img2.size() / 4);

	// Compute and normalize the disparity map from our two images
	Mat disp, vdisp;
	cout << "Computing disparity\n";
	stereo->compute(img1, img2, disp);
	cout << "Normalizing disparity\n";
	normalize(disp, vdisp, 0, 256, cv:: NORM_MINMAX, CV_8U);
	namedWindow("disparity", WINDOW_NORMAL);

	// Show the results and save the image
	imshow("disparity", vdisp);
	imwrite("disparity.jpg", vdisp);
	waitKey();
}


int main(int argc, char **argv)
{
	StereoCalib();
	return 0;
}

