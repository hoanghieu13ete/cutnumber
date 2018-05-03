#include "FindPlate.h"

int main()
{
	ifstream read;
	read.open("data/bg.txt");

	string line;

	//VideoCapture Cap(0);

	Rect result;
	Mat frame, gray;

	while (getline(read, line))
		//while (Cap.read(frame))
	{

		frame = imread("data/" + line, 1);
		cvtColor(frame, gray, CV_BGR2GRAY);

		result = FindPlate(gray);

		if (!result.empty())
		{
			rectangle(frame, result, Scalar(0, 255, 0));
			/*cv::line(frame(result), Point(frame(result).cols / 2, 0), Point(frame(result).cols / 2, frame(result).rows), Scalar(0, 255, 0));
			cv::line(frame(result), Point(0, frame(result).rows / 2), Point(frame(result).cols, frame(result).rows / 2), Scalar(0, 255, 0));
			imshow("plate", frame(result));*/
		}
		imshow("imshow", frame);
		waitKey(0);
	}

	return 0;
}