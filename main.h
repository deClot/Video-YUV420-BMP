#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

typedef struct {
    // BIT MAP FILE HEADER - 14 bytes
    unsigned short  bfType;         // 2 bytes, the same as short int
    unsigned int             bfSize;         // 4 bytes
    unsigned short  bfReserved1;    // 2 bytes
    unsigned short  bfReserved2;    // 2 bytes
    unsigned int             bfOffBits;      // 4 bytes
} BMPFileHeader;

typedef struct {
    // BIT MAP INFO HEADER - 40 bytes
    unsigned int    biSize;         // 4 bytes, biSize = 40 bytes
    unsigned int             biWidth;        // 4 bytes
    unsigned int             biHeight;       // 4 bytes
    unsigned short  biPlanes;       // 2 bytes, must be 1
    unsigned short  biBitCount;     // 2 bytes
    unsigned int             biCompression;  // 4 bytes
    unsigned int             biSizeImage;    // 4 bytes, usually 0
    unsigned int             biXPelsPerMeter;// 4 bytes
    unsigned int             biYPelsPerMeter;// 4 bytes
    unsigned int             biClrUsed;      // 4 bytes
    unsigned int             biClrImportant; // 4 bytes
} BMPInfoHeader;


#endif // MAIN_H_INCLUDEDs
