/**
@file TestImageShow.cpp
@brief Simple test of imshow in opencv
@author Phillip Lopez - pgl5711@rit.edu
@date 2/1/2017
*/

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
	Mat img;
	img = imread("../data/ducks1.jpg");
	namedWindow("Image", WINDOW_AUTOSIZE);
	imshow("Image", img);
	cout << "Press any key to close the image window...";
	waitKey(0);
	return 0;
}