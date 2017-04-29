// TestCalibration.cpp
// Tests the camera calibration undistortion feature
// given a file containing distortion coefficients 
// Phillip Lopez
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main()
{

	String calibFile = "checkers.calib";

	cout << "Undistorting image...";
	
	// Load in the parameters
	FileStorage fs1(calibFile, FileStorage::READ);
	Mat intrinsic;
	Mat distCoeffs;
	fs1["intrinsic"] >> intrinsic;
	fs1["distCoeffs"] >> distCoeffs;

	// Load in the image
	Mat im = imread("../data/smile.jpg");
	Mat im_dist;

	// Undistort the image
	undistort(im, im_dist, intrinsic, distCoeffs);

	// resize the image so we can show it
	resize(im_dist, im_dist, im_dist.size() / 4);
	resize(im, im, im.size() / 4);

	// show the image
	imshow("original", im);
	imshow("undistorted", im_dist);
	waitKey();
		
	return 0;
}