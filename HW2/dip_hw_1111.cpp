// dip_hw_1111.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "image_reader.h"
#include "image_writer.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include "timage.h"

#include <vector>

#include <iostream>

#define M_PI 3.14159265358979323846

using namespace std;

void rgb2gray(TImage<uint8_t>&, TImage<uint8_t>&);

void DFT_pPx(int x,TImage<CMX>& src, vector<CMX>& colv_Px) {

	CMX result;

	int N = src.getHeight();

	for (int v = 0; v < N; v++) {
		result.real = 0.0;
		result.image = 0.0;
		for (int y = 0; y < N; y++) {
			double d_phase = (double)v*(double)y / (double)N;
			//result = result +
			//	src.getPixel(x, y, 1)*CMX(1, -2.0 * M_PI*d_phase, CMX::CMX_RAD) * CMX(1.0/double(N),0);
			result = result +
				src.getPixel(x, y, 1)*CMX(1, -2.0 * M_PI*d_phase, CMX::CMX_RAD);
		}
		colv_Px.push_back( result );
	}

	return ;
}

void DFT_image(TImage<CMX>& src, TImage<CMX>& dst) {
	vector<CMX> image_row;

	TImage<CMX> temp_dst(src.getWidth(), src.getHeight(), 1);

	image_row.reserve(512);

	int N = src.getHeight();
	int M = src.getWidth();

	//DFT_pPx(256, src, image_row);

	for (int x = 0; x < M; x++) {
		image_row.clear();
		//cout << "Col:" << x << endl;
		DFT_pPx(x, src, image_row);

		for (int v = 0; v < N; v++) {
			temp_dst.setPixel(x, v, 1, image_row.at(v));
		}
		
	}

	image_row.clear();

	CMX result;

	for (int v = 0; v < N; v++) {
		for (int u = 0; u < M; u++) {
			result.real = 0.0;
			result.image = 0.0;
			for (int x = 0; x < M; x++) {
				double d_phase = (double)u*(double)x / (double)M;
				//result = result +
				//	temp_dst.getPixel(x, v, 1)*CMX(1, -2.0 * M_PI*d_phase, CMX::CMX_RAD) * CMX(1.0 / (double)M, 0);
				result = result +
					temp_dst.getPixel(x, v, 1)*CMX(1, -2.0 * M_PI*d_phase, CMX::CMX_RAD);
			}
			
			dst.setPixel(u, v, 1, result);
//			dst.setPixel(u, v, 1, temp_dst.getPixel(u,v,1));
		}
	}


}

ostream &operator<<(ostream& stream, CMX complex) {
	if (complex.image < 0) {
		stream << complex.real << "-i" << -complex.image;
	}
	else {
		stream << complex.real << "+i" << complex.image;
	}
	
	return stream;
}

