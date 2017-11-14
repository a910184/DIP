#pragma once

#include<string>
#include<stdio.h>
#include"timage.h"
#include"img_str.h"

class ImageReader {
	

public:
	ImageReader();
	ImageReader(std::string filePath);
	~ImageReader();
	FResult openImage(std::string filePath);
	FResult closeImage();
	FResult readImage();
	ImageFileHeader* getFileHeader();
	ImageInfoHeader* getInfoHeader();
	TImage<uint8_t>* getImage();

private:

	FResult parseParam();

	FILE* m_fp;
	bool m_isFileOpened;
	bool m_isFileParsed;
	std::string m_fileName;

	uint8_t header_buffer[16];
	ImageFileHeader m_imgFileHeader;
	ImageInfoHeader m_imgInfoHeader;
	TImage<uint8_t>* m_p_imageData;
	
};