#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "main.h"

#include <SDL2/SDL.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


using namespace std;

template <typename Type> 
void read(ifstream &file, Type &result, size_t size) {
    file.read(reinterpret_cast<char*>(&result), size);
}

int read_bmp_header (ifstream &file_bmp, BMPFileHeader &file_header, BMPInfoHeader &info_header) {
    size_t size_struct = sizeof(file_header.bfType)+sizeof(file_header.bfSize)+sizeof(file_header.bfReserved1)+sizeof(file_header.bfReserved2)+sizeof(file_header.bfOffBits);
    read(file_bmp, file_header, size_struct);
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

    char* bmp_name = argv[1];
    ifstream file_bmp( bmp_name,ios_base::in |ios_base::binary);
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

    av_register_all();
    
	// Init SDL with video support;
    SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 0;
    }

    // Open video file
    char* video_name = argv[2];
	AVFormatContext* format_context = NULL;     // pointer to AVFormatContexts

    // file header and info about file format
    int err;     //error info, for av_strerror
    err = avformat_open_input(&format_context, video_name, 0, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to open input file\n"<< endl;
        /*   char buf[128];
        av_strerror(err, buf, sizeof(buf));
        cout << buf<< endl;*/
		return 0;
	}

    // Retrieve stream information
    err = avformat_find_stream_info(format_context, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to find stream info\n"<< endl;
		return  0;
        }

    av_dump_format(format_context, 0, argv[2], 0);

    // Find the first video stream
    int video_stream;
   	for (video_stream = 0; video_stream < format_context->nb_streams; video_stream++) {
		if (format_context->streams[video_stream]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			break;
		}
    }
	if (video_stream == format_context->nb_streams) {
		cout << "ffmpeg: Unable to find video stream\n"<< endl;
		return 0;
	}

    // Get a pointer to the codec context for the video stream
    AVCodecContext* codec_context = NULL;
    AVCodecParameters* par = format_context->streams[video_stream]->codecpar;
    AVCodec* codec = avcodec_find_decoder(par->codec_id);     // find decoder

    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        cout <<  "Could not allocate a decoding context\n" << endl;
        return 0;
    }

    err = avcodec_parameters_to_context (codec_context, par);
    if (err < 0) {
		cout << "Could not convert parameters to context\n" << endl;
		return 0;
    }

    // Open codec
 	err = avcodec_open2(codec_context, codec, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to open codec\n" << endl;
		return 0;
    }


    SDL_Window *screen = SDL_CreateWindow("YUV420",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          codec_context->width, codec_context->height,
                                          SDL_WINDOW_OPENGL);
    //    SDL_Overlay* bmp = SDL_CreateYUVOverlay(codec_context->width, codec_context->height,
	//										SDL_YV12_OVERLAY, screen);

    SDL_Renderer* renderer =  SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture =  SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV,
                                              SDL_TEXTUREACCESS_TARGET,
                                              codec_context->width, codec_context->height);


    return 1;
}

