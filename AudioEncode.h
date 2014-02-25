/*
 * AudioEncode.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef AUDIOENCODE_H_
#define AUDIOENCODE_H_
#include "ffmpeg.h"
#include "MediaEncode.h"

class AudioEncode : public MediaEncode
{
    public:
        AudioEncode();
        virtual ~AudioEncode();

        virtual int open(AVCodecID codec_id, int flags);
       // virtual int write(AVPacket * pkt_ptr);
       // virtual int write(uint8_t ** data, int nb_samples);
        virtual int encode(AVPacket * pkt_ptr, const uint8_t ** data, int nb_samples);
        virtual void close();

        virtual void setChannelLayout(uint64_t channel_layout);
        virtual void setSampleFormat(AVSampleFormat sample_fmt);
        virtual void setSampleRate(int sample_rate);
        virtual uint64_t getChannelLayout();
        virtual AVSampleFormat getSampleFormat();
        virtual int getSampleRate();
    private:
        virtual int defaultBitRate();
        int doEncode(AVPacket * pkt_ptr, const uint8_t ** data, int nb_samples);

    private:
//        AVCodecContext * mCodecCtx;
 //       AVCodec * mCodec;
        uint64_t mChannelLayout;
        int mChannelNum;
        int mMaxSamplesCount;
        AVSampleFormat mSampleFmt;
        int mSampleRate;
        int mFlags;
        int mState;
        AVAudioFifo * mFifo;
        uint8_t ** mData;
        AVFrame * mFrame;

};

#endif /* AUDIOENCODE_H_ */
