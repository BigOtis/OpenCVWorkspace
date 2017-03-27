#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

#include "cartoon.hpp"
#include "stdafx.h"

using namespace cv;

int main(int argc, char **argv)
{
	Mat image = imread("../data/fall.jpg");
	cartoonifyImage(image, image);
	imshow("Cartoon image", image);
	waitKey();
}