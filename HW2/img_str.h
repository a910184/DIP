#pragma once
#include<stdint.h>

#pragma pack(push)
#pragma pack(1)
typedef struct {
	uint16_t id;
	uint32_t file_sz;
	uint32_t reserved;
	uint32_t bitmap_data_offset;
} ImageFileHeader;

typedef struct {
	uint32_t bitmap_header_sz;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t bitmap_data_sz;
	uint32_t h_res;
	uint32_t v_res;
	uint32_t used_colors;
	uint32_t important_colors;
} ImageInfoHeader;
#pragma pack(pop)

typedef enum {
	FILE_OPEN_SUCCESS,
	FILE_OPEN_FAIL,
	FILE_CLOSE_SUCCESS,
	FILE_CLOSE_FAIL,
	FILE_NOT_OPENED,
	FILE_READ_FAIL,
	FILE_READ_SUCCESS,
	FILE_WRITE_FAIL,
	FILE_WRITE_SUCCESS,
	FILE_PARSE_FAIL,
	DATA_BUF_NOT_AVAILABLE,
	PARAM_UPDATE_SUCCESS
} FResult;