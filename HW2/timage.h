#pragma once
#include<stdint.h>
#include<stdio.h>
#include<iostream>

#define _USE_MATH_DEFINES

#include<cmath>



class CMX {
public:

	enum {
		CMX_DEG,
		CMX_RAD
	};

	CMX() {
		real = 0;
		image = 0;
	}

	CMX(int i) {
		real = i;
		image = 0;
	}
	CMX(double r, double i) {
		real = r;
		image = i;
	}
	CMX(double ampl, double phase, int type) {
		if (type == CMX_DEG) {
			phase /= 180.0;
			phase *= 3.14159265358979323846;
		}
		real = ampl*cos(phase);
		image = ampl*sin(phase);
	}

	CMX& operator=(const CMX &complex) {
		if (this == &complex)
			return *this;

		real = complex.real;
		image = complex.image;
		return *this;
	}

	CMX operator+(const CMX& complex) {
		return CMX(real + complex.real, image + complex.image);
	}
	CMX operator-(const CMX& complex) {
		return CMX(real - complex.real, image - complex.image);
	}
	CMX operator*(const CMX& complex) {
		return CMX(real*complex.real - image*complex.image, 
			real*complex.image + image*complex.real);
	}

	double getAmp() {
		return sqrtf(real*real + image* image);
	}

	double getPaser() {
		return atan2f(image, real);
	}

	double real = 0.0;
	double image = 0.0;
	
};



template<class T>
class TImage {

public:
	TImage(size_t width, size_t height, size_t channel);
	TImage(TImage& image);
	~TImage();

	T getPixel(unsigned int x, unsigned int y, unsigned int channel);
	void setPixel(unsigned int x, unsigned int y, unsigned int channel, T value);
	size_t getSize();
	T** getImagePtr();
	size_t getHeight();
	size_t getWidth();
	size_t getChannel();
	T** m_pImageData;

private:
	size_t m_height;
	size_t m_width;
	size_t m_channel;
};


template<class T>
T TImage<T>::getPixel(unsigned int x, unsigned int y, unsigned int channel) {
	// TODO modify to use channel
	return m_pImageData[y][m_channel * x + channel -  1];
}

template<class T>
void TImage<T>::setPixel(unsigned int x, unsigned int y, unsigned int channel, T value) {
	if( x >= m_width ||
		x < 0 ||
		y >= m_height ||
		y < 0 || 
		channel > m_channel ||
		channel < 0) {
		printf("<TImage> access out of range (%d, %d) \n", x, y);
	}
	// TODO modify to use channel
	//printf("Set m_pImageData[%d] (%d,%d) = %d\n", y*m_width*m_channel + x,x, y, value);

	m_pImageData[y][m_channel * x + channel - 1] = value;
}
