#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace cv;
using namespace std;
bool expandTo(Mat img, Vec3b orig, int r, int c);
int doVoronoi(Mat img);
vector<Vec3b> colors(1000);

int main() {
	//Mat img = imread("../data/blocks.JPG");

	namedWindow("bg", CV_WINDOW_AUTOSIZE);	
	namedWindow("final", CV_WINDOW_AUTOSIZE);

	for (int label = 1; label < 1000; ++label) {
		rand();
		colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) {  // check if we succeeded
		cout << "Unable to open the default camera. Press enter to quit.";
		waitKey();
		return -1;
	}
	cap.set(CV_CAP_PROP_FPS, 10);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 380);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 280);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		doVoronoi(frame);
	}
}

int doVoronoi(Mat img)
{

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	cvtColor(img, gray, COLOR_BGR2GRAY);

	GaussianBlur(gray, gray, Size(5, 5), 3, 6);


	Mat thresh;
	threshold(gray, thresh, 80, 150, THRESH_BINARY + THRESH_TRIANGLE);

	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_ERODE,
		Size(3,3),
		Point(-1, -1));

	// Erode the image to get rid of the flicker noise
	Mat opening;
	erode(thresh, opening, element);
	dilate(thresh, opening, element);

	opening.setTo(Scalar(255,255,255), opening > 0);

	Mat markers;
	int nLabels = connectedComponents(opening, markers);
	Mat dst(img.size(), CV_8UC3);
	vector<int> rows;
	vector<int> cols;
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = markers.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[label % 1000];
			if (label > 0) {
				rows.push_back(r);
				cols.push_back(c);
			}
		}
	}

	Mat res;
	dst.copyTo(res);

	int pass = 0;
	while(rows.size() > 0){

		vector<int> nextRows;
		vector<int> nextCols;
		vector< int >::const_iterator row_itr = rows.begin();
		vector< int >::const_iterator row_end = rows.end();
		vector< int >::const_iterator col_itr = cols.begin();
		vector< int >::const_iterator col_end = cols.end();

		for (int i = 1; row_itr != row_end; ++i)
		{
			int r = *row_itr;
			int c = *col_itr;

			// Below
			Vec3b orig = res.at<Vec3b>(r, c);
			if (expandTo(res, orig, r + 1, c)) {
				nextRows.push_back(r + 1);
				nextCols.push_back(c);
			}
			// Above
			if (expandTo(res, orig, r - 1, c)) {
				nextRows.push_back(r - 1);
				nextCols.push_back(c);
			}
			// Right
			if (expandTo(res, orig, r, c + 1)) {
				nextRows.push_back(r);
				nextCols.push_back(c + 1);
			}
			// Left
			if (expandTo(res, orig, r, c - 1)) {
				nextRows.push_back(r);
				nextCols.push_back(c - 1);
			}
			++row_itr;
			++col_itr;
		}
		rows = nextRows;
		cols = nextCols;
	}

	img.copyTo(res, opening);
	imshow("bg", opening);
	imshow("final", res);
	waitKey(10);
	return 0;
}

bool expandTo(Mat img, Vec3b orig, int r, int c) {

	if (r < 0 || c < 0) {
		return false;
	}
	if (r >= img.size().height || c >= img.size().width) {
		return false;
	}

	Vec3b &pixel = img.at<Vec3b>(r, c);
	if ((pixel[0] == 0) && (pixel[1] == 0) && (pixel[2] == 0)) {
		pixel = Vec3b(orig[0], orig[1], orig[2]);
		return true;
	}

	return false;
}

