#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace cv;
using namespace std;


/*
* WaterShed - Takes in an image and attempts to separate the foreground
*			  from the background using the Watershed algorithm
*
*	Phillip Lopez - pgl5711@rit.edu
*/
int main(int argc, char** argv)
{
	// Various Mat's we will need
	Mat imgOrig;
	Mat img;
	Mat img_gray;
	Mat edges;
	Mat gray;
	Mat thresh;
	Mat opening;
	Mat sure_bg;
	Mat sure_fg;
	Mat unknown;
	Mat dist_transform;
	Mat markers;
	Mat kernel = Mat::ones(1, 1, CV_32F);

	// Bright
	//imgOrig = imread("data/IMG_7755_shrunk_1ovr.jpg");
	//img = imread("data/IMG_7755_shrunk_1ovr.jpg");

	//imgOrig = imread("data/IMG_7779_shrunk_2ovr.jpg");
	//img = imread("data/IMG_7779_shrunk_2ovr.jpg");

	// Darker
	imgOrig = imread("data/IMG_7757_shrunk_2undr.jpg");
	img = imread("data/IMG_7757_shrunk_2undr.jpg");

	cvtColor(img, img_gray, CV_BGR2GRAY);

	// Edges for dark image
	Canny(img_gray, edges, 0, 35);

	// Edges for lighter images
	//Canny(img_gray, edges, 50, 100);

	imshow("canny", edges);
	waitKey();

	cvtColor(img, gray, COLOR_BGR2GRAY);

	// Preset threshold for the dark image
	threshold(gray, thresh, 40, 90, THRESH_BINARY_INV);

	// Automatically find the best threshold
	//threshold(gray, thresh, 80, 150, THRESH_BINARY_INV + THRESH_TRIANGLE);
	imshow("thresh", thresh);
	waitKey();

	// Clean up the threshold image to eliminate gaps within the foreground object
	morphologyEx(thresh, opening, MORPH_OPEN, kernel, Point(-1, -1), 2);
	dilate(opening, sure_bg, kernel, Point(-1, -1), 2);
	distanceTransform(opening, dist_transform, DIST_L2, 5);

	double maxVal;
	double minVal;
	Point minLoc;
	Point maxLoc;
	minMaxLoc(dist_transform, &minVal, &maxVal, &minLoc, &maxLoc);

	// Now threshold the distance transform we did earlier to get
	// back to a binary image
	threshold(dist_transform, sure_fg, .7*maxVal, 255, 100);

	// Now we will shrink down the distance transform threshold
	// we have created. This will make sure the marker points 
	// for our foreground objects are within the objects themselves
	// Watershed will flood back out to get the exact outline
	kernel = Mat::ones(9, 9, CV_32F);
	dilate(sure_fg, sure_fg, kernel);
	dilate(sure_bg, sure_bg, kernel);
	kernel = Mat::ones(56, 56, CV_32F);
	erode(sure_fg, sure_fg, kernel);
	erode(sure_bg, sure_bg, kernel);

	sure_fg.convertTo(sure_fg, CV_8U);
	sure_bg.convertTo(sure_bg, CV_8U);

	// The foreground we will pass into watershed
	// after marking the area as different components
	imshow("fg", sure_fg);

	// Find our potential connected components
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

	// Show the connected components we found
	imshow("Connected Components", dst);
	waitKey();
	
	// Add edges to the image passed into watershed
	// to help deter the effect of shadows on the algorithm
	img.setTo(Scalar(255, 255, 255), edges != 0);
	imshow("img1", img);
	waitKey();

	// perform watershed
	watershed(img, markers);

	// Highlight edges in red
	imgOrig.setTo(Scalar(0, 0, 255), markers == -1);

	// show the final result
	imshow("img", imgOrig);
	waitKey();
}