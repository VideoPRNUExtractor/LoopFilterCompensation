/*
 * Copyright (c) 2012 Stefano Sabatini
 * Copyright (c) 2014 Clément Bœsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <libavutil/motion_vector.h>
#include <libavformat/avformat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static AVStream *video_stream = NULL;
static char src_filename[256];

static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static int video_frame_count = 0;


int WriteJPEG(AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo) {
	AVCodecContext *pOCodecCtx;
	AVCodec *pOCodec;
	uint8_t *Buffer;
	int BufSiz;
	int BufSizActual;
	int ImgFmt = AV_PIX_FMT_YUVJ420P; //for thenewer ffmpeg version, this int to pixelformat
	FILE *JPEGFile;
	char JPEGFName[256];

	BufSiz = avpicture_get_size(ImgFmt, pCodecCtx->width, pCodecCtx->height);

	Buffer = (uint8_t *) malloc(BufSiz * 10);
	if (Buffer == NULL)
		return (0);
	memset(Buffer, 0, BufSiz);

	pOCodecCtx = avcodec_alloc_context3(AVMEDIA_TYPE_VIDEO);
	if (!pOCodecCtx) {
		free(Buffer);
		return (0);
	}

	pOCodecCtx->bit_rate = pCodecCtx->bit_rate;
	pOCodecCtx->width = pCodecCtx->width;
	pOCodecCtx->height = pCodecCtx->height;
	pOCodecCtx->pix_fmt = ImgFmt;
	pOCodecCtx->codec_id = AV_CODEC_ID_MJPEG;
	pOCodecCtx->codec_type = pCodecCtx->codec_type; //AVMEDIA_TYPE_VIDEO;
	pOCodecCtx->time_base.num = pCodecCtx->time_base.num;
	pOCodecCtx->time_base.den = pCodecCtx->time_base.den;

	//pOCodecCtx->bit_rate_tolerance=452678328;

	pOCodec = avcodec_find_encoder(pOCodecCtx->codec_id);
	if (!pOCodec) {
		free(Buffer);
		return (0);
	}
	if (avcodec_open2(pOCodecCtx, pOCodec, NULL) < 0) {
		free(Buffer);
		return (0);
	}
	pOCodecCtx->qmin = 1;
	pOCodecCtx->qmax = 1;
	pOCodecCtx->mb_lmin = pOCodecCtx->lmin = pOCodecCtx->qmin * FF_QP2LAMBDA;
	pOCodecCtx->mb_lmax = pOCodecCtx->lmax = pOCodecCtx->qmax * FF_QP2LAMBDA;
	pOCodecCtx->flags = CODEC_FLAG_QSCALE;
	pOCodecCtx->global_quality = pOCodecCtx->qmin * FF_QP2LAMBDA;

	pFrame->pts = 1;
	pFrame->quality = pOCodecCtx->global_quality;

	AVPacket outPacket = { .data = NULL, .size = 0 };
	av_init_packet(&outPacket);
	int gotFram = 0;
	BufSizActual = avcodec_encode_video2(pOCodecCtx, &outPacket, pFrame,
			&gotFram);

	sprintf(JPEGFName, "%sFrame/frame-%04d.jpg", src_filename, FrameNo);
	JPEGFile = fopen(JPEGFName, "wb");
	fwrite(outPacket.data, 1, outPacket.size, JPEGFile);
	fclose(JPEGFile);

	avcodec_close(pOCodecCtx);
	free(Buffer);
	return (BufSizActual);
}

char* ReadFile(char *filename) {
	char *buffer = NULL;
	int string_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler) {
		// Seek the last byte of the file
		fseek(handler, 0, SEEK_END);
		// Offset from the first to the last byte, or in other words, filesize
		string_size = ftell(handler);
		// go back to the start of the file
		rewind(handler);

		// Allocate a string that can hold it all
		buffer = (char*) malloc(sizeof(char) * (string_size + 1));

		// Read it all in one operation
		read_size = fread(buffer, sizeof(char), string_size, handler);

		// fread doesn't set it so put a \0 in the last position
		// and buffer is now officially a string
		buffer[string_size] = '\0';

		if (string_size != read_size) {
			// Something went wrong, throw away the memory and set
			// the buffer to NULL
			free(buffer);
			buffer = NULL;
		}

		// Always remember to close the file.
		fclose(handler);
	}
	return buffer;
}
static int decode_packet(int *got_frame, int cached) {
	int decoded = pkt.size;
	*got_frame = 0;
	if (pkt.stream_index == video_stream_idx) {
		pid_t pid;

		pid = fork();
		if (pid == 0) {
			// 49 for skip loop filter
			video_dec_ctx->skip_loop_filter = 49;
			int ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame,
					&pkt);

			if (ret < 0) {
				fprintf(stderr, "Error decoding video frame (%s)\n",
						av_err2str(ret));
				return ret;
			}

			if (*got_frame) {

				fflush(stdout);
				WriteJPEG(video_dec_ctx, frame, video_frame_count);
				printf("Loop filter compensated frame %d was extracted. \n", video_frame_count);

			}
			exit(0);
		} else {
			wait(NULL);

			int ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame,
					&pkt);

			video_frame_count++;

		}

	}

	return decoded;
}

static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx,
		enum AVMediaType type) {
	int ret;
	AVStream *st;
	AVCodecContext *dec_ctx = NULL;
	AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not find %s stream in input file '%s'\n",
				av_get_media_type_string(type), src_filename);
		return ret;
	} else {
		*stream_idx = ret;
		st = fmt_ctx->streams[*stream_idx];

		/* find decoder for the stream */
		dec_ctx = st->codec;
		dec = avcodec_find_decoder(dec_ctx->codec_id);
		if (!dec) {
			fprintf(stderr, "Failed to find %s codec\n",
					av_get_media_type_string(type));
			return AVERROR(EINVAL);
		}

		/* Init the video decoder */
		av_dict_set(&opts, "flags2", "+export_mvs", 0);
		if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
			fprintf(stderr, "Failed to open %s codec\n",
					av_get_media_type_string(type));
			return ret;
		}
	}

	return 0;

}

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <video_path> \n", argv[0]);
		exit(1);
	}
	char *src_video = argv[1];
	int ret = 0, got_frame;
	video_frame_count = 0;

	char src_frame[256];
	sprintf(src_frame, "%sFrame", src_video);
	mkdir(src_frame, 0700);

	sprintf(src_filename, "%s", src_video);

	av_register_all();

	if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
		fprintf(stderr, "Could not open source file %s\n", src_filename);
		return -1;
	}

	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		exit(1);
	}

	if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO)
			>= 0) {
		video_stream = fmt_ctx->streams[video_stream_idx];
		video_dec_ctx = video_stream->codec;
	}

	av_dump_format(fmt_ctx, 0, src_filename, 0);

	if (!video_stream) {
		fprintf(stderr, "Could not find video stream in the input, aborting\n");
		ret = 1;
		goto end;
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate frame\n");
		ret = AVERROR(ENOMEM);
		goto end;
	}

	/* initialize packet, set data to NULL, let the demuxer fill it */
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	/* read frames from the file */
	while (av_read_frame(fmt_ctx, &pkt) >= 0) {
		AVPacket orig_pkt = pkt;
		do {
			ret = decode_packet(&got_frame, 0);
			if (ret < 0)
				break;
			pkt.data += ret;
			pkt.size -= ret;
		} while (pkt.size > 0);
		av_packet_unref(&orig_pkt);
	}

	/* flush cached frames */
	pkt.data = NULL;
	pkt.size = 0;

	do {
		decode_packet(&got_frame, 1);
	} while (got_frame);
	end: avcodec_close(video_dec_ctx);
	avformat_close_input(&fmt_ctx);
	av_frame_free(&frame);
	return ret < 0;
}
