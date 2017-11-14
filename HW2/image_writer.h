#pragma once

#include"img_str.h"
#include<stdio.h>
#include"timage.h"
#include<string>

template<class T>
class ImageWriter {
public:

	ImageWriter();
	ImageWriter(std::string filePath);
	~ImageWriter();

	FResult createFile(std::string filePath);
	void registerImagePtr(TImage<uint8_t>* pImage);

	void setHeight(size_t height);
	void setWidth(size_t width);
	void setPlane(int plane);
	void setChannel(int channel);

	FResult write();

	void configFileHeader(ImageFileHeader& header);
	void configInfoHeader(ImageInfoHeader& header);

private:

	FResult closeImage();

	FResult writeParam();
	FResult updateParam();

	FResult writeData();

	FILE* m_fp;
	bool m_isFileOpened;
	bool m_isParamUpdated;
	std::string m_fileName;

	ImageFileHeader m_imgFileHeader;
	ImageInfoHeader m_imgInfoHeader;
	TImage<uint8_t>* m_p_imageData;
};


