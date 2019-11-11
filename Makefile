CC=g++
CARGS= -pthread --std=c++17 -lavcodec -lavformat -lavutil -lavdevice
FILES=screen_capture.h screen_capture.cpp main.cpp
OUT_NAME=linux_ambilight

build:
	$(CC) $(FILES) $(CARGS) -o $(OUT_NAME)

clean:
	rm -f $(BUILD_DIRECTORY)
