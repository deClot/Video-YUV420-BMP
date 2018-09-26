#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "work_video.h"

using namespace std;

int open_input_video (char* video_name, AVFormatContext** format_context, AVCodecContext** codec_context, int* video_stream) {
    // file header and info about file format
    int err;     //error info, for av_strerror
    err = avformat_open_input(format_context, video_name, 0, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to open input file\n"<< endl;
        /*   char buf[128];
        av_strerror(err, buf, sizeof(buf));
        cout << buf<< endl;*/
		return 0;
	}

    // Retrieve stream information
    err = avformat_find_stream_info(*format_context, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to find stream info\n"<< endl;
		return  0;
    }

    av_dump_format(*format_context, 0, video_name, 0);

    // Find the first video stream
    AVCodec *dec;

    err = av_find_best_stream(*format_context, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (err < 0) {
        cout << "Cannot find a video stream in the input file\n" << endl;
        return 0;
    }
    *video_stream = err;

    // Create decoding context
    AVCodecParameters* par = (*format_context)->streams[*video_stream]->codecpar;

    //   cout << "format bit_rate " << (*format_context)->bit_rate; 
    //    AVCodecContext *ctx;

    *codec_context = avcodec_alloc_context3(dec);
    if (!*codec_context) {
        cout <<  "Could not allocate a decoding context\n" << endl;
        return 0;
    }

    err = avcodec_parameters_to_context (*codec_context, par);
    if (err < 0) {
		cout << "Could not convert parameters to context\n" << endl;
		return 0;
    }

    //    cout << "timebase " << (*codec_context)->time_base.den <<" "<< (*codec_context)->time_base.num <<endl;
    // Init the video decoder
 	err = avcodec_open2(*codec_context, dec, NULL);
	if (err < 0) {
		cout << "ffmpeg: Unable to open codec\n" << endl;
		return 0;
    }

    return 1;
}

int open_output_video(char *filename, AVCodecContext *input_codec_context,
                            AVFormatContext **output_format_context,
                            AVCodecContext **output_codec_context) {
    AVCodecContext *avctx          = NULL;
    AVIOContext *output_io_context = NULL;
    AVStream *stream               = NULL;
    AVCodec *output_codec          = NULL;
    int error;
    /** Open the output file to write to it. */
    if ((error = avio_open(&output_io_context, filename,
                           AVIO_FLAG_WRITE)) < 0) {
        cout <<"Could not open output file" << endl;
        return 0;
    }
    /** Create a new format context for the output container format. */
    if (!(*output_format_context = avformat_alloc_context())) {
        fprintf(stderr, "Could not allocate output format context\n");
        return AVERROR(ENOMEM);
    }

    /** Associate the output file (pointer) with the container format context. */
    (*output_format_context)->pb = output_io_context;
    /** Guess the desired container format based on the file extension. */
    if (!((*output_format_context)->oformat = av_guess_format(NULL, filename,
                                                              NULL))) {
        fprintf(stderr, "Could not find output file format\n");
        return 0;
    }

    //    av_strlcpy((*output_format_context)->filename, filename,
    //           sizeof((*output_format_context)->filename));

    /** Find the encoder to be used by its name. */ //input_codec_context->codec_id
    if (!(output_codec = avcodec_find_encoder(AV_CODEC_ID_RAWVIDEO))) {
        fprintf(stderr, "Could not find encoder.\n");
        return 0;
    }

    /** Create a new stream in the output file container. */
    if (!(stream = avformat_new_stream(*output_format_context, NULL))) {
        fprintf(stderr, "Could not create new stream\n");
        error = AVERROR(ENOMEM);
        return 0;
    }

    avctx = avcodec_alloc_context3(output_codec);
    if (!avctx) {
        fprintf(stderr, "Could not allocate an encoding context\n");
        error = AVERROR(ENOMEM);
        return 0;
    }

    avctx->time_base = (AVRational){1, 25};
    avctx->width  = input_codec_context->width;
    avctx->height = input_codec_context->height;
    avctx->pix_fmt = AV_PIX_FMT_YUV420P;

    //   if ((*output_format_context)->oformat->flags & AVFMT_GLOBALHEADER)
    //  avctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    /** Open the encoder for stream to use it later. */
    if ((error = avcodec_open2(avctx, output_codec, NULL)) < 0) {
        cout << "Could not open output codec " << error<< endl;
        return 0;
    }

    error = avcodec_parameters_from_context(stream->codecpar, avctx);
    if (error < 0) {
        fprintf(stderr, "Could not initialize stream parameters\n");
        return 0;
    }
    /** Save the encoder context for easier access later. */
    *output_codec_context = avctx;
    return 1;
}


/** Write the header of the output file container. */
int write_output_file_header(AVFormatContext *output_format_context)
{
    int error;
    if ((error = avformat_write_header(output_format_context, NULL)) < 0) {
        cout << "Could not write output file header " << error << endl;
        return error;
    }
    return 0;
}
