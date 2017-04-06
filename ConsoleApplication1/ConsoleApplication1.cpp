// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main() {


	String path("../data/checkerboard/*.JPG"); //select only jpg
	vector<String> fn;
	vector<Mat> imgVec;
	vector<Mat> cornerVec;
	vector<Mat> imgPtsVec;
	vector<vector<Point3f>> objects;
	Mat img;

	int board_width = 8;
	int board_height = 6;
	int numSquares = board_width * board_height;


	glob(path, fn, true); // recurse
	for (size_t k = 0; k<fn.size(); ++k)
	{
		Mat im = imread(fn[k]);
		resize(im, im, Size(1080, 720));
		imgVec.push_back(im);
	}



	vector< Mat >::const_iterator itr = imgVec.begin();
	vector< Mat >::const_iterator end = imgVec.end();
	for (int i = 0; itr != end; ++itr, ++i)
	{
		Mat im_in;
		Mat gray;
		Mat corners;

		itr->copyTo(im_in);
		cvtColor(im_in, gray, CV_RGB2GRAY);

		bool found = findChessboardCorners(gray, Size(board_width, board_height), corners);

		namedWindow("corners", CV_WINDOW_AUTOSIZE);
		drawChessboardCorners(im_in, Size(8, 6), corners, found);
		imgPtsVec.push_back(im_in);
		cornerVec.push_back(corners);
		imshow("corners", im_in);
		waitKey();

		vector< Point3f > obj;
		for (int i = 0; i < numSquares; i++)
				obj.push_back(Point3f((float) i / board_width, i % board_height, 0.0f));
		objects.push_back(obj);

		img = im_in;
	}

	Mat camera;
	Mat dist;
	vector<Mat> rvecs, tvecs;
	int flag = 0;
	flag |= CV_CALIB_FIX_K4;
	flag |= CV_CALIB_FIX_K5;
	calibrateCamera(objects, cornerVec, img.size(), dist, camera, rvecs, tvecs, flag);

	waitKey();
}