/*
 * AudioEncode.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#include "AudioEncode.h"
#include <assert.h>

AudioEncode::AudioEncode()
{
    mCodecCtx = NULL;
    mCodec = NULL;
    mChannelLayout = 0;
    mSampleFmt = AV_SAMPLE_FMT_NONE;
    mSampleRate = 0;
    mBitRate = 0;
    mState = ENCODE_STATE_INIT;
}

AudioEncode::~AudioEncode()
{

}

int AudioEncode::open(AVCodecID codec_id, int flags)
{
    mCodec = avcodec_find_encoder(codec_id);
    if (!mCodec) {
        fprintf(stderr, "Couldn't find encoder: %s\n", avcodec_get_name(codec_id));
        return -1;
    }

    mCodecCtx = avcodec_alloc_context3(mCodec);
    if (!mCodecCtx) {
        fprintf(stderr, "allocate codec context failed.\n");
        return -1;
    }

    mFlags |= flags;
    mChannelNum = av_get_channel_layout_nb_channels(mChannelLayout);
    mMaxSamplesCount = mCodec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE ? 10000 :
                         mCodecCtx->frame_size;
    mFifo = av_audio_fifo_alloc(mSampleFmt, mChannelNum, mMaxSamplesCount);

    av_samples_alloc_array_and_samples(&mData, NULL, mChannelNum,
                                               mMaxSamplesCount, mSampleFmt, 0);
    return 0;
}

void AudioEncode::close()
{
    mState = ENCODE_STATE_CLOSE;
    if (mCodecCtx && avcodec_is_open(mCodecCtx)) {
        avcodec_close(mCodecCtx);
    }
    av_freep(&mData[0]);
    av_free(mCodecCtx);
    av_audio_fifo_free(mFifo);
}

int AudioEncode::encode(AVPacket * pkt, const uint8_t ** data, int nb_samples)
{
    if (mBitRate == 0)
        mBitRate = defaultBitRate();

    if (mChannelLayout == 0) {
        mChannelLayout = defaultChannelLayout();
    } else {
        mChannelLayout = checkChannelLayout(mChannelLayout);
    }

    if (mSampleFmt == AV_SAMPLE_FMT_NONE) {
        mSampleFmt = defaultSampleFmt();
    } else {
        mSampleFmt = checkSampleFmt(mSampleFmt);
    }

    if (mSampleRate == 0)
        mSampleRate = defaultSampleRate();

    mCodecCtx->bit_rate = mBitRate;
    mCodecCtx->channel_layout = mChannelLayout;
    mCodecCtx->sample_fmt = mSampleFmt;
    mCodecCtx->sample_rate = mSampleRate;
    mCodecCtx->flags |= mFlags;
    if (!avcodec_is_open(mCodecCtx)) {
        avcodec_open2(mCodecCtx, mCodec, NULL);
    }
    mState = ENCODE_STATE_RUN;

    return doEncode(pkt, data, nb_samples);
}

int AudioEncode::doEncode(AVPacket * pkt, const uint8_t ** data, int nb_samples)
{
    assert(pkt);
    assert(data);

    int ret = av_audio_fifo_write(mFifo, (void **)data, nb_samples);
    if (ret < 0) {
        fprintf(stderr, "write audio data into FIFO failed: %s\n", av_err2str(ret));
        exit(1);
    }

    int got_pkt = 0;
    while (av_audio_fifo_size(mFifo) > mMaxSamplesCount) {
        av_init_packet(pkt);
        int nb_samples = av_audio_fifo_read(mFifo, (void **)mData, mMaxSamplesCount);
        if (nb_samples < 0) {
            fprintf(stderr, "read audio data from FIFO failed: %s\n", av_err2str(nb_samples));
            exit(1);
        }

        if (!mFrame) {
            mFrame = avcodec_alloc_frame();
            mFrame->pts = 0;
        }
        mFrame->pts++;
        int bytes = av_samples_get_buffer_size(NULL,mChannelNum,nb_samples,mSampleFmt,0);
        avcodec_fill_audio_frame(mFrame,mChannelNum,mSampleFmt,mData[0],bytes,0);
        int ret = avcodec_encode_audio2(mCodecCtx, pkt, mFrame, &got_pkt);
        if (ret < 0) {
            fprintf(stderr, "encode audio data failed: %s\n", av_err2str(ret));
            exit(1);
        }

        if (!got_pkt) {
            avcodec_encode_audio2(mCodecCtx, pkt, NULL, &got_pkt);
            continue;
        }
    }

    return 0;
}

int AudioEncode::defaultBitRate()
{
    return 44100;
}

void AudioEncode::setChannelLayout(uint64_t channel_layout)
{
    if (mState == ENCODE_STATE_INIT)
        mChannelLayout = channel_layout;
}

void AudioEncode::setSampleFormat(AVSampleFormat sample_fmt)
{
    if (mState == ENCODE_STATE_INIT)
        mSampleFmt = sample_fmt;
}

void AudioEncode::setSampleRate(int sample_rate)
{
    if (mState == ENCODE_STATE_INIT)
        mSampleRate = sample_rate;
}

uint64_t AudioEncode::getChannelLayout()
{
    return mChannelLayout;
}

AVSampleFormat AudioEncode::getSampleFormat()
{
    return mSampleFmt;
}

int AudioEncode::getSampleRate()
{
    return mSampleRate;
}
