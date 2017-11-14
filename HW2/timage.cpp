#include"timage.h"

template<class T>
TImage<T>::TImage(size_t width, size_t height, size_t channel) :
	m_pImageData(nullptr),
	m_height(height),
	m_width(width),
	m_channel(channel) {

	size_t L = channel;
	size_t N = height;
	size_t M = width;

	m_pImageData = new T *[N];
	for (int i = 0; i < N; i++) {
		m_pImageData[i] = new T[M * L];
		for (int j = 0; j < M; j++)
			m_pImageData[i][j] = T(0);
	}

}

template<class T>
TImage<T>::TImage(TImage& image) {
	m_height = image.m_height;
	m_width = image.m_width;
	m_channel = image.m_channel;

	m_pImageData = new T *[m_height];
	for (int i = 0; i < m_height; i++) {
		m_pImageData[i] = new T[m_width * m_channel];
		for (int j = 0; j < m_width*m_channel; j++)
			m_pImageData[i][j] = image.m_pImageData[i][j];
	}
}

template<class T>
TImage<T>::~TImage() {
	for (int i = 0; i < m_height; i++) {
		delete[] m_pImageData[i];
	}
	delete[] m_pImageData;
}

template<class T>
size_t TImage<T>::getSize() {
	return m_height*m_width*m_channel*sizeof(T);
}

template<class T>
T** TImage<T>::getImagePtr() {
	return m_pImageData;
}

template<class T>
size_t TImage<T>::getHeight() {
	return m_height;
}

template<class T>
size_t TImage<T>::getWidth() {
	return m_width;
}

template<class T>
size_t TImage<T>::getChannel() {
	return m_channel;
}



template class TImage<unsigned char>;
template class TImage<double>;
template class TImage<CMX>;

