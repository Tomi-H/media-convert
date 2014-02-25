/*
 * VideoEncode.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef VIDEOENCODE_H_
#define VIDEOENCODE_H_
#include "MediaEncode.h"
#include "ffmpeg.h"

class VideoEncode : public MediaEncode
{
    public:
        VideoEncode();
        virtual ~VideoEncode();

        virtual int open(AVCodecID codec_id, int flags);
        virtual int encode(AVPacket * pkt_ptr, const uint8_t ** data, int count);
        virtual void close();

        virtual void setBitRate(int bit_rate);
        virtual void setWidth(int width);
        virtual void setHeight(int height);
        virtual void setPixFmt(AVPixelFormat pix_fmt);

        virtual int getWidth();
        virtual int getHeight();
        virtual AVPixelFormat getPixFmt();
        virtual int getBitRate();

    private:
        int getDefaultWidth()            { return 640; }
        int getDefaultHeight()           { return 480; }
        AVPixelFormat getDefaultFormat()           { return AV_PIX_FMT_YUV420P; }
        AVRational getDefaultTimeBase()  { AVRational r = {0, 1}; return r; }
        int getDefaultGroupSize()        { return 12; }

        int doEncode(AVPacket * pkt, const uint8_t ** data, int size);

    private:
        int mWidth;
        int mHeight;
        AVPixelFormat mPixFormat;
        AVRational mTimeBase;
        int mGroupSize;
        int mState;
        AVFrame * mFrame;
        AVPicture * mPicture;
        int mFlags;
};

#endif /* VIDEOENCODE_H_ */
