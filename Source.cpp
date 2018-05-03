#include "FindPlate.h"

int main()
{
	//ifstream read;
	//read.open("trainingdata/bg.txt");

	//string line;

	VideoCapture Cap(0);

	Rect result;
	vector<Rect> list;
	Mat frame, gray;
	vector<Rect> possibleChars;

	//while (getline(read, line))
	while (Cap.read(frame))
	{
		//frame = imread("trainingdata/" + line, 1);
		resize(frame, frame, Size(480, 360));
		cvtColor(frame, gray, CV_BGR2GRAY);

		list = FindLicensePlate(gray);
		result = FindBestMat(list, gray, possibleChars);

		if (!result.empty())
		{
			rectangle(frame, result, Scalar(0, 255, 0));
		}

		for (auto rect : possibleChars)
		{
			rectangle(frame(result), resizeRect(result.size(), rect), Scalar(0, 0, 255));
		}

		imshow("imshow", frame);
		waitKey(25);
	}

	return 0;
}