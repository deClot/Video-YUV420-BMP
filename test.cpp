#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


#include "main.h"

using namespace std;

template <typename Type>
void read(ifstream &file, Type &result, size_t size) {
  file.read(reinterpret_cast<char*>(&result), size);
}

unsigned char bitextract(const unsigned int byte, const unsigned int mask) {
    int
        maskBufer = mask,
        maskPadding = 0;
 
    while (!(maskBufer & 1)) {
        maskBufer >>= 1;
        //       cout << "!!!" << endl;
        maskPadding++;
    }
 
    return (byte & mask) >> maskPadding;
}

int main(int argc, char **argv){

  ifstream file_bmp( "qq.bmp",ios_base::in |ios_base::binary);
  if (!file_bmp) {
    cout << "Error opening file" << endl;
    return 0;
  }

  BMPFileHeader file_header;
  BMPInfoHeader info_header;
  read(file_bmp, file_header, sizeof(file_header)-2);
  read(file_bmp, info_header, sizeof(info_header));

  if (file_header.bfType != 0x4D42) {   // check format file - BM
      cout << "Error: file is not BMP file. "<< std::endl;
      return 0;
  }

  if (info_header.biSize != 40) {       // check size of header - 40 (without compression)
      cout << "Error: file is not BMP file or include compression. "<< std::endl;
      return 0;
  }

  RGBQUAD **rgb = new RGBQUAD*[info_header.biHeight];
  for (unsigned int i = 0; i < info_header.biHeight; i++) {
      rgb[i] = new RGBQUAD[info_header.biWidth];
  }

  int padding = ((info_header.biWidth * (info_header.biBitCount / 8)) % 4) & 3;

  unsigned int bufer;

  int colorsCount = info_header.biBitCount >> 3;    // from bit to byte
  int bitsOnColor = info_header.biBitCount / colorsCount;
  int maskValue   = (1 << bitsOnColor) - 1;

  int redMask   = maskValue << (bitsOnColor * 2);
  int greenMask = maskValue << bitsOnColor;
  int blueMask  = maskValue;

  //  cout << redMask << endl;

  for (unsigned int i = 0; i < info_header.biHeight; i++) {
      for (unsigned int j = 0; j < info_header.biWidth; j++) {
          read(file_bmp, bufer, info_header.biBitCount / 8);
 
          rgb[i][j].rgbRed = bitextract(bufer, redMask);
          rgb[i][j].rgbGreen = bitextract(bufer, greenMask);
          rgb[i][j].rgbBlue = bitextract(bufer, blueMask);
      }
      file_bmp.seekg(padding, ios_base::cur);    // from current position move to + padding
  }
 
  /*  for (unsigned int i = 0; i < info_header.biHeight; i++) {
      for (unsigned int j = 0; j < info_header.biWidth; j++) {
          cout << hex
               << +rgb[i][j].rgbRed << " "
               << +rgb[i][j].rgbGreen << " "
               << +rgb[i][j].rgbBlue << " "
               << endl;
        }
      cout << endl;
    }
  */
    return 1; 
}

