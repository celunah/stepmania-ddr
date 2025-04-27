/*
<<<<<<< HEAD:extern/ffmpeg-w32/include/libavcodec/ac3_parser.h
 * AC-3 parser prototypes
 * Copyright (c) 2003 Fabrice Bellard
 * Copyright (c) 2003 Michael Niedermayer
=======
<<<<<<<< HEAD:extern/ffmpeg/libavfilter/x86/vf_maskedmerge_init.c
 * Copyright (c) 2015 Paul B Mahol
========
 * AC-3 parser prototypes
 * Copyright (c) 2003 Fabrice Bellard
 * Copyright (c) 2003 Michael Niedermayer
>>>>>>>> 5_1-new:extern/ffmpeg-w32/include/libavcodec/ac3_parser.h
>>>>>>> 5_1-new:extern/ffmpeg/include/libavcodec/ac3_parser.h
 *
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

<<<<<<< HEAD:extern/ffmpeg-w32/include/libavcodec/ac3_parser.h
=======
<<<<<<<< HEAD:extern/ffmpeg/libavfilter/x86/vf_maskedmerge_init.c
#include "libavutil/attributes.h"
#include "libavutil/cpu.h"
#include "libavutil/x86/cpu.h"
#include "libavfilter/maskedmerge.h"

void ff_maskedmerge8_sse2(const uint8_t *bsrc, const uint8_t *osrc,
                          const uint8_t *msrc, uint8_t *dst,
                          ptrdiff_t blinesize, ptrdiff_t olinesize,
                          ptrdiff_t mlinesize, ptrdiff_t dlinesize,
                          int w, int h,
                          int half, int shift);

av_cold void ff_maskedmerge_init_x86(MaskedMergeContext *s)
{
    int cpu_flags = av_get_cpu_flags();

    if (EXTERNAL_SSE2(cpu_flags) && s->depth == 8) {
        s->maskedmerge = ff_maskedmerge8_sse2;
    }
}
========
>>>>>>> 5_1-new:extern/ffmpeg/include/libavcodec/ac3_parser.h
#ifndef AVCODEC_AC3_PARSER_H
#define AVCODEC_AC3_PARSER_H

#include <stddef.h>
#include <stdint.h>

/**
 * Extract the bitstream ID and the frame size from AC-3 data.
 */
int av_ac3_parse_header(const uint8_t *buf, size_t size,
                        uint8_t *bitstream_id, uint16_t *frame_size);


#endif /* AVCODEC_AC3_PARSER_H */
<<<<<<< HEAD:extern/ffmpeg-w32/include/libavcodec/ac3_parser.h
=======
>>>>>>>> 5_1-new:extern/ffmpeg-w32/include/libavcodec/ac3_parser.h
>>>>>>> 5_1-new:extern/ffmpeg/include/libavcodec/ac3_parser.h
