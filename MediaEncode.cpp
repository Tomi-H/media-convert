/*
 * MediaEncode.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#include "MediaEncode.h"
#include <assert.h>

MediaEncode::MediaEncode()
{
    mCodecCtx = NULL;
    mCodec = NULL;
    mBitRate = 0;
}

MediaEncode::~MediaEncode()
{

}

int MediaEncode::isOpen()
{
    if (mCodec)
        return avcodec_is_open(mCodecCtx);
    return 0;
}

uint64_t MediaEncode::checkChannelLayout(uint64_t channel_layout)
{

    uint64_t layout;
    const uint64_t * p = NULL;
    int nb_channels = 0;
    int channels = 0;

    assert(mCodec);
    if (!mCodec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = mCodec->channel_layouts;
    while (*p) {
        if (*p == channel_layout)
            return *p;
        channels = av_get_channel_layout_nb_channels(*p);
        if (channels >nb_channels) {
            nb_channels = channels;
            layout = *p;
        }
        p++;
    }
    return layout;
}

uint64_t MediaEncode::defaultChannelLayout()
{
    assert(mCodec);
    return *mCodec->channel_layouts;
}

int MediaEncode::defaultSampleRate()
{
    return 44100;
}

AVSampleFormat MediaEncode::defaultSampleFmt()
{
    assert(mCodec);
    return *mCodec->sample_fmts;
}

AVSampleFormat MediaEncode::checkSampleFmt(AVSampleFormat sample_fmt)
{
    assert(mCodec);
    const AVSampleFormat * p = mCodec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return *p;
        p++;
    }

    if (*p == AV_SAMPLE_FMT_NONE)
        return defaultSampleFmt();

    return AV_SAMPLE_FMT_NONE;
}

void MediaEncode::setBitRate(int bit_rate)
{
    mBitRate = bit_rate;
}

int MediaEncode::getBitRate()
{
    return mBitRate;
}
