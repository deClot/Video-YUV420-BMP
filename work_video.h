#ifndef WORK_VIDEO
#define WORK_VIDEO

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

int open_input_video (char* video_name, AVFormatContext** format_context,
                      AVCodecContext** codec_context, int* video_stream);

int open_output_video(char* filename, AVCodecContext *input_codec_context,
                     AVFormatContext **output_format_context,
                     AVCodecContext **output_codec_context);

int write_output_file_header(AVFormatContext *output_format_context);

int encode_frame(AVCodecContext *codec_context_out, AVFrame *frame_out, AVPacket *packet);

#endif
