#pragma once
#include<opencv2/core/core.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

Mat kernel1 = Mat::ones(Size(1, 10), CV_8UC1);
Mat kernel2 = Mat::ones(Size(10, 1), CV_8UC1);
Mat kernel3 = (Mat_<uchar>(5, 5) << 1, 1, 0, 0, 0,
	1, 1, 1, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 1, 1, 1,
	0, 0, 0, 1, 1);
Mat kernel4 = (Mat_<uchar>(5, 5) << 0, 0, 0, 1, 1,
	0, 0, 1, 1, 1,
	0, 1, 1, 1, 0,
	1, 1, 1, 0, 0,
	1, 1, 0, 0, 0);

bool checkSize(Rect r)
{
	if (r.width * 1.0 / r.height > 1.0 && r.width * 1.0 / r.height < 2)
	{
		if (r.width * r.height > 1500)
			return true;
	}
	return false;
}

void ConectLine(Mat &binary)
{
	erode(binary, binary, Mat::ones(Size(1, 2), CV_8UC1));

	dilate(binary, binary, kernel1);
	erode(binary, binary, kernel1);

	dilate(binary, binary, kernel2);
	erode(binary, binary, kernel2);

	dilate(binary, binary, kernel3);
	erode(binary, binary, kernel3);

	dilate(binary, binary, kernel3);
	erode(binary, binary, kernel3);
}

bool wayToSort(Rect a, Rect b)
{
	return a.width * a.height < b.width * b.height;
}

//list gray.
Rect FindBestMat(vector<Rect> list, Mat frame)
{
	vector<Rect> tmp;
	Mat binary, clone;
	vector<vector<Point>> contours;

	for (auto rect : list)
	{
		int count = 0;

		clone = frame(rect).clone();
		//cvtColor(clone, clone, CV_BGR2GRAY);

		resize(clone, clone, Size(150, 150));

		GaussianBlur(clone, clone, Size(5, 5), 0);

		adaptiveThreshold(clone, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 9);

		findContours(binary, contours, RETR_LIST, CV_CHAIN_APPROX_NONE);

		for (int i = 0; i < contours.size(); i++) {
			Rect r = boundingRect(contours[i]);

			if (r.width > 5 && r.width < 30 && r.height > 30 && r.height < 80)
			{
				count++;
			}
		}
		if (count > 5) {
			tmp.push_back(rect);
		}
	}

	if (!tmp.empty()) {
		sort(tmp.begin(), tmp.end(), wayToSort);
		return tmp[0];
	}

	else
		return Rect();
}

