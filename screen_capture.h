#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

class screen_capture {
	private:
		AVInputFormat* input_format;
		AVFormatContext* format_context;
		AVCodecContext* codec_context;
		AVCodec* codec;
		int width, height;
		std::string capture_params;
		int convert_frame(AVFrame* frame, AVFrame* out_frame);
		
	public:
		screen_capture(int width, int height, std::string& capture_params);
		~screen_capture();
		void init_capture();
		void start_capture();

};
