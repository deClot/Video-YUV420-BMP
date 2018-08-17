#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "main.h"

using namespace std;

template <typename Type> 
void read(ifstream &file, Type &result, size_t size) {
    file.read(reinterpret_cast<char*>(&result), size);
}

int read_bmp_header (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header) {
    read(file_bmp, file_header, sizeof(file_header)-2);  // -2 due to adding 2 bytes to sizeof(file_header)=16
    read(file_bmp, info_header, sizeof(info_header));

    if (file_header.bfType != 0x4D42) {   // check format file - BM
        cout << "Error: file is not BMP file. "<< endl;
        return 0;
    }
    if (info_header.biBitCount != 24) {       // check size of header - 40 (without compression)
        cout << "Error: file is not 24 bit file. "<< std::endl;
        return 0;
    }
    if (info_header.biSize != 40) {       // check size of header - 40 (without compression)
        cout << "Error: file is not BMP file or include compression. "<< std::endl;
        return 0;
    }
    return 1;
}

int read_bmp_rgb (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header, RGB *rgb) {

    int padding = ((info_header.biWidth * (info_header.biBitCount / 8)) % 4) & 3;

    for (unsigned int i = 0; i < info_header.biHeight; i++) {
        for (unsigned int j = 0; j < info_header.biWidth; j++) {
            read(file_bmp, rgb[i*info_header.biHeight+j].rgbRed, info_header.biBitCount / 24); // 8*3; 8 for byte and 3 for number of bytes
            read(file_bmp, rgb[i*info_header.biHeight+j].rgbGreen, info_header.biBitCount / 24);
            read(file_bmp, rgb[i*info_header.biHeight+j].rgbBlue, info_header.biBitCount / 24);
        }
        file_bmp.seekg(padding, ios_base::cur);    // from current position move to + padding
    }
    /*   for (unsigned int i = 0; i < info_header.biHeight*info_header.biWidth; i++) {
        //  for (unsigned int j = 0; j < info_header.biWidth; j++) {
         cout << hex
         << +rgb[i].rgbRed << " "
             //    << +rgb[i][j].rgbGreen << " "
             //   << +rgb[i][j].rgbBlue << " "
         << endl;
         //   }
         cout << endl;
         }
    */
    return 1;
}

int RGBtoYUV420 (RGB **rgb, int Height, int Width, YUV420 *yuv) {

    for (unsigned int i = 0; i < Height; i++) {
        for (unsigned int j = 0; j < Width; j = j+4) {
            
        }
    }
    return 1;
    }

int main(int argc, char **argv){

    //  Open file
    ifstream file_bmp( "qq.bmp",ios_base::in |ios_base::binary);
    if (!file_bmp) {
        cout << "Error opening file" << endl;
        return 0;
    }

    // Read header of BMP file
    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    read_bmp_header (file_bmp, file_header, info_header);

    // Read RGB
    RGB *rgb = new RGB[info_header.biHeight*info_header.biWidth];
    //    for (unsigned int i = 0; i < info_header.biHeight; i++) {
    //  rgb[i] = new RGB[info_header.biWidth];
    //}

    read_bmp_rgb (file_bmp, file_header, info_header, rgb);

    YUV420 *yuv = new YUV420[(info_header.biHeight*info_header.biWidth)/4];
    //   RGBtoYUV420 (rgb, info_header.biHeight, info_header.biWidth, yuv);

    return 1;
}

