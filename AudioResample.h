/*
 * AudioResample.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef AUDIORESAMPLE_H_
#define AUDIORESAMPLE_H_
#include "ffmpeg.h"

class AudioResample
{
    public:
        AudioResample();
        virtual ~AudioResample();
        int init(uint64_t in_ch_layout, AVSampleFormat in_sample_fmt, int in_sample_rate,
                 uint64_t out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate);
        int resample(uint8_t ** out, int out_count, const uint8_t ** in, int in_count);
        int64_t getDalay();
        void release();

    private:
        struct SwrContext * mSwrCtx;
        uint64_t mInChannelLayout;
        uint64_t mOutChannellayout;
        AVSampleFormat mInSampleFmt;
        AVSampleFormat mOutSampleFmt;
        int mInSampleRate;
        int mOutSampleRate;
};

#endif /* AUDIORESAMPLE_H_ */
