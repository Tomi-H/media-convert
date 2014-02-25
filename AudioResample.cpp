#include "AudioResample.h"
#include <assert.h>

AudioResample::AudioResample()
{
    mSwrCtx = NULL;
}

AudioResample::~AudioResample()
{
}

int AudioResample::init(uint64_t in_ch_layout, AVSampleFormat in_sample_fmt, int in_sample_rate,
                        uint64_t out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate)
{
    mInChannelLayout = in_ch_layout;
    mInSampleFmt = in_sample_fmt;
    mInSampleRate = in_sample_rate;
    mOutChannellayout = out_ch_layout;
    mOutSampleFmt = out_sample_fmt;
    mOutSampleRate = out_sample_rate;

    return 0;
}

int AudioResample::resample(uint8_t ** out, int out_count, const uint8_t ** in, int in_count)
{
    if (!mSwrCtx)
        mSwrCtx = swr_alloc_set_opts(mSwrCtx, mInChannelLayout, mInSampleFmt, mInSampleRate,
                                     mOutChannellayout, mOutSampleFmt, mOutSampleRate, 0, NULL);
    return swr_convert(mSwrCtx, out, out_count, in, in_count);
}

int64_t AudioResample::getDalay()
{
    assert(mSwrCtx);
    return swr_get_delay(mSwrCtx, mOutSampleRate);
}

void AudioResample::release()
{
    swr_free(&mSwrCtx);
}
