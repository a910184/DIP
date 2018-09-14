// ConsoleApplication1.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"stdint.h"
#define _USE_MATH_DEFINES
#include<math.h>
using namespace std;
using namespace cv;

Mat darkChannel(Mat src)
{
	Mat rgbmin = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Mat dark = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Vec3b intensity;

	for (int m = 0; m<src.rows; m++)
	{
		for (int n = 0; n<src.cols; n++)
		{
			intensity = src.at<Vec3b>(m, n);
			rgbmin.at<uchar>(m, n) = min(min(intensity.val[0], intensity.val[1]), intensity.val[2]);
		}
	}

	//模板尺寸  
	int scale = 7;
	//cout << "Please enter the mask scale: " << endl;  
	//cin >> scale;  

	//边界扩充  
	int radius = (scale - 1) / 2;
	Mat border;
	//由于要求最小值，所以扩充的边界可以用复制边界填充  
	copyMakeBorder(rgbmin, border, radius, radius, radius, radius, BORDER_REPLICATE);

	//最小值滤波  
	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			//选取兴趣区域  
			Mat roi;
			roi = border(Rect(i, j, scale, scale));

			//求兴趣区域的最小值  
			double minVal = 0; double maxVal = 0;
			Point minLoc = 0; Point maxLoc = 0;
			minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc, noArray());

			dark.at<uchar>(Point(i, j)) = (uchar)minVal;
		}
	}
	return dark;
}
uchar light(vector<uchar> inputIamgeMax)
{
	uchar maxA;
	int32_t max = 0;
	int i;
	for (i = 0; i < inputIamgeMax.size() - 1; i++)
	{
		max = max+inputIamgeMax[i];
		printf("\nmaxA %d\n", max);
	}
	printf("%d", max/i);
	maxA = max / i;
	
	return maxA;
}


int main(){
	Mat image;
	
	image = imread("P_20171210_202824_vHDR_Auto.jpg",IMREAD_COLOR);
	Mat invertimage = Mat::zeros(image.size(), image.type());
////invert
	for (int x = 0; x < image.rows; x++){
		for (int y = 0; y < image.cols; y++) {
			for (int c = 0; c < 3; c++){
				invertimage.at<Vec3b>(x, y)[c] = 255 - image.at<Vec3b>(x, y)[c];
			}
		}
	}
	Mat ycrcbinvert;
	cvtColor(invertimage, ycrcbinvert, CV_BGR2YCrCb);
//transmission t(x) = c*255-I(x)+a+b , c=1.06
	//estimate a = avg/1.6
	Mat ycrcbimage;
	cvtColor(image,ycrcbimage, CV_BGR2YCrCb);
	int avg = 0;
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			avg = avg+ycrcbimage.at<Vec3b>(x, y)[0];
		}
	}
	avg = avg / (image.rows*image.cols);
	avg = (int)avg / 1.6;
	printf("raw image avg %d", avg);
	//estimate b ,if b>= 220 perform -1*(RGB`s maximum - 220)*avg/50.else if b<= 40 perform (abs(RGB`s maximum-40))*avg/35
	uint8_t maximum;
	Mat b =  Mat::zeros(image.size(), CV_16S);
	for (int  x = 0; x < image.rows; x++){
		for (int y = 0; y < image.cols; y++){
				if ((image.at<Vec3b>(x, y)[0] >= image.at<Vec3b>(x, y)[1]) &&(image.at<Vec3b>(x, y)[0] >= image.at<Vec3b>(x, y)[2])) {
					maximum = image.at<Vec3b>(x, y)[0];
				}
				else if ((image.at<Vec3b>(x, y)[1] >= image.at<Vec3b>(x, y)[0]) && (image.at<Vec3b>(x, y)[1] >= image.at<Vec3b>(x, y)[2])) {
					maximum = image.at<Vec3b>(x, y)[1];
				}
				else if ((image.at<Vec3b>(x, y)[2] >= image.at<Vec3b>(x, y)[0]) && (image.at<Vec3b>(x, y)[2] >= image.at<Vec3b>(x, y)[1])) {
					maximum = image.at<Vec3b>(x, y)[2];
				}
			
				if (maximum >= 220) {
					b.at< short>(x,y) = -1 * (maximum - 220)*avg / 50;
				}

				else if (maximum <= 40) {
					b.at< short>(x,y) = (abs( maximum - 40))*avg / 35;
				}
		}
	}
	
	Mat transmission = Mat::zeros(image.size(),CV_8UC3);
	for (int x = 0; x < image.rows; x++){
		for (int y = 0; y < image.cols; y++){
			for (int c = 0; c < 3; c++) {
				transmission.at<Vec3b>(x, y)[c] = 1.06*255-ycrcbinvert.at<Vec3b>(x, y)[0] + avg + b.at< short>(x, y);
			}
		}
	}

	//printf("here");
	//atomspheric light
	Mat darkChannel1 = darkChannel(invertimage);
	Mat temp; darkChannel1.copyTo(temp);
	vector<Point> darkMaxPoint;
	vector<uchar> inputMax;
	for (long i = 0; i < ((darkChannel1.rows*darkChannel1.cols) / 10000); i++)
	{
		double minVal = 0; double maxVal = 0;
		Point minLoc = 0; Point maxLoc = 0;
		minMaxLoc(temp, &minVal, &maxVal, &minLoc, &maxLoc, noArray());

		darkMaxPoint.push_back(maxLoc);
		inputMax.push_back(invertimage.at<uchar>(maxLoc));
		circle(temp, maxLoc, 5, Scalar(0), 1, 8, 0);
		temp.at<uchar>(maxLoc) = temp.at<uchar>(minLoc);
	}

	uchar A = light(inputMax);
//the set is 0.1% lower of dark channel  and 10% of this set`s average RGB value as atom light atomspheric light
	//printf("maxA %d", A);
	//restore haze image
	Mat J(image.rows, image.cols, CV_8UC3, Scalar(180, 120, 50));
	Mat temp1(image.rows, image.cols, CV_8UC3, Scalar(180, 120, 50));

	//subtract(image, Scalar(A, A, A), temp1);  
	temp1 = abs(image - Scalar(A, A, A));
	double t0 = 0.1;

	Scalar T1;
	Vec3b intsrc;

	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			T1 = transmission.at<Vec3b>(Point(i, j));
			intsrc = invertimage.at<Vec3b>(Point(i, j));
			double tmax = (T1.val[0] / 255) < t0 ? t0 : (T1.val[0] / 255);

			for (int k = 0; k < 3; k++)
			{
				J.at<Vec3b>(Point(i, j))[k] = abs((intsrc.val[k] - A) / tmax + A) > 255 ? 255 : abs((intsrc.val[k] - A) / tmax + A);
			}
		}
	}
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			for (int c = 0; c < 3; c++) {
				J.at<Vec3b>(x, y)[c] = 255 - J.at<Vec3b>(x, y)[c];
			}
		}
	}
	imwrite("outputImage1.jpg", J);
	imshow("fianl project",J);
	waitKey(0);
	return 1;
}

