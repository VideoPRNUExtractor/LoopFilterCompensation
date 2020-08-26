/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_MOTION_VECTOR_H
#define AVUTIL_MOTION_VECTOR_H

#include <stdint.h>
#include "libavutil/mem.h"

typedef struct AVMotionVector {
    /**
     * Where the current macroblock comes from; negative value when it comes
     * from the past, positive value when it comes from the future.
     * XXX: set exact relative ref frame reference instead of a +/- 1 "direction".
     */
    int32_t source;
    /**
     * Width and height of the block.
     */
    uint8_t w, h;
    /**
     * Absolute source position. Can be outside the frame area.
     */
    int16_t src_x, src_y;
    /**
     * Absolute destination position. Can be outside the frame area.
     */
    int16_t dst_x, dst_y;
    /**
     * Extra flag information.
     * Currently unused.
     */
    uint64_t flags;
    /**
     * Motion vector
     * src_x = dst_x + motion_x / motion_scale
     * src_y = dst_y + motion_y / motion_scale
     */
    int32_t motion_x, motion_y;
    uint16_t motion_scale;
} AVMotionVector;

typedef struct KSM_AVVideoInfo {
	int codec_id;
	char codec_name_short[128];
	char codec_name_long[1024];
	int gop_size;
	int aspect_ratio_num;
	int aspect_ratio_den;

} KSM_AVVideoInfo;

typedef struct KSM_AVMacroBlockInfo {
	uint32_t macroblock_no;
	uint32_t mb_x;
	uint32_t mb_y;
	uint32_t mb_type;
	int mb_qp;
	AVMotionVector MV;
} KSM_AVMacroBlockInfo;

typedef struct KSM_AVFrameInfo {
	uint32_t coded_picture_number;
	int frame_number;
	uint32_t pict_type;
	uint32_t width;
	uint32_t height;
	int min_cb_width;
	int min_cb_height;
	int min_tb_width;
	int min_tb_height;
	int min_pu_width;
	int min_pu_height;
	int min_cb_size;
	int min_pu_size;
	int min_tb_size;
	int bit_depth;
	int min_block_size;
	int min_block_width;
	int min_block_height;
} KSM_AVFrameInfo;

typedef struct KSM_Mv {
    int16_t x;  ///< horizontal component of motion vector
    int16_t y;  ///< vertical component of motion vector
} KSM_Mv;

typedef struct KSM_MvField {
    DECLARE_ALIGNED(4, KSM_Mv, mv)[2];
    int8_t ref_idx[2];
    int8_t pred_flag;
} KSM_MvField;

typedef struct KSM_AV_HEVC_PU_Info {
	KSM_MvField mvf;
} KSM_AV_HEVC_PU_Info ;

typedef struct KSM_AV_HEVC_CU_Info {
	int pred_mode;
	int part_mode;
	int intra_split_flag;
	int x;
	int y;
	int cb_size;
	int mb_qp;
} KSM_AV_HEVC_CU_Info ;

#endif /* AVUTIL_MOTION_VECTOR_H */
