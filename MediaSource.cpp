/*
 * MediaSource.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#include "MediaSource.h"
#include <assert.h>

MediaSource::MediaSource()
{
    mFmtCtx = NULL;
    mVideoStream = NULL;
    mAudioStream = NULL;
    mVideoCodecCtx = NULL;
    mAudioCodecCtx = NULL;
    mAudioCodec = NULL;
    mVideoCodec = NULL;
    mVideoStreamIndex = -1;
    mAudioStreamIndex = -1;
}

MediaSource::~MediaSource()
{
}

int MediaSource::open(const char * filename, int show)
{
    mFmtCtx = avformat_alloc_context();
    int ret = avformat_open_input(&mFmtCtx, filename, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "open souce file failed: %s\n", av_err2str(ret));
        avformat_free_context(mFmtCtx);
        return -1;
    }

    avformat_find_stream_info(mFmtCtx, NULL);
    if (show) {
        av_dump_format(mFmtCtx, 0, filename, 0);
    }

    mAudioStreamIndex = av_find_best_stream(mFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &mAudioCodec, 0);
    if (mAudioStreamIndex == AVERROR_STREAM_NOT_FOUND &&
        mAudioStreamIndex == AVERROR_DECODER_NOT_FOUND) {
        fprintf(stderr, "Could not find source audio stream: %s\n", av_err2str(mAudioStreamIndex));
    } else {
        mAudioStream = mFmtCtx->streams[mAudioStreamIndex];
        mAudioCodecCtx = mAudioStream->codec;
       // mAudioCodec = mAudioCodecCtx->codec;
        int ret = avcodec_open2(mAudioCodecCtx,mAudioCodec,NULL);
        if (ret < 0) {
            fprintf(stderr, "open source audio codec failed: %s\n", av_err2str(ret));
            exit(1);
        }
    }

    mVideoStreamIndex = av_find_best_stream(mFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &mVideoCodec, 0);
    if (mVideoStreamIndex == AVERROR_STREAM_NOT_FOUND &&
        mVideoStreamIndex == AVERROR_DECODER_NOT_FOUND) {
        fprintf(stderr, "Could not find source video stream: %s\n", av_err2str(mVideoStreamIndex));
    } else {
        mVideoStream = mFmtCtx->streams[mVideoStreamIndex];
        mVideoCodecCtx = mVideoStream->codec;
        //mVideoCodec = mVideoCodecCtx->codec;
        int ret = avcodec_open2(mVideoCodecCtx,mVideoCodec,NULL);
        if (ret < 0) {
            fprintf(stderr, "open source video codec failed: %s\n", av_err2str(ret));
            exit(1);
        }
    }

    return 0;
}

bool MediaSource::isAudioPacket(const AVPacket *pkt)
{
    if (pkt->stream_index == mAudioStreamIndex)
        return true;
    return false;
}

bool MediaSource::isVideoPacket(const AVPacket * pkt)
{
    if (pkt->stream_index == mVideoStreamIndex)
        return true;
    return false;
}

AVMediaType MediaSource::getMediaType(const AVPacket * pkt)
{
    if (pkt->stream_index == mAudioStreamIndex)
        return AVMEDIA_TYPE_AUDIO;

    if (pkt->stream_index == mVideoStreamIndex)
        return AVMEDIA_TYPE_VIDEO;

    return AVMEDIA_TYPE_UNKNOWN;
}

AVRational MediaSource::getAudioStreamTimeBase()
{
    if (mAudioStream)
        return mAudioStream->time_base;

    AVRational ret = {0, 0};
    return ret;
}

AVRational MediaSource::getVideoStreamTimeBase()
{
    if (mVideoStream)
        return mVideoStream->time_base;

    AVRational ret = {0, 0};
    return ret;
}

int MediaSource::read(AVPacket * pkt)
{
   assert(pkt);
   return av_read_frame(mFmtCtx, pkt);
}

int MediaSource::decode(AVFrame * frame, AVPacket * packet)
{
    assert(frame);
    assert(packet);
    int got_frame = 0;
    int ret = 0;

    if (isAudioPacket(packet)) {
        ret = avcodec_decode_audio4(mAudioCodecCtx, frame, &got_frame, packet);
        if (ret < 0) {
            fprintf(stderr, "decode audio packet failed: %s\n", av_err2str(ret));
            return ret;
        }
        if (!got_frame)
            return 0;
        return ret;
    } else if (isVideoPacket(packet)) {
        ret = avcodec_decode_video2(mVideoCodecCtx, frame, &got_frame, packet);
        if (ret < 0) {
            fprintf(stderr, "decode video packet failed: %s\n", av_err2str(ret));
            return ret;
        }
        if (!got_frame)
            return 0;
        return ret;
    }

    return 0;
}

void MediaSource::close()
{
    if (mAudioCodecCtx && avcodec_is_open(mAudioCodecCtx)) {
        avcodec_close(mAudioCodecCtx);
    }
    if (mVideoCodecCtx && avcodec_is_open(mVideoCodecCtx)) {
        avcodec_close(mVideoCodecCtx);
    }

    avformat_close_input(&mFmtCtx);
    avformat_free_context(mFmtCtx);
}


const AVFormatContext * MediaSource::getFmtCtx()
{
    return mFmtCtx;
}

/**
 *    audio
 */
uint64_t MediaSource::getChannelLayout()
{
    if (mAudioCodecCtx)
        return mAudioCodecCtx->channel_layout;
    return 0;
}

AVSampleFormat MediaSource::getSampleFormat()
{
    if (mAudioCodecCtx)
        return mAudioCodecCtx->sample_fmt;
    return AV_SAMPLE_FMT_NONE;
}

int MediaSource::getSampleRate()
{
    if (mAudioCodecCtx)
        return mAudioCodecCtx->sample_rate;
    return 0;
}

void MediaSource::setChannelLayout(uint64_t channel_layout)
{
    mOutChannelLayout = channel_layout;
}

void MediaSource::setSampleFormat(AVSampleFormat sample_fmt)
{
    mOutSampleFmt = sample_fmt;
}

void MediaSource::setSampleRate(int sample_rate)
{
    mOutSampleRate = sample_rate;
}

/**
 * video
 */

int MediaSource::getWidth()
{
    if (mVideoCodecCtx)
        return mVideoCodecCtx->width;
    return 0;
}

int MediaSource::getHeight()
{
    if (mVideoCodecCtx)
        return mVideoCodecCtx->height;
    return 0;
}

AVPixelFormat MediaSource::getPixFormat()
{
    if (mVideoCodecCtx)
        return mVideoCodecCtx->pix_fmt;
    return AV_PIX_FMT_NONE;
}

void MediaSource::setWidth(int width)
{
    mOutWidth = width;
}

void MediaSource::setHeight(int height)
{
    mOutHeight = height;
}

void MediaSource::setPixFormat(AVPixelFormat pix_fmt)
{
    mOutPixFmt = pix_fmt;
}