int main() {

	// Raw inage
	TImage<uint8_t> img1(512, 512, 3);
	TImage<CMX> img2(512, 512, 1);
	TImage<uint8_t> img_output(512, 512, 3);

	ImageReader img_reader;
	img_reader.openImage("Goldhill.bmp");
	img_reader.readImage();

	ImageReader img_fakereader;
	img_fakereader.openImage("lena.bmp");
	img_fakereader.readImage();

	ImageWriter<uint8_t> img_tester;
	img_tester.createFile("test.bmp");
	img_tester.configFileHeader(*img_fakereader.getFileHeader());
	img_tester.configInfoHeader(*img_fakereader.getInfoHeader());


	ImageWriter<uint8_t> img_writer;
	img_writer.createFile("hw1.bmp");
	img_writer.configFileHeader(*img_fakereader.getFileHeader());
	img_writer.configInfoHeader(*img_fakereader.getInfoHeader());

	ImageWriter<uint8_t> img_writer_hw21;
	img_writer_hw21.createFile("hw21.bmp");
	img_writer_hw21.configFileHeader(*img_fakereader.getFileHeader());
	img_writer_hw21.configInfoHeader(*img_fakereader.getInfoHeader());

	ImageWriter<uint8_t> img_writer_hw22;
	img_writer_hw22.createFile("hw22.bmp");
	img_writer_hw22.configFileHeader(*img_fakereader.getFileHeader());
	img_writer_hw22.configInfoHeader(*img_fakereader.getInfoHeader());

	ImageWriter<uint8_t> img_writer_hw3;
	img_writer_hw3.createFile("hw3.bmp");
	img_writer_hw3.configFileHeader(*img_fakereader.getFileHeader());
	img_writer_hw3.configInfoHeader(*img_fakereader.getInfoHeader());

	// Convert to gray
	//rgb2gray(*img_fakereader.getImage(), img1);
	TImage<uint8_t>* p_Image = img_reader.getImage();
	for (int i = 0; i < img2.getHeight(); i++) {
		for (int j = 0; j < img2.getWidth(); j++) {
				img1.setPixel(i, j, 1, p_Image->getPixel(i,j,1));
				img1.setPixel(i, j, 2, p_Image->getPixel(i, j, 1));
				img1.setPixel(i, j, 3, p_Image->getPixel(i, j, 1));
		}
	}

	img_tester.registerImagePtr(&img1);
	img_tester.write();

	

	// Convert to complex 
	for (int i = 0; i < img2.getHeight(); i++) {
		for (int j = 0; j < img2.getWidth(); j++) {
			if ((i + j) % 2 == 0)
				img2.setPixel(i, j, 1, CMX(img1.getPixel(i, j, 1), 0));
			else
				img2.setPixel(i, j, 1, CMX(-img1.getPixel(i, j, 1), 0));
		}
	}

	TImage<CMX> img3(512, 512, 1);
	DFT_image(img2, img3);

	double max_value = 0.0;

	for (int i = 0; i < img1.getHeight(); i++) {
		for (int j = 0; j < img1.getWidth(); j++) {
			double value = 
				sqrtf(img3.getPixel(i, j, 1).real*img3.getPixel(i, j, 1).real +
				img3.getPixel(i, j, 1).image*img3.getPixel(i, j, 1).image);

			if (value > max_value) max_value = value;
		}
	}
	double ampl = 255.0/log(1+max_value);
	printf("test -> %f ", ampl);
	for (int i = 0; i < img1.getHeight(); i++) {
		for (int j = 0; j < img1.getWidth(); j++) {
			double value =
				sqrt(img3.getPixel(i, j, 1).real*img3.getPixel(i, j, 1).real +
					img3.getPixel(i, j, 1).image*img3.getPixel(i, j, 1).image);
			if (i % 64 == 0 || j % 64 == 0)
				printf("%f ", value);
			img_output.setPixel(i, j, 1, (uint8_t)(log(1 + value)*ampl));
			img_output.setPixel(i, j, 2, (uint8_t)(log(1 + value)*ampl));
			img_output.setPixel(i, j, 3, (uint8_t)(log(1 + value)*ampl));
		}
		printf("\n");
	}

	img_writer.registerImagePtr(&img_output);
	img_writer.write();

	TImage<uint8_t> image_padding(512 + 2, 512 + 2, 1);

	// Padding image
	for (int i = 0; i < 512; i++) {
		image_padding.setPixel(i + 1, 0, 1, img1.getPixel(i, 1, 1));
		image_padding.setPixel(i + 1, 513, 1, img1.getPixel(i, 510, 1));
		image_padding.setPixel(0, i+1, 1, img1.getPixel(1, i, 1));
		image_padding.setPixel(513, i+1, 1, img1.getPixel(510, i, 1));
	}
	image_padding.setPixel(0, 0, 1, img1.getPixel(1, 1, 1));
	image_padding.setPixel(0, 513, 1, img1.getPixel(1, 510, 1));
	image_padding.setPixel(513, 0, 1, img1.getPixel(510, 1, 1));
	image_padding.setPixel(513, 513, 1, img1.getPixel(510, 510, 1));


	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			image_padding.setPixel(i+1, j+1, 1, img1.getPixel(i,j,1));
		}
	}

	TImage<uint8_t> mask1(3,3,1);
	TImage<uint8_t> mask2(3,3,1);

	mask1.setPixel(0, 0, 1, 1);
	mask1.setPixel(0, 1, 1, 2);
	mask1.setPixel(0, 2, 1, 1);
	mask1.setPixel(1, 0, 1, 2);
	mask1.setPixel(1, 1, 1, 4);
	mask1.setPixel(1, 2, 1, 2);
	mask1.setPixel(2, 0, 1, 1);
	mask1.setPixel(2, 1, 1, 2);
	mask1.setPixel(2, 2, 1, 1);

	mask2.setPixel(0, 0, 1, 1);
	mask2.setPixel(0, 1, 1, 1);
	mask2.setPixel(0, 2, 1, 1);
	mask2.setPixel(1, 0, 1, 1);
	mask2.setPixel(1, 1, 1, 2);
	mask2.setPixel(1, 2, 1, 1);
	mask2.setPixel(2, 0, 1, 1);
	mask2.setPixel(2, 1, 1, 1);
	mask2.setPixel(2, 2, 1, 1);

	TImage<uint8_t> image_hw21(512, 512, 3);
	TImage<uint8_t> image_hw22(512, 512, 3);
	TImage<uint8_t> image_hw3(512, 512, 3);

	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			int sum;
			sum = 0;
			for (int xi = 0; xi < 3; xi++) {
				for (int yi = 0; yi < 3; yi++) {
					sum += (int)image_padding.getPixel(i + xi, j + yi, 1)*(int)mask1.getPixel(xi, yi, 1);
				}
			}
			sum /= 16;
			image_hw21.setPixel(i, j, 1, sum);
			image_hw21.setPixel(i, j, 2, sum);
			image_hw21.setPixel(i, j, 3, sum);
		}
	}

	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			int sum;
			sum = 0;
			for (int xi = 0; xi < 3; xi++) {
				for (int yi = 0; yi < 3; yi++) {
					sum += (int)image_padding.getPixel(i + xi, j + yi, 1)*(int)mask2.getPixel(xi, yi, 1);
				}
			}
			sum /= 10;
			image_hw22.setPixel(i, j, 1, sum);
			image_hw22.setPixel(i, j, 2, sum);
			image_hw22.setPixel(i, j, 3, sum);
		}
	}

	// Find min max
	float max_ = -255;
	float min_ = 255;

	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			float value =
				(int)image_hw22.getPixel(i,j,1)- (int)image_hw21.getPixel(i,j,1);

			if (value > max_) max_ = value;
			if (value < min_) min_ = value;
		}
	}
	float ampli = 255/(max_);

	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			uint8_t value =
				labs((int)image_hw22.getPixel(i, j, 1) - (int)image_hw21.getPixel(i, j, 1)) *ampli;
			image_hw3.setPixel(i, j, 1, value);
			image_hw3.setPixel(i, j, 2, value);
			image_hw3.setPixel(i, j, 3, value);
		}
	}

	img_writer_hw21.registerImagePtr(&image_hw21);
	img_writer_hw22.registerImagePtr(&image_hw22);
	img_writer_hw3.registerImagePtr(&image_hw3);

	img_writer_hw21.write();
	img_writer_hw22.write();
	img_writer_hw3.write();

	return 0;
}

void rgb2gray(TImage<uint8_t> &source_image, TImage<uint8_t> &dest_image) {
	if ( (source_image.getChannel() != 3) || (dest_image.getChannel() != 1)) {
		printf("Image dimation not suit!\n");
	}
	for (int i = 0; i < source_image.getHeight(); i++) {
		for (int j = 0; j < source_image.getWidth(); j++) {
			uint16_t avg = 0;
			avg += 0.299*source_image.getPixel(i, j, 3);
			avg += 0.587*source_image.getPixel(i, j, 2);
			avg += 0.114*source_image.getPixel(i, j, 1);
			dest_image.setPixel(i, j, 1, avg);
		}
	}
}
