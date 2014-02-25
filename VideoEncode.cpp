/*
 * VideoEncode.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#include "VideoEncode.h"
#include <assert.h>

VideoEncode::VideoEncode()
{
    mState = ENCODE_STATE_INIT;
    mWidth = getDefaultWidth();
    mHeight = getDefaultHeight();
    mPixFormat = getDefaultFormat();
    mTimeBase = getDefaultTimeBase();
    mGroupSize = getDefaultGroupSize();
    mFrame = NULL;
    mPicture = NULL;
    mFlags = 0;
}

VideoEncode::~VideoEncode()
{

}

int VideoEncode::open(AVCodecID codec_id, int flags)
{
    mCodec = avcodec_find_encoder(codec_id);
    if (!mCodec) {
        fprintf(stderr, "find video encoder failed.\n");
        return -1;
    }

    mCodecCtx = avcodec_alloc_context3(mCodec);
    if (!mCodecCtx) {
        fprintf(stderr, "allocate codec context failed.\n");
        return -1;
    }

    mFlags |= flags;

    return 0;
}

int VideoEncode::encode(AVPacket * pkt, const uint8_t ** data, int size)
{
    assert(pkt);
    assert(data);

    if (!mFrame) {
        mFrame = avcodec_alloc_frame();
        mFrame->pts = 0;
    }

    if (!mPicture) {
        mPicture = new AVPicture;
        int ret = avpicture_alloc(mPicture, mPixFormat, mWidth, mHeight);
        if (ret < 0) {
            fprintf(stderr, "allocate picture buffer failed.\n");
            exit(1);
        }
    }

    if (mState == ENCODE_STATE_CHANGED ||
        mState == ENCODE_STATE_INIT) {
        mCodecCtx->width = mWidth;
        mCodecCtx->height = mHeight;
        mCodecCtx->pix_fmt = mPixFormat;
        mCodecCtx->gop_size = mGroupSize;
        mCodecCtx->bit_rate = mBitRate;
        mCodecCtx->time_base = mTimeBase;
        mCodecCtx->flags |= mFlags;
    }

    mState = ENCODE_STATE_RUN;
    if (!avcodec_is_open(mCodecCtx))
        avcodec_open2(mCodecCtx, mCodec, NULL);

    return doEncode(pkt, data, size);
}

int VideoEncode::doEncode(AVPacket * pkt, const uint8_t ** data, int size)
{
    int got_pkt = 0;

    int bytes = avpicture_fill(mPicture, data[0], mPixFormat, mWidth, mHeight);
    if (bytes < 0) {
        fprintf(stderr, "fill video data into frame failed.\n");
        exit(1);
    }

    mFrame = (AVFrame *)mPicture;
    mFrame->pts++;
    int ret = avcodec_encode_video2(mCodecCtx, pkt, mFrame, &got_pkt);
    if (ret < 0) {
        fprintf(stderr, "encode video failed: %s\n", av_err2str(ret));
        return ret;
    }

    if (!got_pkt)
        avcodec_encode_video2(mCodecCtx, pkt, NULL, &got_pkt);

    return 0;
}

void VideoEncode::close()
{
    if (mCodecCtx && avcodec_is_open(mCodecCtx)) {
        avcodec_close(mCodecCtx);
    }
    av_free(mCodecCtx);
    avcodec_free_frame(&mFrame);
    delete mPicture;
}

void VideoEncode::setBitRate(int bit_rate)
{
    if (mState == ENCODE_STATE_INIT ||
        mState == ENCODE_STATE_CHANGED) {
        mBitRate = bit_rate;
    }
}

void VideoEncode::setWidth(int width)
{
    if (mState == ENCODE_STATE_INIT ||
        mState == ENCODE_STATE_CHANGED) {
        mWidth = width;
    }
}

void VideoEncode::setHeight(int height)
{
    if (mState == ENCODE_STATE_RUN)
        return;
    mHeight = height;
}

void VideoEncode::setPixFmt(AVPixelFormat pix_fmt)
{
    if (mState == ENCODE_STATE_RUN)
        return;
    mPixFormat = pix_fmt;
}

int VideoEncode::getWidth()
{
    return mWidth;
}

int VideoEncode::getHeight()
{
    return mHeight;
}

AVPixelFormat VideoEncode::getPixFmt()
{
    return mPixFormat;
}

int VideoEncode::getBitRate()
{
    return mBitRate;
}
