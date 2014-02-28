/*
 * MediaEncode.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef MEDIAENCODE_H_
#define MEDIAENCODE_H_
#include "ffmpeg.h"

class MediaEncode
{
    public:
        MediaEncode();
        virtual ~MediaEncode();

        virtual int open(AVCodecID codec_id, int flags) = 0;

        virtual int isOpen();

        /**
         * Encode the data into packet
         * @param pkt_ptr the AVPacket used to store the encoded data
         * @param data the source data
         * @param count for audio, count represent the number of audio samples;
         *              for video, it's unused.
         * @return
         */
        virtual int encode(AVPacket *pkt_ptr, const uint8_t ** data, int count) = 0;
        virtual void close() = 0;

        /*
         *   For Audio
         */
        virtual void setChannelLayout(uint64_t channel_layout) {};
        virtual void setSampleFormat(AVSampleFormat sample_fmt) {};
        virtual void setSampleRate(int sample_rate) {};

        virtual uint64_t getChannelLayout() { return 0; }
        virtual AVSampleFormat getSampleFormat() { return AV_SAMPLE_FMT_NONE; }
        virtual int getSampleRate() { return 0; }

        /*
         *   For Video
         */
        virtual void setWidth(int width) {}
        virtual void setHeight(int height) {}
        virtual void setPixFmt(AVPixelFormat pix_fmt) {}

        virtual int getWidth() { return 0; }
        virtual int getHeight() { return 0; }
        virtual AVPixelFormat getPixFmt() { return AV_PIX_FMT_YUV420P; }

        void setBitRate(int bit_rate);
        int getBitRate();

        AVCodec * getCodec() { return mCodec; }

    protected:
        /**
         *   For audio
         */
        AVSampleFormat checkSampleFmt(AVSampleFormat sample_fmt);
        uint64_t checkChannelLayout(uint64_t channel_layout);

        uint64_t defaultChannelLayout();
        AVSampleFormat defaultSampleFmt();
        int defaultSampleRate();
        int defaultBitRate();


        /**
         *   For video
         */
        int defaultWidth();
        int defaultHeight();
        AVPixelFormat defaultPixelFormat();

        AVCodecContext * mCodecCtx;
        AVCodec * mCodec;
        int mBitRate;

#define ENCODE_STATE_INIT    1
#define ENCODE_STATE_RUN     2
#define ENCODE_STATE_CHANGED 3
#define ENCODE_STATE_CLOSE   4
};

#endif /* MEDIAENCODE_H_ */
