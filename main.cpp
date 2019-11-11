#include "screen_capture.h"

int main() {
	std::string params(":0.0");

	screen_capture *capturer = new screen_capture(1680, 1050, params);
	capturer->init_capture();
	capturer->start_capture();
}
