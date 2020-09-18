//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.439.509&rep=rep1&type=pdf - ссылка на научную работу с описанием алгоритма Ниблэка
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;

void make_black(int i, int j)
{
	img.at<uchar>(i, j) = 0;
}

void make_white(int i, int j)
{
	img.at<uchar>(i, j) = 255;
}

int main()
{
	int r = 16; //радиус окрестности, в которой высчитывается формула
	double k = -0.2; //эмпирически определяемый параметр
	double nAverage, average, sqrnDeviation, threshold;
	img = imread("../test3.png", IMREAD_COLOR);
	if (img.empty())
	{
		std::cout << "error" << endl;
		return -1;
	}
	int Isize = img.rows;
	int Jsize = img.cols;
	int Isize1 = Isize + 2 * r;
	int Jsize1 = Jsize + 2 * r;
	int n = (2 * r + 1) * (2 * r + 1);
	cvtColor(img, img, COLOR_BGR2GRAY);

	double** brightness = new double* [Isize1];
	for (int i = 0; i < Isize1; i++)
		brightness[i] = new double[Jsize1];
	for (int i = 0; i < Isize1; i++)
		for (int j = 0; j < Jsize1; j++)
		{
			brightness[i][j] = -1;
		}

	for (int i = 0; i < Isize; i++)
		for (int j = 0; j < Jsize; j++)
		{
			brightness[i + r][j + r] = img.at<uchar>(i, j);
		}

	//(i,r) first
	for (int i = r; i < Isize + r; i++)
	{
		average = nAverage = sqrnDeviation = 0;
		for (int i1 = i - r; i1 < i + r + 1; i1++)
			for (int j1 = r; j1 < 2 * r + 1; j1++)
				if (brightness[i1][j1] != -1) nAverage += brightness[i1][j1];
		average = nAverage / n;
		for (int i1 = i - r; i1 < i + r + 1; i1++)
			for (int j1 = r; j1 < 2 * r + 1; j1++)
			{
				double b = brightness[i1][j1];
				if (b != -1) sqrnDeviation += (b - average) * (b - average);
				//sqrnDeviation += b * b;
			}
		threshold = average + k * sqrt(sqrnDeviation / n);
		//threshold = average + k * sqrt(-average * average + sqrnDeviation / n);
		if (brightness[i][r] < threshold) make_black(i - r, 0); else make_white(i - r, 0);

		for (int j = r + 1; j < Jsize + r; j++)
		{
			for (int k = i - r; k < i + r + 1; k++)
			{
				double b1 = brightness[k][j - r - 1];
				double b2 = brightness[k][j + r];
				if (b1 != -1) nAverage -= b1;
				if (b2 != -1) nAverage += b2;
			}
			average = nAverage / n;
			for (int k = i - r; k < i + r + 1; k++)
			{
				double b1 = brightness[k][j - r - 1];
				double b2 = brightness[k][j + r];
				//sqrnDeviation -= b1 * b1;
				//sqrnDeviation += b2 * b2;
				if (b1 != -1) sqrnDeviation -= (b1 - average) * (b1 - average);
				if (b2 != -1) sqrnDeviation += (b2 - average) * (b2 - average);
			}

			threshold = average + k * sqrt(sqrnDeviation / n);
			//threshold = average + k * sqrt(-average * average + sqrnDeviation / n);
			if (brightness[i][j] < threshold) make_black(i - r, j - r); else make_white(i - r, j - r);
		}
	}

	namedWindow("window", WINDOW_AUTOSIZE);
	imshow("window", img);
	waitKey(0);

	destroyWindow("window");
	for (int i = 0; i < Isize1; i++)
	{
		delete[] brightness[i];
	}
	delete[] brightness;
	return 0;
}