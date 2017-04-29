// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main()
{

	// Flip this parameter if you want to test the calibration from a saved file
	bool und = false;
	String calibFile = "checkers.calib";

	if (und) {
		cout << "Undistorting image...";
		FileStorage fs1(calibFile, FileStorage::READ);
		Mat intrinsic;
		Mat distCoeffs;
		fs1["intrinsic"] >> intrinsic;
		fs1["distCoeffs"] >> distCoeffs;
		Mat im = imread("../data/smile.jpg");
		Mat im_dist;

		undistort(im, im_dist, intrinsic, distCoeffs);
		resize(im_dist, im_dist, im_dist.size() / 4);
		resize(im, im, im.size() / 4);
		imshow("original", im);
		imshow("undistorted", im_dist);
		waitKey();
		return 0;
	}

	// Run the camera calibration algorithm
	cout << "Calibrating camera...\n";
	int numCornersHor = 8;
	int numCornersVer = 6;

	int numSquares = numCornersHor * numCornersVer;
	Size board_sz = Size(numCornersHor, numCornersVer);

	vector<vector<Point3f>> object_points;
	vector<vector<Point2f>> image_points;

	vector<Point2f> corners;
	int successes = 0;

	Mat image;
	Mat gray_image;

	vector<Point3f> obj;
	for (int j = 0; j<numSquares; j++)
		obj.push_back(Point3f(j / numCornersHor, j%numCornersHor, 0.0f));

	String path("../data/c2/*.jpg");
	vector<Mat> imgVec;
	vector<String> fn;
	glob(path, fn, true); // recurse
	for (size_t k = 0; k<fn.size(); ++k)
	{
		Mat im = imread(fn[k]);
		//resize(im, im, Size(1080, 720));
		imgVec.push_back(im);
	}

	vector< Mat >::const_iterator itr = imgVec.begin();
	vector< Mat >::const_iterator end = imgVec.end();
	for (int i = 0; itr != end; ++itr, ++i)
	{
		cout << "Finding corners for image: " << i << "\n";
		itr->copyTo(image);
		cvtColor(image, gray_image, CV_BGR2GRAY);

		bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
		drawChessboardCorners(gray_image, board_sz, corners, found);

		image_points.push_back(corners);
		object_points.push_back(obj);

	}

	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	intrinsic.ptr<float>(0)[0] = 1;
	intrinsic.ptr<float>(1)[1] = 1;

	cout << "Calibrating camera now...\n";
	double error = calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);
	cout << "The calibration error is: " << error << "\n";
	
	FileStorage fs("checkers.calib", FileStorage::WRITE);
	fs << "intrinsic" << intrinsic;
	fs << "distCoeffs" << distCoeffs;
	fs << "error" << error;
	printf("Done Calibration\n");
	waitKey();

	return 0;
}