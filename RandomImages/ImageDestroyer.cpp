// ImageDestroyer.cpp : Takes an input folder of images
// and applies some random image transformations on these images
// the process continues until the program is directed to stop
// This will destroy the images
//
// @author Phillip Lopez - pgl5711@rit.edu
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>


using namespace cv;
using namespace std;

Mat randomAct(Mat);

Mat last;

// Main function
int main()
{

	String folderpath = "..//data//dogs//*.jpg";
	vector<String> filenames;
	vector<Mat> images;
	
	// Load in all of the jpg files in the dogs dir
	glob(folderpath, filenames);
	last = imread(filenames[0]);
	
	// Loop through each file until a key is pressed
	bool done = false;
	while (!done) {
		for (size_t i = 0; i < filenames.size(); i++)
		{
			// read in the image file
			Mat im = imread(filenames[i]);
			Mat mod;

			// make a random change to it
			mod = randomAct(im);

			// display the image
			namedWindow("Dogs", WINDOW_NORMAL);
			resizeWindow("Dogs", 720, 720);
			imshow("Dogs", mod);
			
			// Save the updated image back to the file
			last = mod.clone();
			imwrite(filenames[i], mod);

			// See if the user wants to quit
			if (waitKey(10) >= 0) {
				done = true;
				break;
			}
		}
	}
}

// Randomly chooses and performs an image transform on the given Mat
Mat randomAct(Mat in) 
{
	Mat out;
	int r = rand() % 6;
	if (r == 0) { // Blur
		cout << "0. Gaussian warp...\n";
		GaussianBlur(in, out, Size(5, 5), 3, 1.5);
		return out;
	}
	if (r == 1) { // Random Warp
		cout << "1. Random warp...\n";
		// From openCV examples
		// http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/warp_affine/warp_affine.html
		Point2f srcTri[3];
		Point2f dstTri[3];

		Mat rot_mat(2, 3, CV_32FC1);
		Mat warp_mat(2, 3, CV_32FC1);
		Mat src, warp_dst, warp_rotate_dst;

		/// Load the image
		src = in;

		/// Set the dst image the same type and size as src
		warp_dst = Mat::zeros(src.rows, src.cols, src.type());

		/// Set your 3 points to calculate the  Affine Transform
		srcTri[0] = Point2f(0, 0);
		srcTri[1] = Point2f(src.cols - 1, 0);
		srcTri[2] = Point2f(0, src.rows - 1);

		dstTri[0] = Point2f(src.cols*0.0, src.rows*0.33);
		dstTri[1] = Point2f(src.cols*0.85, src.rows*0.25);
		dstTri[2] = Point2f(src.cols*0.15, src.rows*0.7);

		/// Get the Affine Transform
		warp_mat = getAffineTransform(srcTri, dstTri);

		/// Apply the Affine Transform just found to the src image
		warpAffine(src, warp_dst, warp_mat, warp_dst.size());

		/** Rotating the image after Warp */

		/// Compute a rotation matrix with respect to the center of the image
		Point center = Point(warp_dst.cols / 2, warp_dst.rows / 2);
		double angle = (rand() % 360);
		double scale = 1;

		/// Get the rotation matrix with the specifications above
		rot_mat = getRotationMatrix2D(center, angle, scale);

		/// Rotate the warped image
		warpAffine(warp_dst, warp_rotate_dst, rot_mat, warp_dst.size());

		Rect roi = Rect(50, 50, warp_rotate_dst.size().width-50, warp_rotate_dst.size().height-50);
		Mat imagen_roi = warp_rotate_dst(roi);
		resize(imagen_roi, imagen_roi, Size(warp_rotate_dst.size().width, warp_rotate_dst.size().height), 0, 0, CV_INTER_AREA);
		return imagen_roi;
	}
	if (r == 2) { // Show only blues
		cout << "2. Only blues...\n";
		// Remove the B and R channels to make this look like the matrix
		Mat BGRChannels[3];
		split(in, BGRChannels); // split the BGR channesl
		BGRChannels[1] = Mat::zeros(in.rows, in.cols, CV_8UC1);// removing Green channel
		BGRChannels[2] = Mat::zeros(in.rows, in.cols, CV_8UC1);// removing Red channel
		merge(BGRChannels, 3, in); // pack the image 
		return in;
	}
	if (r == 3) { // Random rotation
		cout << "3. Random rotation...\n";
		Mat dst;
		Point2f pt(in.cols / 2., in.rows / 2.);
		Mat r = getRotationMatrix2D(pt, rand() % 360, 1.0);
		warpAffine(in, dst, r, Size(in.cols, in.rows));
		return dst;
	}
	if (r == 4) { // Apply sobel edge detector
		cout << "4. Sobel...\n";
		Mat grey;
		cvtColor(in, grey, CV_BGR2GRAY);
		Mat sobelx = grey;
		Sobel(grey, sobelx, in.depth(), 1, 0);
		cvtColor(sobelx, sobelx, CV_GRAY2BGR);
		return  sobelx;
	}
	if (r == 5) { // Blend two images together
		cout << "5. Blending...\n";
		Mat dst;
		resize(in, in, Size(720, 720));
		resize(last, last, Size(720, 720));

		addWeighted(in, .7, last, .6, 0.0, dst);
		return dst;
	}

	return in;
}


