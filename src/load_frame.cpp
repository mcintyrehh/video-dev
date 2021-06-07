extern "C" {
#include <libavcodec/avcodec.h>
#include<libavformat/avformat.h>
#include <inttypes.h>
}

/* Returns false if it can't load a frame
 * Give it a file name and it returns width, height and data for image data
 * */
bool load_frame(const char* filename, int* width, int* height, unsigned char** data) {
  // Open the file using libavformat
  AVFormatContext* av_format_ctx = avformat_alloc_context();
  if(!av_format_ctx) {
    printf("Couldn't create AVFormatContext\n");
    return false;
  }
  if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
    printf("Couldn't open video file\n");
    //don't need to worry about deallocating the ctx resource yet because it will crash
    //the program anyway
    return false;
  };
  // Find the first valid video steaam inside the file
  int video_stream_index = -1;
  AVCodecParameters* av_codec_params;
  AVCodec* av_codec;
  for (int i=0; i < av_format_ctx->nb_streams; ++i) {
    auto stream = av_format_ctx->streams[i];
    //get codex parameters from the stream
    av_codec_params = av_format_ctx->streams[i]->codecpar;
    //for that codec we need to find the decoder
    //after unpacking the format with libavformat, now we are unpacking the format
    //with libavcodec within the file
    //now we are looking to see if we have the decoder to decode the file
    av_codec = avcodec_find_decoder(av_codec_params->codec_id);
    if(!av_codec) {
      continue;
    }
    if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
      break;
    }
  }

  if(video_stream_index == -1) {
    printf("Couldn't find valid video stream inside file\n");
    return false;
  }
  // Set up a codec context for the decoder
  // Need to allocate space for the codec to store its internal data structures for decoding
  AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
  if(!av_codec_ctx) {
    printf("Couldn't create AVCodecContext\n");
    return false;
  }
  //The info we got out of avformat needs to be set into the codec context as its initial state
  if(!avcodec_parameters_to_context(av_codec_ctx, av_codec_params)){
    printf("Couldn't initialize AVCodecContext\n");
    return false;
  }
  if(!avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    printf("Couldn't open codec\n");
    return false;
  }
  // Going to get AVPackets(compressed data https://ffmpeg.org/doxygen/3.1/structAVPacket.html) out of the file format
  // Then we put those packets in to the decoder (codec) which will give us the AVFrame( raw audio or video data https://ffmpeg.org/doxygen/trunk/structAVFrame.html)
  AVFrame* av_frame = av_frame_alloc();
  if(!av_frame) {
    printf("Couldn't allocate AVFrame\n");
    return false;
  }
  AVPacket* av_packet = av_packet_alloc();
  if(!av_packet) {
    printf("Couldn't allocate AVFrame\n");
    return false;
  }
  //from the context, we want to read in a packet
  //returns a number, if the number is >0, there is a packet that we have read
  while(av_read_frame(av_format_ctx, av_packet) >=0) {
    //if its not the video stream, we don't care about it
    if(av_packet->stream_index != video_stream_index) {
      continue;
    }
    //if it is our video stream, send it to the decoder
    int response = avcodec_send_packet(av_codec_ctx, av_packet);
    if (response <0) {
      printf("Failed to decode packet: %s\n", av_err2str(response));
      return false;
    }
    //want the decoder to send us a frame
    response = avcodec_receive_frame(av_codec_ctx, av_frame);
    if(response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      continue;
    }
  }
  avformat_close_input(&av_format_ctx);
  avformat_free_context(av_format_ctx);
  avcodec_free_context(&av_codec_ctx);

  return false;
};
