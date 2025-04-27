/*
 * Version macros.
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

#ifndef AVFORMAT_VERSION_MAJOR_H
#define AVFORMAT_VERSION_MAJOR_H

/**
 * @file
 * @ingroup libavf
 * Libavformat version macros
 */

<<<<<<<< HEAD:extern/ffmpeg/libavdevice/version.h
#include "libavutil/version.h"

#define LIBAVDEVICE_VERSION_MAJOR  59
#define LIBAVDEVICE_VERSION_MINOR   4
#define LIBAVDEVICE_VERSION_MICRO 100

#define LIBAVDEVICE_VERSION_INT AV_VERSION_INT(LIBAVDEVICE_VERSION_MAJOR, \
                                               LIBAVDEVICE_VERSION_MINOR, \
                                               LIBAVDEVICE_VERSION_MICRO)
#define LIBAVDEVICE_VERSION     AV_VERSION(LIBAVDEVICE_VERSION_MAJOR, \
                                           LIBAVDEVICE_VERSION_MINOR, \
                                           LIBAVDEVICE_VERSION_MICRO)
#define LIBAVDEVICE_BUILD       LIBAVDEVICE_VERSION_INT

#define LIBAVDEVICE_IDENT       "Lavd" AV_STRINGIFY(LIBAVDEVICE_VERSION)
========
// Major bumping may affect Ticket5467, 5421, 5451(compatibility with Chromium)
// Also please add any ticket numbers that you believe might be affected here
#define LIBAVFORMAT_VERSION_MAJOR  59
>>>>>>>> 5_1-new:extern/ffmpeg/libavformat/version_major.h

/**
 * FF_API_* defines may be placed below to indicate public API that will be
 * dropped at a future version bump. The defines themselves are not part of
 * the public API and may change, break or disappear at any time.
 *
 * @note, when bumping the major version it is recommended to manually
 * disable each FF_API_* in its own commit instead of disabling them all
 * at once through the bump. This improves the git bisect-ability of the change.
 *
 */
<<<<<<<< HEAD:extern/ffmpeg/libavdevice/version.h
#define FF_API_DEVICE_CAPABILITIES (LIBAVDEVICE_VERSION_MAJOR < 60)
========
#define FF_API_LAVF_PRIV_OPT            (LIBAVFORMAT_VERSION_MAJOR < 60)
#define FF_API_COMPUTE_PKT_FIELDS2      (LIBAVFORMAT_VERSION_MAJOR < 60)
#define FF_API_AVIOCONTEXT_WRITTEN      (LIBAVFORMAT_VERSION_MAJOR < 60)
#define FF_HLS_TS_OPTIONS               (LIBAVFORMAT_VERSION_MAJOR < 60)
#define FF_API_AVSTREAM_CLASS           (LIBAVFORMAT_VERSION_MAJOR > 59)
>>>>>>>> 5_1-new:extern/ffmpeg/libavformat/version_major.h


#define FF_API_R_FRAME_RATE            1

#endif /* AVFORMAT_VERSION_MAJOR_H */
