#ifndef WORK_BMP
#define WORK_BMP

#include <iostream>
#include "main.h"
using namespace std;

template <typename Type> 
void read(ifstream &file, Type &result, size_t size);

int read_bmp_header (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header);

int read_bmp_rgb (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header, RGB *rgb);

int RGBtoYUV (RGB *rgb, unsigned int size_total, YUV *yuv);
#endif
