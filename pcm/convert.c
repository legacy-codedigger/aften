/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file convert.c
 * Raw audio sample format conversion
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pcm.h"

static void
fmt_convert_u8_to_u8(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(uint8_t));
}

static void
fmt_convert_s16_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 8) + 128;
}

static void
fmt_convert_s20_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 12) + 128;
}

static void
fmt_convert_s24_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 16) + 128;
}

static void
fmt_convert_s32_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 24) + 128;
}

static void
fmt_convert_float_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (uint8_t)CLIP(((src[i] * 128) + 128), 0, 255);
}

static void
fmt_convert_double_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (uint8_t)CLIP(((src[i] * 128) + 128), 0, 255);
}

static void
fmt_convert_u8_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - 128) << 8;
}

static void
fmt_convert_s16_to_s16(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int16_t));
}

static void
fmt_convert_s20_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 4);
}

static void
fmt_convert_s24_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 8);
}

static void
fmt_convert_s32_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 16);
}

static void
fmt_convert_float_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int16_t)CLIP((src[i] * 32768), -32768, 32767);
}

static void
fmt_convert_double_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int16_t)CLIP((src[i] * 32768), -32768, 32767);
}

static void
fmt_convert_u8_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - 128) << 12;
}

static void
fmt_convert_s16_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 4);
}

static void
fmt_convert_s20_to_s20(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_s24_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 4);
}

static void
fmt_convert_s32_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 12);
}

static void
fmt_convert_float_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 524288), -524288, 524287);
}

static void
fmt_convert_double_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 524288), -524288, 524287);
}

static void
fmt_convert_u8_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - 128) << 16;
}

static void
fmt_convert_s16_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 8);
}

static void
fmt_convert_s20_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 4);
}

static void
fmt_convert_s24_to_s24(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_s32_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] >> 8);
}

static void
fmt_convert_float_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 8388608), -8388608, 8388607);
}

static void
fmt_convert_double_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 8388608), -8388608, 8388607);
}

static void
fmt_convert_u8_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - 128) << 24;
}

static void
fmt_convert_s16_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 16);
}

static void
fmt_convert_s20_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 12);
}

static void
fmt_convert_s24_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] << 8);
}

static void
fmt_convert_s32_to_s32(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_float_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)(src[i] * 2147483648LL);
}

static void
fmt_convert_double_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (int32_t)(src[i] * 2147483648LL);
}

static void
fmt_convert_u8_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - FCONST(128.0)) / FCONST(128.0);
}

static void
fmt_convert_s16_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(32768.0);
}

static void
fmt_convert_s20_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(524288.0);
}

static void
fmt_convert_s24_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(8388608.0);
}

static void
fmt_convert_s32_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(2147483648.0);
}

static void
fmt_convert_float_to_float(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(float));
}

static void
fmt_convert_double_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    double *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (float)src[i];
}

static void
fmt_convert_u8_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = (src[i] - FCONST(128.0)) / FCONST(128.0);
}

static void
fmt_convert_s16_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(32768.0);
}

static void
fmt_convert_s20_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(524288.0);
}

static void
fmt_convert_s24_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(8388608.0);
}

static void
fmt_convert_s32_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i] / FCONST(2147483648.0);
}

static void
fmt_convert_float_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    float *src = src_v;
    int i;

    for(i=0; i<n; i++)
        dest[i] = src[i];
}

static void
fmt_convert_double_to_double(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(double));
}

static void
set_fmt_convert_from_u8(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_u8_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_u8_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_u8_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_u8_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_u8_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_u8_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_u8_to_double;
}

static void
set_fmt_convert_from_s16(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_s16_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_s16_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_s16_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_s16_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_s16_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_s16_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_s16_to_double;
}

static void
set_fmt_convert_from_s20(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_s20_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_s20_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_s20_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_s20_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_s20_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_s20_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_s20_to_double;
}

static void
set_fmt_convert_from_s24(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_s24_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_s24_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_s24_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_s24_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_s24_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_s24_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_s24_to_double;
}

static void
set_fmt_convert_from_s32(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_s32_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_s32_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_s32_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_s32_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_s32_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_s32_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_s32_to_double;
}

static void
set_fmt_convert_from_float(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_float_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_float_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_float_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_float_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_float_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_float_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_float_to_double;
}

static void
set_fmt_convert_from_double(PcmFile *pf)
{
    enum PcmSampleFormat fmt = pf->read_format;

    if(fmt == PCM_SAMPLE_FMT_U8)
        pf->fmt_convert = fmt_convert_double_to_u8;
    else if(fmt == PCM_SAMPLE_FMT_S16)
        pf->fmt_convert = fmt_convert_double_to_s16;
    else if(fmt == PCM_SAMPLE_FMT_S20)
        pf->fmt_convert = fmt_convert_double_to_s20;
    else if(fmt == PCM_SAMPLE_FMT_S24)
        pf->fmt_convert = fmt_convert_double_to_s24;
    else if(fmt == PCM_SAMPLE_FMT_S32)
        pf->fmt_convert = fmt_convert_double_to_s32;
    else if(fmt == PCM_SAMPLE_FMT_FLT)
        pf->fmt_convert = fmt_convert_double_to_float;
    else if(fmt == PCM_SAMPLE_FMT_DBL)
        pf->fmt_convert = fmt_convert_double_to_double;
}

void
pcmfile_set_source(PcmFile *pf, int fmt, int order)
{
    pf->source_format = fmt;
    pf->order = order;
    switch(fmt) {
        case PCM_SAMPLE_FMT_UNKNOWN:
            break;
        case PCM_SAMPLE_FMT_U8:
            set_fmt_convert_from_u8(pf);
            pf->bit_width = 8;
            break;
        case PCM_SAMPLE_FMT_S16:
            set_fmt_convert_from_s16(pf);
            pf->bit_width = 16;
            break;
        case PCM_SAMPLE_FMT_S20:
            set_fmt_convert_from_s20(pf);
            pf->bit_width = 20;
            break;
        case PCM_SAMPLE_FMT_S24:
            set_fmt_convert_from_s24(pf);
            pf->bit_width = 24;
            break;
        case PCM_SAMPLE_FMT_S32:
            set_fmt_convert_from_s32(pf);
            pf->bit_width = 32;
            break;
        case PCM_SAMPLE_FMT_FLT:
            set_fmt_convert_from_float(pf);
            pf->bit_width = 32;
            break;
        case PCM_SAMPLE_FMT_DBL:
            set_fmt_convert_from_double(pf);
            pf->bit_width = 64;
            break;
    }
    if(pf->file_format != PCM_FORMAT_WAVE || pf->wav_format == WAVE_FORMAT_PCM ||
            pf->wav_format == WAVE_FORMAT_IEEEFLOAT) {
        pf->block_align = MAX(1, ((pf->bit_width + 7) >> 3) * pf->channels);
        pf->samples = (pf->data_size / pf->block_align);
    }
}