//input is gray image
vector<Rect> FindLicensePlate(Mat frame)
{
	vector<Rect> list;

	Mat binary, tmp;

	tmp = frame.clone();

	//find local max
	dilate(tmp, tmp, Mat::ones(Size(5, 5), CV_8UC1));

	//cal gradient
	tmp.convertTo(tmp, CV_32F, 1 / 255.0);
	// Calculate gradients gx, gy
	Mat gx, gy;
	Sobel(tmp, gx, CV_32F, 1, 0, 1);
	Sobel(tmp, gy, CV_32F, 0, 1, 1);
	Mat mag, angle;
	cartToPolar(gx, gy, mag, angle, 1);

	threshold(mag, binary, 0.1, 255, CV_THRESH_BINARY);

	//imshow("before", binary);

	ConectLine(binary);

	vector<vector<Point>> contours;

	binary.convertTo(binary, CV_8UC1);

	//imshow("after", binary);

	findContours(binary, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		Rect r = boundingRect(contours[i]);

		if (checkSize(r))
		{
			list.push_back(r);
		}
	}

	return list;
}
vector<Mat> findCharOfHalfPlate(Mat binary) {
	int num_scan = 20;
	vector<int> num_zeros;
	int i = 0;
	vector<Point> tmp;
	int j;
	/*while () 
	{
		for (j = i; j < num_scan + i && j < binary.cols - num_scan; j = j + 2) {
			findNonZero(binary(Rect(j, 0, 20, binary.rows)), tmp);
			num_zeros.push_back(tmp.size());
			cout << tmp.size() << endl;
		}
		int position = distance(num_zeros.begin(), min_element(num_zeros.begin(), num_zeros.end()));
		cout << position << endl;
		imshow("ff",binary(Rect(i + 2 * position, 0, 20, binary.rows)));
		waitKey(0);
		num_zeros.clear();
		i = 15 + j;
		j = 0;
	}*/
	for (int i = 1; i < 5; i++)
	{
		int x = 12;
		int a = 0;
		if (i == 1)
		{
			for (j = 0; j < 2 * x; j++)
			{
				findNonZero(binary(Rect(j, 0, 20, binary.rows)), tmp);
				num_zeros.push_back(tmp.size());
				cout << tmp.size() << endl;
			}
			int position = distance(num_zeros.begin(), min_element(num_zeros.begin(), num_zeros.end()));
			a = position + 20;
			cout << position << endl;
			imshow("ff", binary(Rect(position + a, 0, 20, binary.rows)));
			waitKey(0);
			num_zeros.clear();
		}
		if (i == 2) {
			for (j = a; j < x + a; j++) {
				findNonZero(binary(Rect(j, 0, 20, binary.rows)), tmp);
				num_zeros.push_back(tmp.size());
				cout << tmp.size() << endl;
			}
			int position = distance(num_zeros.begin(), min_element(num_zeros.begin(), num_zeros.end()));
			cout << position << endl;
			a = position + 20;
			imshow("ff", binary(Rect(position + a, 0, 20, binary.rows)));
			waitKey(0);
			num_zeros.clear();
		}
		if (i == 3) {
			for (j = 1.5*a; j < a + 2*x + a; j++) {
				findNonZero(binary(Rect(j, 0, 20, binary.rows)), tmp);
				num_zeros.push_back(tmp.size());
				cout << tmp.size() << endl;
			}
			int position = distance(num_zeros.begin(), min_element(num_zeros.begin(), num_zeros.end()));
			cout << position << endl;
			a = position + 20;
			imshow("ff", binary(Rect(position+a, 0, 20, binary.rows)));
			waitKey(0);
			num_zeros.clear();
		}
		if (i == 4) {
			for (j = a; j < x + a; j++) {
				findNonZero(binary(Rect(j, 0, 20, binary.rows)), tmp);
				num_zeros.push_back(tmp.size());
				cout << tmp.size() << endl;
			}
			int position = distance(num_zeros.begin(), min_element(num_zeros.begin(), num_zeros.end()));
			cout << position << endl;
			imshow("ff", binary(Rect(position  + a, 0, 20, binary.rows)));
			waitKey(0);
			num_zeros.clear();
		}
	}
	return vector<Mat>();
}
vector<Mat> findCharOfFullPlate(Rect plate, Mat frame) {
	Mat binary, clone;
	vector<Mat> tmp;
	vector<Mat> halfOfPlates;
	clone = frame(plate).clone();
	resize(clone, clone, Size(150, 150));
	adaptiveThreshold(clone, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 19, 9);

	imshow("aa", binary);
	Rect roi_1 = Rect(0, 0, 150, 70);
	Rect roi_2 = Rect(0, 75, 150, 65);
	halfOfPlates.push_back(binary(roi_1));
	halfOfPlates.push_back(binary(roi_2));
	imshow("bb", halfOfPlates[0]);
	imshow("cc", halfOfPlates[1]);
	cout << halfOfPlates[0].cols << " " << halfOfPlates[0].rows << endl;
	tmp = findCharOfHalfPlate(halfOfPlates[0]);
	return vector<Mat>();
}

Rect FindPlate(Mat frame)
{	
	vector<Mat> charFound;
	vector<Rect> list = FindLicensePlate(frame);
	Rect result = FindBestMat(list, frame);
	charFound = findCharOfFullPlate(result, frame);
	return result;
}