//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.439.509&rep=rep1&type=pdf - ссылка на научную работу с описанием алгоритма Ниблэка
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
int Isize, Jsize;
int r = 200; //радиус окрестности, в которой высчитывается формула
double k = -0.2; //эмпирически определяемый параметр

void make_black(int i, int j)
{
	img.at<uchar>(i, j) = 0;
}

void make_white(int i, int j)
{
	img.at<uchar>(i, j) = 255;
}

void niblack_algorithm(double** brightness)
{
	double nAverage, average, sqrnDeviation, threshold;
	//(i, 0) first
	for (int i = 0; i < Isize; i++)
	{
		int n = (min(Isize, i + r + 1) - max(0, i - r)) * (r + 1);
		average = nAverage = sqrnDeviation = 0;
		for (int i1 = max(0, i - r); i1 < min(Isize, i + r + 1); i1++)
			for (int j1 = 0; j1 < r + 1; j1++)
				nAverage += brightness[i1][j1];
		average = nAverage / n;
		for (int i1 = max(0, i - r); i1 < min(Isize, i + r + 1); i1++)
			for (int j1 = 0; j1 < r + 1; j1++)
			{
				double b = brightness[i1][j1];
				sqrnDeviation += (b - average) * (b - average);
				//sqrnDeviation += b * b;
			}
		threshold = average + k * sqrt(sqrnDeviation / n);
		//threshold = average + k * sqrt(-average * average + sqrnDeviation / n);
		if (brightness[i][0] < threshold) make_black(i, 0); else make_white(i, 0);

		for (int j = 1; j < Jsize; j++)
		{
			n = (min(Jsize, j + r) - max(0, j - r)) * (min(Isize, i + r) - max(0, i - r));
			for (int k1 = i - r; k1 < i + r + 1; k1++)
			{
				double b1 = 0;
				if (j - r - 1 >= 0 && k1 >= 0 && k1 < Isize) b1 = brightness[k1][j - r - 1];
				double b2 = 0;
				if (j + r < Jsize && k1 >= 0 && k1 < Isize) b2 = brightness[k1][j + r];
				nAverage -= b1;
				nAverage += b2;
			}
			average = nAverage / n;
			for (int k1 = i - r; k1 < i + r + 1; k1++)
			{
				double b1 = average;
				if (j - r - 1 >= 0 && k1 >= 0 && k1 < Isize) b1 = brightness[k1][j - r - 1];
				double b2 = average;
				if (j + r < Jsize && k1 >= 0 && k1 < Isize) b2 = brightness[k1][j + r];
				//sqrnDeviation -= b1 * b1;
				//sqrnDeviation += b2 * b2;
				sqrnDeviation -= (b1 - average) * (b1 - average);
				sqrnDeviation += (b2 - average) * (b2 - average);
			}

			threshold = average + k * sqrt(sqrnDeviation / n);
			//threshold = average + k * sqrt(-average * average + sqrnDeviation / n);
			if (brightness[i][j] < threshold) make_black(i, j); else make_white(i, j);
		}
	}
}

int main()
{
	img = imread("../test1.png", IMREAD_COLOR);
	if (img.empty())
	{
		std::cout << "error" << endl;
		return -1;
	}
	Isize = img.rows;
	Jsize = img.cols;

	cvtColor(img, img, COLOR_BGR2GRAY);

	double** brightness = new double* [Isize];
	for (int i = 0; i < Isize; i++)
		brightness[i] = new double[Jsize];
	
	for (int i = 0; i < Isize; i++)
		for (int j = 0; j < Jsize; j++)
		{
			brightness[i][j] = img.at<uchar>(i, j);
		}

	niblack_algorithm(brightness);

	namedWindow("window", WINDOW_AUTOSIZE);
	imshow("window", img);
	waitKey(0);

	destroyWindow("window");
	for (int i = 0; i < Isize; i++)
	{
		delete[] brightness[i];
	}
	delete[] brightness;
	return 0;
}