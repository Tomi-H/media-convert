/*
 * MediaSource.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef MEDIASOURCE_H_
#define MEDIASOURCE_H_
#include "ffmpeg.h"
#include "AudioResample.h"
#include "VideoScale.h"

class MediaSource
{
    public:
        MediaSource();
        virtual ~MediaSource();

        int open(const char * filename, int show);
        int read(AVPacket * pkt_ptr);

        /**
         * decode AVPacket, and store the data into AVFrame.
         * for audio packet, the caller should check the return value
         * to packet->size, return value is small than packet->size, the
         * caller should call decode() function again.
         *
         * @param frame_ptr
         * @param pkt_ptr
         * @return ERRNO, if decode AVPacket failed;
         *    GOT_FRAME_FAILED, if the decoder didn't get the
         *    AVPacket data(need to call decode again).
         */
        int decode(AVFrame * frame_ptr, AVPacket * pkt_ptr);
        void close();
        bool isAudioPacket(const AVPacket * pkt_ptr);
        bool isVideoPacket(const AVPacket * pkt_ptr);
        AVMediaType getMediaType(const AVPacket * pkt_ptr);

        const AVFormatContext * getFmtCtx();
        AVRational getAudioStreamTimeBase();
        AVRational getVideoStreamTimeBase();

        /**
         *     For Audio
         */
        uint64_t getChannelLayout();
        AVSampleFormat getSampleFormat();
        int getSampleRate();

        void setChannelLayout(uint64_t channel_layout);
        void setSampleFormat(AVSampleFormat sample_fmt);
        void setSampleRate(int sample_rate);

        /**
         *    For Video
         */
        int getWidth();
        int getHeight();
        AVPixelFormat getPixFormat();

        void setWidth(int width);
        void setHeight(int height);
        void setPixFormat(AVPixelFormat pix_fmt);


    private:
        AVFormatContext * mFmtCtx;
        AVStream * mVideoStream;
        AVStream * mAudioStream;
        int mVideoStreamIndex;
        int mAudioStreamIndex;
        AVCodecContext * mVideoCodecCtx;
        AVCodecContext * mAudioCodecCtx;
        AVCodec * mVideoCodec;
        AVCodec * mAudioCodec;
        AudioResample * mResample;
        VideoScale * mScale;

        uint64_t mOutChannelLayout;
        AVSampleFormat mOutSampleFmt;
        int mOutSampleRate;

        int mOutWidth;
        int mOutHeight;
        AVPixelFormat mOutPixFmt;
};

#endif /* MEDIASOURCE_H_ */
