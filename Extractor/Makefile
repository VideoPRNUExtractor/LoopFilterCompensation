# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \


CFLAGS += -Wall -g
CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS) ) $(CFLAGS)
LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS) ) $(LDLIBS)

EXAMPLES=       extract_mvs                        \


OBJS=$(addsuffix .o,$(EXAMPLES))

# the following examples make explicit use of the math library
avcodec:           LDLIBS += -lm
decoding_encoding: LDLIBS += -lm
muxing:            LDLIBS += -lm
resampling_audio:  LDLIBS += -lm

.phony: all clean-test clean

ffmpegMake: 
	$(MAKE) -C /home/lab/Desktop/ws/c/git/FFmpeg

all: ffmpegMake $(OBJS) $(EXAMPLES)

clean-test:
	$(RM) test*.pgm test.h264 test.mp2 test.sw test.mpg

clean: clean-test
	$(RM) $(EXAMPLES) $(OBJS)
