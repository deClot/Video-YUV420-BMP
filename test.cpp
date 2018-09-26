#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "main.h"
#include "work_bmp.h"
#include "work_video.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/rational.h>
}

using namespace std;

int main(int argc, char **argv){
    //  Open file
    // Read header of BMP file
    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    char* bmp_name = argv[1];
    ifstream file_bmp( bmp_name,ios_base::in |ios_base::binary);
    if (!file_bmp) {
        cout << "Error opening file" << endl;
        return 0;
    }

    read_bmp_header (file_bmp, file_header, info_header);

    // Read RGB
    unsigned int size_total = info_header.biHeight*info_header.biWidth;
    cout << "width bmp = " << info_header.biWidth << "  height bmp = " <<info_header.biHeight <<endl;
    RGB *rgb = new RGB[size_total];

    read_bmp_rgb (file_bmp, file_header, info_header, rgb);

    // Convert RGB to YUV420
    YUV *yuv = new YUV[size_total];
    RGBtoYUV (rgb, size_total, yuv);

    uint8_t *inputBufferY = new uint8_t[size_total];
    uint8_t *inputBufferU = new uint8_t[size_total/4];
    uint8_t *inputBufferV = new uint8_t[size_total/4];

    for ( unsigned int i = 0; i < size_total; i++){
        *inputBufferY++ = uint8_t(yuv[i].Y);
    }
    for ( unsigned int i = 0; i < size_total; i=i+4){
        *inputBufferU++ =  uint8_t(yuv[i].U);
        *inputBufferV++ =  uint8_t(yuv[i].V);
    }

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
    */

    int err = 0;

    char* video_name = argv[2];
    char* output_name = argv[3];
	AVFormatContext *format_context = NULL, *format_context_out = NULL;     // pointer to AVFormatContexts
    AVCodecContext *codec_context = NULL, *codec_context_out = NULL;
    int video_stream;

    err = open_input_video(video_name, &format_context, &codec_context, &video_stream);

    err = open_output_video(output_name, codec_context, &format_context_out, &codec_context_out);

    err = write_output_file_header(format_context_out);

    AVPacket packet, *packet_out = av_packet_alloc();;
    AVFrame *frame     = av_frame_alloc();
    AVFrame *frame_out = av_frame_alloc() ;

    if (!frame) {
        cout <<"Could not allocate frame" << endl;
        return 0;
    }

    /** Read one frame from the input file into a temporary packet. */
    while (av_read_frame(format_context, &packet) >= 0) {  // Return the next frame of a stream
    // Check packet is video stream?
        if (packet.stream_index == video_stream) {
            // Decode video frame
			//avcodec_decode_video2(codec_context, frame, &frame_finished, &packet);    it was

            err = avcodec_send_packet (codec_context, &packet);
            if (err < 0) {
                cout << "avcodec_send_packet error: " << err << endl;
                break;
            }

            while (err  >= 0) {
                err = avcodec_receive_frame(codec_context, frame);
                if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
                    break;
                }
                else if (err < 0) {
                    cout << "avcodec_receive_frame error: " << err << endl;
                    return err;
                }
                if (err >= 0) {
                    memcpy(frame->data[0], inputBufferY, size_total);
                    memcpy(frame->data[1], inputBufferU, size_total/4);
                    memcpy(frame->data[2], inputBufferV, size_total/4);
                    encode_frame(codec_context_out, frame_out, &packet) ;
                }
            }
        }
    }
    return 0;
}

