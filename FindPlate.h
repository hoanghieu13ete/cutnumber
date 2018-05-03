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

bool checkSize(Rect r, Mat frame)
{
	if (r.width * 1.0 / r.height > 0.8 && r.width * 1.0 / r.height < 2)
	{
		if (r.width * r.height > 1500 && r.width * r.height < frame.rows * frame.cols / 4)
			return true;
	}
	return false;
}

bool wayToSort(Rect a, Rect b)
{
	return a.width * a.height < b.width * b.height;
}

Mat maximizeContrast(cv::Mat &imgGrayscale) {
	cv::Mat imgTopHat;
	cv::Mat imgBlackHat;
	cv::Mat imgGrayscalePlusTopHat;
	cv::Mat imgGrayscalePlusTopHatMinusBlackHat;

	cv::Mat structuringElement = cv::getStructuringElement(CV_SHAPE_RECT, cv::Size(5, 5));

	cv::morphologyEx(imgGrayscale, imgTopHat, CV_MOP_TOPHAT, structuringElement);
	cv::morphologyEx(imgGrayscale, imgBlackHat, CV_MOP_BLACKHAT, structuringElement);

	imgGrayscalePlusTopHat = imgGrayscale + imgTopHat;
	imgGrayscalePlusTopHatMinusBlackHat = imgGrayscalePlusTopHat - imgBlackHat;

	return(imgGrayscalePlusTopHatMinusBlackHat);
}

bool checkPossibleChar(Rect r)
{
	if (r.width > 8 && r.width < 35 && r.height > 40 && r.height < 75 && r.x > 2 && r.x + r.width < 150 - 2 && r.width*r.height > 300)
	{
		return true;
	}
	return false;
}

bool CheckArea(Rect rect)
{
	int area = rect.width * rect.height;
	if (area < 500 || area > 1500)
		return false;

	return true;
}

vector<Rect> FindPossibleChar(Mat imgThresh)
{
	vector<vector<Point>> contours;
	vector<Rect> possibleChar;
	findContours(imgThresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		Rect r = boundingRect(contours[i]);
		if (CheckArea(r))
		{
			if (r.width > 10 && r.width < 30 && r.height > 40 && r.height < 70)
			{
				possibleChar.push_back(r);
			}
		}
	}
	return possibleChar;
}

//list gray.
Rect FindBestMat(vector<Rect> list, Mat frame, vector<Rect> &possibleChars)
{
	Mat binary, clone;
	vector<vector<Point>> contours;

	for (auto rect : list)
	{
		clone = frame(rect).clone();
		resize(clone, clone, Size(150, 150));
		GaussianBlur(clone, clone, Size(3, 3), 9);

		adaptiveThreshold(clone, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 2);

		possibleChars = FindPossibleChar(binary);
		if (possibleChars.size() > 4)
			return rect;
	}
	return Rect();
}

//input is gray image find square object
vector<Rect> FindLicensePlate(Mat frame)
{
	vector<Rect> list;

	Mat binary, tmp;

	tmp = frame.clone();
	
	//find local max
	dilate(tmp, tmp, Mat::ones(Size(5, 5), CV_8UC1));

	GaussianBlur(tmp, tmp, Size(3, 3), 0);

	//cal gradient
	tmp.convertTo(tmp, CV_32F, 1 / 255.0);
	// Calculate gradients gx, gy
	Mat gx, gy;
	Sobel(tmp, gx, CV_32F, 1, 0, 3);
	Sobel(tmp, gy, CV_32F, 0, 1, 3);
	Mat mag, angle;
	cartToPolar(gx, gy, mag, angle, 1);

	threshold(mag, binary, 0.25, 255, CV_THRESH_BINARY);

	imshow("binary", binary);

	vector<vector<Point>> contours;

	binary.convertTo(binary, CV_8UC1);

	findContours(binary, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		Rect r = boundingRect(contours[i]);

		if (checkSize(r, frame))
		{
			list.push_back(r);
		}
	}

	return list;
}

Rect FindPlate(Mat frame, vector<Rect> &possibleChars)
{
	vector<Rect> list = FindLicensePlate(frame);
	Rect result = FindBestMat(list, frame, possibleChars);
	return result;
}

Rect resizeRect(Size x, Rect reszied)
{
	Rect result;

	result.x = x.width * reszied.x / 150;
	result.y = x.height * reszied.y / 150;
	result.width = x.width * reszied.width / 150;
	result.height = x.height * reszied.height / 150;

	return result;
}