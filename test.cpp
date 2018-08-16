#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>


#include "main.h"

using namespace std;

template <typename Type>
void read(std::ifstream &fp, Type &result, std::size_t size) {
  fp.read(reinterpret_cast<char*>(&result), size);
}

//void open_bmp (string name_bmp_file) {
  // file *fbmp = fopen( name_bmp_file, "rb" );
  // if( !fbmp ) return NULL;

//  string str;
  

//  }
//}
int main(int argc, char **argv){

  ifstream file_bmp( "LAND.BMP",ios_base::in |ios_base::binary);
  if (!file_bmp) {
    cout << "Error opening file" << endl;
    return 0;
  }

  BMPFileHeader file_header;    
  BMPInfoHeader info_header;
  read(file_bmp, file_header, sizeof(file_header)-2);
  read(file_bmp, info_header, sizeof(info_header));
  
  /*  read(file_bmp, bfh.bfType, sizeof(bfh.bfType));
  read(file_bmp, bfh.bfSize, sizeof(bfh.bfSize));
  read(file_bmp, bfh.bfReserved1, sizeof(bfh.bfReserved1));
  read(file_bmp, bfh.bfReserved2, sizeof(bfh.bfReserved2));
  read(file_bmp, bfh.bfOffBits, sizeof(bfh.bfOffBits));
  read(file_bmp, bih.biSize, sizeof(bih.biSize)); */

  if (file_header.bfType != 0x4D42) {   // check format file - BM
      cout << "Error: file is not BMP file. "<< std::endl;
      return 0;
  }

  if (info_header.biSize != 40) {       // check size of header - 40 (without compression)
      cout << "Error: file is not BMP file or include compression. "<< std::endl;
      return 0;
  }

}

