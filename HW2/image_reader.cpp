#include"image_reader.h"
#include"timage.h"
#include<stdio.h>


ImageReader::ImageReader() : 
	m_fp(NULL),
	m_isFileOpened(false),
	m_isFileParsed(false),
	m_fileName(""),
	m_p_imageData(nullptr) {

}

ImageReader::ImageReader(std::string filePath) :
	m_fp(NULL),
	m_isFileOpened(false),
	m_isFileParsed(false),
	m_fileName(""),
	m_p_imageData(nullptr) {
	
	// Read image file
	openImage(filePath);
}

ImageReader::~ImageReader() {
	closeImage();
}

FResult ImageReader::openImage(std::string filePath) {
	// Try to read file
	int error = fopen_s(&m_fp ,filePath.c_str(), "rb");

	// If there have any error
	if (m_fp == NULL) {
		printf("<ERRROR> Cannot open file \"%s\"\n", filePath.c_str());
		m_isFileOpened = false;
		return FResult::FILE_OPEN_FAIL;
	}

	m_isFileOpened = true;

	if (parseParam() != FResult::FILE_READ_SUCCESS)
		return FResult::FILE_PARSE_FAIL;
	else {
		m_isFileParsed = true;
		
		m_imgInfoHeader.bits_per_pixel;
		// Alloc image data buffer with corrbound width and height
		m_p_imageData = 
			new TImage<uint8_t>(m_imgInfoHeader.width, m_imgInfoHeader.height, m_imgInfoHeader.bits_per_pixel / 8);

		return FResult::FILE_OPEN_SUCCESS;
	}
}

FResult ImageReader::closeImage() {
	if (!m_isFileOpened) {
		return FResult::FILE_NOT_OPENED;
	}
		
	// If return EOF
	if (fclose(m_fp))
		return FILE_CLOSE_FAIL;
	
	m_isFileOpened = false;
	m_isFileParsed = false;
	if (m_p_imageData != nullptr) {
		delete m_p_imageData;
		m_p_imageData = nullptr;
	}

	// Close successful
	return FILE_CLOSE_SUCCESS;
}

FResult ImageReader::parseParam() {

	if (!m_isFileOpened) return FResult::FILE_NOT_OPENED;

	// Read first 
	printf("Sizeof ImageFileHeader %d\n", sizeof(ImageFileHeader));
	fread(&m_imgFileHeader, 1, sizeof(ImageFileHeader), m_fp);
	
	if (ferror(m_fp)) {
		// Show error message and return
		printf("<Error> Can't read ImageFileHeader !\n");
		return FResult::FILE_READ_FAIL;
	}

	printf("Sizeof ImageInfoHeader %d\n", sizeof(ImageInfoHeader));
	fread(&m_imgInfoHeader, 1, sizeof(ImageInfoHeader), m_fp);

	if (ferror(m_fp)) {
		// Show error message and return
		printf("<Error> Cant't read ImageInfoHeader !\n");
		return FResult::FILE_READ_FAIL;
	}

	printf("<Image Param> Width %d\n", m_imgInfoHeader.width);
	printf("<Image Param> Height %d\n", m_imgInfoHeader.height);

	return FResult::FILE_READ_SUCCESS;

}

FResult ImageReader::readImage() {
	if (!m_isFileOpened)
		return FResult::FILE_NOT_OPENED;
	if (!m_isFileParsed) {
		printf("< ImageReader > Image havenot parsed\n");
		return FResult::FILE_READ_FAIL;
	}
	if (m_p_imageData == nullptr) {
		printf("< ImageReader > Image data havn't alloc!\n");
	}

	int countLeft = m_p_imageData->getSize();
	printf("< ImageReader > Start to read raw file (%d bytes)\n", countLeft);

	int countLoop = 0;
	int countRead = 0;

	printf("< ImageReader > per ROW %d bytes\n", m_p_imageData->getWidth()* m_p_imageData->getChannel());

	// Relocation file pointer to data section
	fseek(m_fp, m_imgFileHeader.bitmap_data_offset, SEEK_SET);

	for (int i = 0; i < m_p_imageData->getHeight(); i++) {
		countRead =
			fread(
				m_p_imageData->getImagePtr()[i],
				1,
				m_p_imageData->getWidth()* m_p_imageData->getChannel(),
				m_fp);
		countLeft -= countRead;
		printf("< ImageReader > Read %d bytes, left %d bytes\n", countRead, countLeft);
	}


	if (countLeft > 0) return FResult::FILE_READ_FAIL;
	
	return FResult::FILE_READ_SUCCESS;

}

ImageFileHeader * ImageReader::getFileHeader() {
	return &m_imgFileHeader;
}

ImageInfoHeader * ImageReader::getInfoHeader() {
	return &m_imgInfoHeader;
}

TImage<uint8_t>* ImageReader::getImage() {
	return m_p_imageData;
}
