/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
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
 * @file pcm.h
 * raw PCM decoder header
 */

#ifndef PCM_H
#define PCM_H

#include "common.h"

#include <stdio.h>

#include "byteio.h"

/* "whence" values for seek functions */
#define PCM_SEEK_SET 0
#define PCM_SEEK_CUR 1
#define PCM_SEEK_END 2

/* maximum single read size: 5 sec at 48kHz */
#define PCM_MAX_READ 240000

/* supported TWOCC WAVE formats */
#define WAVE_FORMAT_PCM         0x0001
#define WAVE_FORMAT_IEEEFLOAT   0x0003
#define WAVE_FORMAT_EXTENSIBLE  0xFFFE

/* raw audio sample types */
enum PcmSampleType {
    PCM_SAMPLE_TYPE_INT = 0,
    PCM_SAMPLE_TYPE_FLOAT
};

/* supported raw audio sample formats */
enum PcmSampleFormat {
    PCM_SAMPLE_FMT_UNKNOWN = -1,
    PCM_SAMPLE_FMT_U8 = 0,
    PCM_SAMPLE_FMT_S16,
    PCM_SAMPLE_FMT_S20,
    PCM_SAMPLE_FMT_S24,
    PCM_SAMPLE_FMT_S32,
    PCM_SAMPLE_FMT_FLT,
    PCM_SAMPLE_FMT_DBL
};

/* supported file formats */
enum PcmFileFormat {
    PCM_FORMAT_UNKNOWN = -1,
    PCM_FORMAT_RAW     =  0,
    PCM_FORMAT_WAVE    =  1
};

/* byte orders */
enum PcmByteOrder {
    PCM_BYTE_ORDER_LE = 0,
    PCM_BYTE_ORDER_BE = 1
};

/* main decoder context */
typedef struct PcmFile {
    /** Format conversion function */
    void (*fmt_convert)(void *dest_v, void *src_v, int n);

    ByteIOContext io;       ///< input buffer
    uint64_t filepos;       ///< current file position
    int seekable;           ///< indicates if input stream is seekable
    int read_to_eof;        ///< indicates that data is to be read until EOF
    uint64_t file_size;     ///< total file size, if known
    uint64_t data_start;    ///< byte position for start of data
    uint64_t data_size;     ///< data size, in bytes
    uint64_t samples;       ///< total number of audio samples

    int sample_type;        ///< sample type (integer or floating-point)
    int file_format;        ///< file format (raw, wav, etc...)
    int order;              ///< sample byte order
    int channels;           ///< number of channels
    uint32_t ch_mask;       ///< channel mask, indicates speaker locations
    int sample_rate;        ///< audio sampling frequency
    int block_align;        ///< bytes in each sample, for all channels
    int bit_width;          ///< bits-per-sample

    int source_format;      ///< sample type in the input file
    int read_format;        ///< sample type to convert to when reading

    int wav_format;         ///< WAVE wFormatTag
} PcmFile;

/**
 * Initializes PcmFile structure using the given input file pointer.
 * Examines the header (if present) to get audio information and has the file
 * pointer aligned at start of data when it exits.
 * Returns non-zero value if an error occurs.
 */
extern int pcmfile_init(PcmFile *pf, FILE *fp, int read_format, int file_format);

/**
 * Frees memory from internal buffer.
 */
extern void pcmfile_close(PcmFile *pf);

/**
 * Sets the source sample format
 */
extern void pcmfile_set_source(PcmFile *pf, int fmt, int order);

/**
 * Reads audio samples to the output buffer.
 * Output is channel-interleaved, native byte order.
 * Only up to PCM_MAX_READ samples can be read in one call.
 * The output sample format depends on the value of pf->read_format.
 * Returns number of samples read or -1 on error.
 */
extern int pcmfile_read_samples(PcmFile *pf, void *buffer, int num_samples);

/**
 * Seeks to byte offset within file.
 * Limits the seek position or offset to signed 32-bit.
 * It also does slower forward seeking for streaming input.
 */
extern int pcmfile_seek_set(PcmFile *pf, uint64_t dest);

/**
 * Seeks to sample offset.
 * Syntax works like fseek. use PCM_SEEK_SET, PCM_SEEK_CUR, or PCM_SEEK_END
 * for the whence value.  Returns -1 on error, 0 otherwise.
 */
extern int pcmfile_seek_samples(PcmFile *pf, int64_t offset, int whence);

/**
 * Seeks to time offset, in milliseconds, based on the audio sample rate.
 * Syntax works like fseek. use PCM_SEEK_SET, PCM_SEEK_CUR, or PCM_SEEK_END
 * for the whence value.  Returns -1 on error, 0 otherwise.
 */
extern int pcmfile_seek_time_ms(PcmFile *pf, int64_t offset, int whence);

/**
 * Returns the current stream position, in samples.
 * Returns -1 on error.
 */
extern uint64_t pcmfile_position(PcmFile *pf);

/**
 * Returns the current stream position, in milliseconds.
 * Returns -1 on error.
 */
extern uint64_t pcmfile_position_time_ms(PcmFile *pf);

/**
 * Prints out a description of the pcm format to the specified
 * output stream.
 */
extern void pcmfile_print(PcmFile *pf, FILE *st);


/**
 * File format functions
 */

extern int pcmfile_probe_raw(uint8_t *data, int size);
extern int pcmfile_init_raw(PcmFile *pf);

extern int pcmfile_probe_wave(uint8_t *data, int size);
extern int pcmfile_init_wave(PcmFile *pf);

#endif /* PCM_H */
