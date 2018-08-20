#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O

#include "main.h"

using namespace std;
using namespace cv;

template <typename Type> 
void read(ifstream &file, Type &result, size_t size) {
    file.read(reinterpret_cast<char*>(&result), size);
}

int read_bmp_header (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header) {

    read(file_bmp, file_header, sizeof(file_header));
    read(file_bmp, info_header, sizeof(info_header));

    if (file_header.bfType != 0x4D42) {   // check format file - BM
        cout << "Error: file is not BMP file. "<< endl;
        return 0;
    }
    if (info_header.biBitCount != 24) {      
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

int RGBtoYUV (RGB *rgb, unsigned int size_total, YUV *yuv) {
    float Wr = 0.299;
    float Wb = 0.114;
    float Wg = 1- Wr - Wb;
    float Umax = 0.426;
    float Vmax = 0.615;

    for (unsigned int i = 0; i < size_total; i++) {
        //       cout << dec << +rgb[i].rgbRed<< endl;
        yuv[i].Y = Wr*rgb[i].rgbRed + Wg*rgb[i].rgbGreen + Wb*rgb[i].rgbBlue;
        yuv[i].U = Umax*(rgb[i].rgbBlue-yuv[i].Y)/(1-Wb);
        yuv[i].V = Vmax*(rgb[i].rgbRed-yuv[i].Y)/(1-Wr);
    }
    return 1;
    }

int main(int argc, char **argv){

    //  Open file

    char* file_name = argv[1];
    ifstream file_bmp( file_name,ios_base::in |ios_base::binary);
    if (!file_bmp) {
        cout << "Error opening file" << endl;
        return 0;
    }

    // Read header of BMP file
    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    read_bmp_header (file_bmp, file_header, info_header);

    // Read RGB
    unsigned int size_total = info_header.biHeight*info_header.biWidth;
    RGB *rgb = new RGB[size_total];

    read_bmp_rgb (file_bmp, file_header, info_header, rgb);

    // Convert RGB to YUV420
    YUV *yuv = new YUV[size_total];
    RGBtoYUV (rgb, size_total, yuv);


    /*    ofstream file_out ("bmp.yuv");

    for ( unsigned int i = 0; i < size_total; i++){
        file_out << yuv[i].Y;
    }
    for ( unsigned int i = 0; i < size_total; i += 4){
        file_out << yuv[i].U;
    }
    for ( unsigned int i = 0; i < size_total; i += 4){
        file_out << yuv[i].V;
    }
    file_out.close();
    */

    return 1;
}

