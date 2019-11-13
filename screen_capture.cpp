#include "screen_capture.h"

screen_capture::screen_capture(int width, int height, std::string& capture_params) {
	
	avdevice_register_all();
	this->width = width;
	this->height = height;
	this->capture_params = capture_params;
}

screen_capture::~screen_capture() {

}

void screen_capture::init_capture() {
	input_format = av_find_input_format("x11grab");
	format_context = avformat_alloc_context();
	AVDictionary* dict = NULL;
	std::string video_size = std::string(std::to_string(width));
	video_size += "x";
	video_size += std::to_string(height);
	av_dict_set(&dict, "video_size", video_size.c_str(), 0);
	int result = 0;
	result = avformat_open_input(&format_context, ":0.0", input_format, &dict);
       
	if(result < 0) {
		printf("Failed to open input!\n");
	}
	
	
	dict = nullptr;

	int stream_index = -1;
	stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if(stream_index < 0) {
		printf("No such videostream!\n");
	}
	AVStream* stream = format_context->streams[stream_index];
	codec = avcodec_find_decoder(stream->codecpar->codec_id);
	if(!codec) {
		printf("No such decoder!\n");
	}
	codec_context = avcodec_alloc_context3(codec);
	if(!codec_context) {
		printf("Failed to allocate codec context!\n");
	}
	codec_context->pix_fmt = (AVPixelFormat)stream->codecpar->format;
	codec_context->width = stream->codecpar->width;
	codec_context->height = stream->codecpar->height;

	result = avcodec_open2(codec_context, codec, NULL);
       	
	printf("Pix_fmt: %d\n", codec_context->pix_fmt);
	if(result < 0) {
		printf("Failed to open codec!\n");
	}



}


int screen_capture::convert_frame(AVFrame* frame, AVFrame* out_frame) {
	int res = 0;
	SwsContext* converter_context = NULL;
	converter_context = sws_getCachedContext(converter_context, frame->width, frame->height, codec_context->pix_fmt,
			frame->width, frame->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	if(!converter_context) {
		printf("Failed to allocate converter context!\n");
		return -1;
	}
	sws_scale(converter_context, frame->data, frame->linesize, 0, frame->height, out_frame->data, out_frame->linesize);
	av_frame_unref(frame);
	if(res < 0){
		return res;
	}
	if(out_frame == NULL){
		printf("Failed to convert, cleaning up!\n");
		return -2;
	}
	sws_freeContext(converter_context);
	return 0;
}

void screen_capture::start_capture() {
	AVFrame* frame = av_frame_alloc();
       	AVPacket* packet = av_packet_alloc();
	AVFrame* rgb_frame = av_frame_alloc();
	int result = 0;
	while(av_read_frame(format_context, packet) > -1) {
		result = avcodec_send_packet(codec_context, packet);
		if(result == AVERROR(EAGAIN)) {
			avcodec_receive_frame(codec_context, frame);
			continue;
		}else if(result == AVERROR_EOF) {
			printf("EOF!\n");
			continue;
		}else if(result != 0){
			printf("Codec failure!\n");
			continue;
		}
		av_packet_unref(packet);

		result = avcodec_receive_frame(codec_context, frame);
		if(result == AVERROR(EAGAIN)) {
			continue;
		}else if(result == AVERROR_EOF) {
			printf("EOF!\n");
			continue;
		}else if(result != 0){
			printf("Codec failure!\n");
			continue;
		}

		av_image_alloc(rgb_frame->data, rgb_frame->linesize, codec_context->width, codec_context->height, AV_PIX_FMT_RGB24, 1);
	

		if(convert_frame(frame, rgb_frame) < 0) {
			printf("Failed to convert frame!\n");
			continue;
		}
		av_freep(&rgb_frame->data[0]);
	
		
	
	}	
}

