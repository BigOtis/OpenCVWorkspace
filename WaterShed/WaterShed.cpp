#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
	Mat img;
	Mat gray;
	Mat thresh;
	Mat opening;
	Mat sure_bg;
	Mat sure_fg;
	Mat unknown;
	Mat dist_transform;
	Mat markers;
	Mat kernel = Mat::ones(1, 1, CV_32F);


	img = imread("data/IMG_7755_shrunk_1ovr.jpg");
	cvtColor(img, gray, COLOR_BGR2GRAY);

	threshold(gray, thresh, 180, 255, THRESH_BINARY_INV);// +THRESH_TRIANGLE);

	morphologyEx(thresh, opening, MORPH_OPEN, kernel, Point(-1, -1), 2);

	dilate(opening, sure_bg, kernel, Point(-1, -1), 2);
	//imshow("dist", opening);
	//waitKey();

	distanceTransform(opening, dist_transform, DIST_L2, 5);

	double maxVal;
	double minVal;
	Point minLoc;
	Point maxLoc;
	minMaxLoc(dist_transform, &minVal, &maxVal, &minLoc, &maxLoc);
	threshold(dist_transform, sure_fg, .7*maxVal, 255, 100);

	kernel = Mat::ones(9, 9, CV_32F);
	dilate(sure_fg, sure_fg, kernel);
	dilate(sure_bg, sure_bg, kernel);
	kernel = Mat::ones(21, 21, CV_32F);
	erode(sure_fg, sure_fg, kernel);
	erode(sure_bg, sure_bg, kernel);

	sure_fg.convertTo(sure_fg, CV_8U);
	sure_bg.convertTo(sure_bg, CV_8U);
	imshow("fg", sure_fg);
//	imshow("bg", sure_bg);
	waitKey();


	int nLabels = connectedComponents(sure_fg, markers);// +1;


	std::vector<Vec3b> colors(nLabels);
	colors[0] = Vec3b(0, 0, 0);//background
	for (int label = 1; label < nLabels; ++label) {
		rand();
		colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}
	Mat dst(img.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = markers.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[label];
		}
	}

	imshow("Connected Components", dst);
	waitKey();

	watershed(img, markers);
	img.setTo(Scalar(255, 0, 0), markers == -1);
	imshow("img", img);
	waitKey();
}