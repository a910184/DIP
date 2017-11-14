#include"image_writer.h"
#include<stdio.h>
#include<string>
#include"img_str.h"

template<class T>
ImageWriter<T>::ImageWriter() :
	m_p_imageData(nullptr),
	m_fp(nullptr),
	m_isFileOpened(false),
	m_isParamUpdated(false),
	m_fileName("") {


}

template<class T>
ImageWriter<T>::ImageWriter(std::string filePath) :
	m_p_imageData(nullptr),
	m_fp(nullptr),
	m_isFileOpened(false),
	m_isParamUpdated(false),
	m_fileName(filePath) {
	
	createFile(filePath);

}

template<class T>
ImageWriter<T>::~ImageWriter() {
	// Do not delete m_p_imageData, 
	// the actual data was not manage by this class
	closeImage();
}

template<class T>
FResult ImageWriter<T>::createFile(std::string filePath) {

	// If there is opened file before, close file and open new
	if (m_isFileOpened) {
		FResult res;
		res = closeImage();
		if (res != FResult::FILE_CLOSE_SUCCESS) {
			printf("<ImageWriter> can't not createFile, because pra");
		}
	}

	// Try to open file
	int error = fopen_s(&m_fp, filePath.c_str(), "wb");

	// If there have any error
	if (m_fp == NULL) {
		printf("<ERRROR> Cannot create file \"%s\"\n", filePath.c_str());
		m_isFileOpened = false;
		return FResult::FILE_OPEN_FAIL;
	}

	m_isFileOpened = true;
	return FResult::FILE_OPEN_SUCCESS;
}

template<class T>
void ImageWriter<T>::registerImagePtr(TImage<uint8_t>* pImage) {
	m_p_imageData = pImage;
}

template<class T>
void ImageWriter<T>::setHeight(size_t height) {
	m_imgInfoHeader.height = height;
}

template<class T>
void ImageWriter<T>::setWidth(size_t width) {
	m_imgInfoHeader.width = width;
}

template<class T>
void ImageWriter<T>::setPlane(int plane) {
	m_imgInfoHeader.planes = plane;
}

template<class T>
void ImageWriter<T>::setChannel(int channel) {
	switch (channel) {
		case 1:
			m_imgInfoHeader.used_colors = 0;
			break;
		case 3:
			m_imgInfoHeader.used_colors = 1;
			break;
		default:
			printf("<ImageWriter> Unavailable channel setting : %d\n", channel);
	}
	
}

template<class T>
FResult ImageWriter<T>::write() {
	if (!m_isFileOpened)
		return FResult::FILE_NOT_OPENED;
	FResult res;

	res = writeParam();
	if (res != FResult::FILE_WRITE_SUCCESS)
		return res;
	
	res = writeData();
	if (res != FResult::FILE_WRITE_SUCCESS)
		return res;

	return FILE_WRITE_SUCCESS;
}

template<class T>
void ImageWriter<T>::configFileHeader(ImageFileHeader & header) {
	// Copy structer memory
	memcpy(&m_imgFileHeader, &header, sizeof(ImageFileHeader));
}

template<class T>
void ImageWriter<T>::configInfoHeader(ImageInfoHeader & header) {
	// Copy structer memory
	memcpy(&m_imgInfoHeader, &header, sizeof(ImageInfoHeader));
}

template<class T>
FResult ImageWriter<T>::closeImage() {
	if (!m_isFileOpened) {
		return FResult::FILE_NOT_OPENED;
	}

	// If return EOF
	if (fclose(m_fp))
		return FILE_CLOSE_FAIL;

	m_isFileOpened = false;
	m_isParamUpdated = false;

	// Do not delete m_p_Image memory self
	// the data was manager by outside

	// Close successful
	return FILE_CLOSE_SUCCESS;
}

template<class T>
FResult ImageWriter<T>::writeParam() {

	if (!m_isFileOpened)
		return FResult::FILE_NOT_OPENED;

	// Must update param
	if (!m_isParamUpdated) {
		updateParam();
	}

	// Check there is valid TImage*
	if (m_p_imageData != nullptr) {

		int countSended = 0;

		// Relocation file pointer
		fseek(m_fp, 0, SEEK_SET);

		// Write header
		countSended = fwrite(&m_imgFileHeader, 1, sizeof(ImageFileHeader), m_fp);
		if (countSended != sizeof(ImageFileHeader)) {
			printf("<ImageWriter> ImageFileHeader write fail, send %d bytes\n", countSended);
		}
		countSended = fwrite(&m_imgInfoHeader, 1, sizeof(ImageInfoHeader), m_fp);
		if (countSended != sizeof(ImageInfoHeader)) {
			printf("<ImageWriter> ImageInfoHeader write fail, send %d bytes\n", countSended);
		}
		
	} 
	else {
		return DATA_BUF_NOT_AVAILABLE;
	}

	return FResult::FILE_WRITE_SUCCESS;
}

template<class T>
FResult ImageWriter<T>::updateParam() {

	// TODO: Update param via m_p_imageData
	return FResult::PARAM_UPDATE_SUCCESS;
}

template<class T>
FResult ImageWriter<T>::writeData() {

	if (!m_isFileOpened) {
		return FResult::FILE_NOT_OPENED;
	}

	// Check there is valid TImage*
	if (m_p_imageData != nullptr) {

		
		// Relocation file pointer to data section
		fseek(m_fp,m_imgFileHeader.bitmap_data_offset, SEEK_SET);

		int countLeft = m_p_imageData->getSize();
		printf("< ImageWriter > Start to write raw file (%d bytes)\n", countLeft);

		int countLoop = 0;
		int countRead = 0;

		printf("< ImageWriter > per ROW %d bytes\n", m_p_imageData->getWidth()* m_p_imageData->getChannel());


		for (int i = 0; i < m_p_imageData->getHeight(); i++) {
			countRead =
				fwrite(
					m_p_imageData->getImagePtr()[i],
					1,
					m_p_imageData->getWidth()* m_p_imageData->getChannel(),
					m_fp);
			countLeft -= countRead;
			printf("< ImageWriter > Write %d bytes, left %d bytes\n", countRead, countLeft);
		}


		if (countLeft > 0) return FResult::FILE_WRITE_FAIL;


	} else {
		return DATA_BUF_NOT_AVAILABLE;
	}

	return FResult::FILE_WRITE_SUCCESS;
}


template class ImageWriter<unsigned char>;
template class ImageWriter<double>;